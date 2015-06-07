/*                               -*- Mode: C -*-
 * game.c --
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Mon Dec 30 00:11:06 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 21:49:58 1997
 * Update Count    : 100
 * Status          : Unknown, Use with caution!
 *
 * (C) Copyright 1996, 1997 by Thomas Biskup.
 * All Rights Reserved.
 *
 * This software may be distributed only for educational, research and
 * other non-proft purposes provided that this copyright notice is left
 * intact.  If you derive a new game from these sources you also are
 * required to give credit to Thomas Biskup for creating them in the first
 * place.  These sources must not be distributed for any fees in excess of
 * $3 (as of January, 1997).
 */


#include "MISC.H"
#include "MONSTER.H"
#include "DUNGEON.H"
#include "PLAYER.H"
#include "SYSDEP.H"
#include "GAME.H"

namespace QHack {
	/*
	 * The main function.
	 */
	bool Game::walk_mode;
	bool Game::walk_in_room;
	short Game::walk_steps;

	void Game::play(void)
	{
		coord opx, opy;
		char c;

		/*
		 * Build the current level.
		 *
		 * XXXX: Once it is possible to save/restore the map this no longer
		 *       must be done if the game was started by restoring a save file.
		 */
		Dungeon::d.dl = 0;
		Dungeon::build_map();
		Monster::create_population();
		Monster::build_monster_map();
		Dungeon::d.visited[0] = true;

		/* Initial player position. */
		Dungeon::d.px = Dungeon::d.opx = Dungeon::d.stxu[0];
		Dungeon::d.py = Dungeon::d.opy = Dungeon::d.styu[0];

		/* Initial panel position. */
		Dungeon::d.psx = Dungeon::d.psy = 0;

		/*
		 * Standard stuff.
		 */

		 /* Setup the screen. */
		SysDep::clear_screen();

		do
		{
			/* Print all the new things. */
			update_screen(Dungeon::d.px, Dungeon::d.py);
			Player::update_player_status();

			/* Display the player and center the cursor. */
			Dungeon::map_cursor(Dungeon::d.px, Dungeon::d.py);
			SysDep::set_color(ConsoleColor::White);
			SysDep::prtchar('@');
			Dungeon::map_cursor(Dungeon::d.px, Dungeon::d.py);

			/* Refresh the IO stuff. */
			SysDep::update();

			/* Continue to walk or read a key. */
			if (!walk_mode)
			{
				walk_steps = 0;
				c = SysDep::getkey();
			}

			/* The message line should be cleared in any case. */
			Misc::clear_messages();

			/* Memorize the old PC position. */
			opx = Dungeon::d.px;
			opy = Dungeon::d.py;

			/* Act depending on the last key received. */
			switch (c)
			{
			case 'T':
				Player::adjust_training();
				break;

			case 'o':
				open_door();
				break;

			case '>':
				descend_level();
				break;

			case '<':
				ascend_level();
				/* Quit if necessary. */
				if (Dungeon::d.dl == -1)
					c = 'Q';
				break;

			case 'R':
				redraw();
				break;

			case 'J':
				activate_walk_mode();
				attempt(W);
				break;

			case 'K':
				activate_walk_mode();
				attempt(S);
				break;

			case 'L':
				activate_walk_mode();
				attempt(E);
				break;

			case 'I':
				activate_walk_mode();
				attempt(N);
				break;

			case 'j':
				if (Dungeon::is_open(Dungeon::d.px - 1, Dungeon::d.py))
					Dungeon::d.px--;
				break;

			case 'l':
				if (Dungeon::is_open(Dungeon::d.px + 1, Dungeon::d.py))
					Dungeon::d.px++;
				break;

			case 'i':
				if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py - 1))
					Dungeon::d.py--;
				break;

			case 'k':
				if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py + 1))
					Dungeon::d.py++;
				break;

			default:
				break;
			}

			Dungeon::d.opx = opx;
			Dungeon::d.opy = opy;

			/* Remove the player character from the screen. */
			Dungeon::print_tile(opx, opy);
		} while (c != 'Q');
	}


	/*
	 * Update the screen based upon the current player position.  Panel scrolling
	 * is also handled in this function.
	 */

	void Game::update_screen(coord x, coord y)
	{
		coord sx, sy, px, py, opsx, opsy;

		/* Find the current general section. */
		Dungeon::get_current_section_coordinates(Dungeon::d.px, Dungeon::d.py, &sx, &sy);

		/* Memorize the old panel view. */
		opsx = Dungeon::d.psx;
		opsy = Dungeon::d.psy;

		/* Adjust the panel view. */
		while (sx < Dungeon::d.psx)
			Dungeon::d.psx--;
		while (Dungeon::d.psx + 4 < sx)
			Dungeon::d.psx++;
		while (sy < Dungeon::d.psy)
			Dungeon::d.psy--;
		while (Dungeon::d.psy + 1 < sy)
			Dungeon::d.psy++;

		/* Repaint the whole screen map if necessary. */
		if (opsx != Dungeon::d.psx || opsy != Dungeon::d.psy)
			Dungeon::paint_map();

		/* Make the immediate surroundings known. */
		for (px = x - 1; px <= x + 1; px++)
			for (py = y - 1; py <= y + 1; py++)
			Dungeon::know(px, py);

		/* Check whether the PC is in a room or not. */
		Dungeon::get_current_section(Dungeon::d.px, Dungeon::d.py, &sx, &sy);

		/* Make rooms known. */
		if (sx != -1 && sy != -1)
			Dungeon::know_section(sx, sy);
	}



	/*
	 * Try to walk into a given direction.  This is used by the walk-mode when
	 * the player tries to run into a given direction.
	 *
	 * Walk-mode will be deactivated if...
	 * ...the surroundings change in a major way (more or less obstacles around
	 *    the player compared to the last step).
	 * ...an intersection is reached in a tunnel.
	 * ...a room is entered or left.
	 */

	void Game::attempt(byte dir)
	{
		coord sx1, sy1, sx2, sy2;
		byte cn, cw, ce, cs;
		static byte old_cn, old_cw, old_ce, old_cs;

		Dungeon::get_current_section(Dungeon::d.px, Dungeon::d.py, &sx1, &sy1);

		/*
		 * Check whether running should be stopped.
		 */

		if (walk_steps || walk_in_room)
		{
			/* Count the possible ways. */
			cn = (Dungeon::might_be_open(Dungeon::d.px, Dungeon::d.py - 1) && (Dungeon::d.py - 1 != Dungeon::d.opy)) ? 1 : 0;
			cs = (Dungeon::might_be_open(Dungeon::d.px, Dungeon::d.py + 1) && (Dungeon::d.py + 1 != Dungeon::d.opy)) ? 1 : 0;
			cw = (Dungeon::might_be_open(Dungeon::d.px - 1, Dungeon::d.py) && (Dungeon::d.px - 1 != Dungeon::d.opx)) ? 1 : 0;
			ce = (Dungeon::might_be_open(Dungeon::d.px + 1, Dungeon::d.py) && (Dungeon::d.px + 1 != Dungeon::d.opx)) ? 1 : 0;

			/* Check... */
			if (walk_in_room)
			{
				/* In rooms we simply check the general look of the surroundings. */
				switch (walk_steps)
				{
				case 0:
					/* One free step in any case. */
					break;

				default:
					/* Check the surroundings. */
					walk_mode &= (cn == old_cn && cs == old_cs &&
						cw == old_cw && ce == old_ce);

				case 1:
					/* Memorize the surroundings. */
					old_cn = cn;
					old_cs = cs;
					old_cw = cw;
					old_ce = ce;
					break;
				}

				/* Check whether we are still in a room. */
				walk_mode &= (sx1 != -1 && sy1 != -1);

				/* Check for special features. */
				if (walk_steps)
					walk_mode &= (Dungeon::tile_at(Dungeon::d.px, Dungeon::d.py) == FLOOR);
			}
			else
				/* Check for intersections. */
				switch (dir)
				{
				case N:
					walk_mode &= (cw + ce < 2 && cn + cw < 2 && cn + ce < 2);
					break;
				case S:
					walk_mode &= (cw + ce < 2 && cs + cw < 2 && cs + ce < 2);
					break;
				case W:
					walk_mode &= (cn + cs < 2 && cw + cs < 2 && cw + cn < 2);
					break;
				case E:
					walk_mode &= (cn + cs < 2 && ce + cs < 2 && ce + cn < 2);
					break;

				default:
					break;
				}

			if (!walk_mode)
				return;
		}

		/*
		 * Walk.  This function also manages to walk around corners in a tunnel.
		 */

		switch (dir)
		{
		case N:
			if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py - 1))
				Dungeon::d.py--;
			else if (Dungeon::is_open(Dungeon::d.px - 1, Dungeon::d.py) && Dungeon::d.px - 1 != Dungeon::d.opx)
				Dungeon::d.px--;
			else if (Dungeon::is_open(Dungeon::d.px + 1, Dungeon::d.py) && Dungeon::d.px + 1 != Dungeon::d.opx)
				Dungeon::d.px++;
			else
				walk_mode = false;
			break;

		case S:
			if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py + 1))
				Dungeon::d.py++;
			else if (Dungeon::is_open(Dungeon::d.px - 1, Dungeon::d.py) && Dungeon::d.px - 1 != Dungeon::d.opx)
				Dungeon::d.px--;
			else if (Dungeon::is_open(Dungeon::d.px + 1, Dungeon::d.py) && Dungeon::d.px + 1 != Dungeon::d.opx)
				Dungeon::d.px++;
			else
				walk_mode = false;
			break;

		case E:
			if (Dungeon::is_open(Dungeon::d.px + 1, Dungeon::d.py))
				Dungeon::d.px++;
			else if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py + 1) && Dungeon::d.py + 1 != Dungeon::d.opy)
				Dungeon::d.py++;
			else if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py - 1) && Dungeon::d.py - 1 != Dungeon::d.opy)
				Dungeon::d.py--;
			else
				walk_mode = false;
			break;

		case W:
			if (Dungeon::is_open(Dungeon::d.px - 1, Dungeon::d.py))
				Dungeon::d.px--;
			else if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py + 1) && Dungeon::d.py + 1 != Dungeon::d.opy)
				Dungeon::d.py++;
			else if (Dungeon::is_open(Dungeon::d.px, Dungeon::d.py - 1) && Dungeon::d.py - 1 != Dungeon::d.opy)
				Dungeon::d.py--;
			else
				walk_mode = false;
			break;

		default:
			break;
		}

		/* Find the new section. */
		Dungeon::get_current_section(Dungeon::d.px, Dungeon::d.py, &sx2, &sy2);

		/* Entering/leaving a room will deactivate walk-mode. */
		if (walk_steps)
			walk_mode &= (sx1 == sx2 && sy1 == sy2);

		/* Increase the number of steps actually walked. */
		walk_steps++;
	}




	/*
	 * Redraw the whole screen.
	 */

	void Game::redraw(void)
	{
		Misc::clear_messages();
		Dungeon::paint_map();
		Player::update_necessary = true;
		Player::update_player_status();
		SysDep::update();
	}



	/*
	 * Switch between dungeon levels.
	 */

	void Game::modify_dungeon_level(byte mod)
	{
		/* Modify the actual dungeon level. */
		Dungeon::d.dl += mod;

		/* Build the current dungeon map from the general description. */
		Dungeon::build_map();

		/* Determine monster frequencies for the current dungeon level. */
		Monster::initialize_monsters();

		/*
		 * If a level is entered for the first time a new monster population
		 * will be generated and the player receives a little bit of experience
		 * for going where nobody went before (or at least managed to come back).
		 */
		if (!Dungeon::d.visited[Dungeon::d.dl])
		{
			Monster::create_population();
			Dungeon::d.visited[Dungeon::d.dl] = true;

			/* Score some experience for exploring unknown depths. */
			Player::score_exp(Dungeon::d.dl);
		}

		/* Place monsters in the appropriate positions. */
		Monster::build_monster_map();

		/* Paint the new map. */
		Dungeon::paint_map();
	}


	/*
	 * Continue one level downwards.
	 */

	void Game::descend_level(void)
	{
		if (Dungeon::tile_at(Dungeon::d.px, Dungeon::d.py) != STAIR_DOWN)
			Misc::You("don't see any stairs leading downwards.");
		else
		{
			modify_dungeon_level(+1);
			Dungeon::d.px = Dungeon::d.stxu[Dungeon::d.dl];
			Dungeon::d.py = Dungeon::d.styu[Dungeon::d.dl];
		}
	}



	/*
	 * Continue one level upwards.
	 */

	void Game::ascend_level(void)
	{
		if (Dungeon::tile_at(Dungeon::d.px, Dungeon::d.py) != STAIR_UP)
			Misc::You("don't see any stairs leading upwards.");
		else
		{
			if (Dungeon::d.dl)
			{
				modify_dungeon_level(-1);
				Dungeon::d.px = Dungeon::d.stxd[Dungeon::d.dl];
				Dungeon::d.py = Dungeon::d.styd[Dungeon::d.dl];
			}
			else
				/* Leave the dungeon. */
				Dungeon::d.dl = -1;
		}
	}



	/*
	 * Handle doors.
	 */

	void Game::open_door(void)
	{
		coord tx, ty;

		/* Find the door. */
		Misc::get_target(Dungeon::d.px, Dungeon::d.py, &tx, &ty);

		/* Command aborted? */
		if (tx == -1 || ty == -1)
			return;

		/* Check the door. */
		switch (Dungeon::tile_at(tx, ty))
		{
		case OPEN_DOOR:
			Misc::Message("This door is already open.");
			break;

		case CLOSED_DOOR:
			Misc::You("open the door.");
			Dungeon::change_door(tx, ty, OPEN_DOOR);
			break;

		case LOCKED_DOOR:
			Misc::Message("This door seems to be locked.");
			break;

		default:
			Misc::Message("Which door?");
			break;
		}
	}


	/*
	 * Activate the walk mode and determine whether we are walking through a
	 * room.
	 */

	void Game::activate_walk_mode(void)
	{
		coord x, y;

		/* Activate walking. */
		walk_mode = true;

		/* Check for a room. */
		Dungeon::get_current_section(Dungeon::d.px, Dungeon::d.py, &x, &y);
		walk_in_room = (x != -1 && y != -1);
	}
}