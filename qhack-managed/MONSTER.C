/*                               -*- Mode: C -*-
 * monster.c --
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Mon Dec 30 18:11:48 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 20:16:01 1997
 * Update Count    : 56
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

 /*
  * Design notes: QHack assumes a maximum that the initial level will have
  * 4 different monster types.  Per level descended another two new monsters
  * become available.
  */


  /*
   * Includes.
   */

#include "error.h"
#include "MISC.H"
#include "monster.h"
#include "DUNGEON.H"
#include "SYSDEP.H"
namespace QHack {


	/*
	 * Local constants.
	 */

	 /* The total number of monsters. */
#define MAX_MONSTER 4

/*
 * Global variables.
 */

	struct monster_struct m;


	/*
	 * Local variables.
	 */


	 /* The complete monster list for the game. */
	struct monster_def md[MAX_MONSTER] =
	{
	  {'k', ConsoleColor::Green, "kobold", 14, "1d4", 1, 0, "1d6", COMMON},
	  {'r', ConsoleColor::DarkYellow, "rat", 12, "1d3", 1, 0, "1d3", COMMON},
	  {'g', ConsoleColor::Blue, "goblin", 13, "1d8", 1, 0, "1d6", COMMON},
	  {'x', ConsoleColor::Yellow, "lightning bug", 18, "2d3", 1, +1, "1d4", RARE}
	};

	/* The dynamic index map for one monster level. */
	static byte midx[MAP_W][MAP_H];

	/* The total rarity for monsters; dependent on the current level. */
	static uint total_rarity;


	/*
	 * Initialize the monster structures.  Basically we have to notify
	 * all slots that they are empty.  Also the general index map needs
	 * to be initialized.
	 */

	void Monster::init_monsters(void)
	{
		coord i, j;

		for (i = 0; i < MAX_DUNGEON_LEVEL; i++)
		{
			/* The first empty monster slot. */
			m.eidx[i] = 0;

			/* Initially all slots are empty. */
			for (j = 0; j < MONSTERS_PER_LEVEL - 1; j++)
			{
				m.m[i][j].used = false;
				m.m[i][j].midx = j + 1;
			}

			/* The last one points to 'no more slots'. */
			m.m[i][MONSTERS_PER_LEVEL - 1].midx = -1;
			m.m[i][MONSTERS_PER_LEVEL - 1].used = false;
		}

		/* Initialize the monster index map as 'empty'. */
		for (i = 0; i < MAP_W; i++)
			for (j = 0; j < MAP_H; j++)
			midx[i][j] = -1;
	}



	/*
	 * Create the monster map for a given dungeon level.
	 */

	void Monster::build_monster_map(void)
	{
		coord x, y;

		/* Initialize the monster index map as 'empty'. */
		for (x = 0; x < MAP_W; x++)
			for (y = 0; y < MAP_H; y++)
			midx[x][y] = -1;

		/* Setup all monster indices. */
		for (x = 0; x < MONSTERS_PER_LEVEL; x++)
			if (m.m[Dungeon::d.dl][x].used)
			midx[m.m[Dungeon::d.dl][x].x][m.m[Dungeon::d.dl][x].y] = x;
	}



	/*
	 * Create an initial monster population for a given level.
	 */

	void Monster::create_population(void)
	{
		byte m;

		/* Initialize the basic monster data. */
		initialize_monsters();

		for (m = 0; m < INITIAL_MONSTER_NUMBER; m++)
		{
			byte midx;

			/* Find a monster index. */
			midx = Monster::get_monster_index();

			/* Paranoia. */
			if (midx == -1)
				Error::Die("Could not create the initial monster population");

			/* Create a new monster. */
			create_monster_in(midx);
		}
	}



	/*
	 * Return the maximum monster number for the current dungeon level.
	 *
	 * XXXX: Since the current monster list is somewhat limited only four
	 *       monsters are available.
	 */
	byte Monster::max_monster(void)
	{
		return Misc::imin(MAX_MONSTER, ((Dungeon::d.dl << 1) + 4));
	}



	/*
	 * Determine the frequency for a given monster.
	 *
	 * This value is level-dependent.  If the monster is out-of-depth (for
	 * QHack this means 'has a lower minimum level than the current dungeon
	 * level) it's frequency will be reduced.
	 */

	int Monster::lmod[14] =
	{
	  100, 90, 80, 72, 64, 56, 50, 42, 35, 28, 20, 12, 4, 1
	};

	short Monster::monster_rarity(byte midx)
	{
		short rarity = md[midx].rarity;
		byte level_diff = Dungeon::d.dl - monster_level(midx);

		return Misc::imax(1, (rarity * Monster::lmod[Misc::imin(13, level_diff)]) / 100);
	}


	/*
	 * Determine the minimum level for a given monster number.
	 */

	byte Monster::monster_level(byte midx)
	{
		if (midx < 4)
			return 0;

		return (midx - 2) >> 1;
	}


	/*
	 * Calculate the frequencies for all available monsters based upon the current
	 * dungeon level.
	 */

	void Monster::initialize_monsters(void)
	{
		byte i;

		total_rarity = 0;

		for (i = 0; i < Monster::max_monster(); i++)
			total_rarity += Monster::monster_rarity(i);
	}



	/*
	 * Determine the index number for a random monster on the current dungeon
	 * level.
	 */

	byte Monster::random_monster_type(void)
	{
		int roll;
		byte i;

		roll = SysDep::rand_long(total_rarity) + 1;
		i = 0;

		while (roll > monster_rarity(i))
		{
			roll -= monster_rarity(i);
			i++;
		}

		return i;
	}



	/*
	 * Create a new monster in a given slot.
	 */

	void Monster::create_monster_in(byte midx)
	{
		/* Adjust the 'empty' index. */
		if (m.eidx[Dungeon::d.dl] == midx)
			m.eidx[Dungeon::d.dl] = m.m[Dungeon::d.dl][midx].midx;

		/* Create the actual monster. */
		m.m[Dungeon::d.dl][midx].used = true;
		m.m[Dungeon::d.dl][midx].midx = random_monster_type();
		get_monster_coordinates(&m.m[Dungeon::d.dl][midx].x, &m.m[Dungeon::d.dl][midx].y);
		m.m[Dungeon::d.dl][midx].hp = m.m[Dungeon::d.dl][midx].max_hp = mhits(m.m[Dungeon::d.dl][midx].midx);
		m.m[Dungeon::d.dl][midx].state = ASLEEP;
	}



	/*
	 * Find coordinates for a new monster.  Some things need to be considered:
	 *  1. Monsters should only be created on 'floor' tiles.
	 *  2. New monsters should not be in LOS of the PC.
	 *  3. New monsters should not be created in spots where another monster is
	 *     standing.
	 */

	void Monster::get_monster_coordinates(coord *x, coord *y)
	{
		do
		{
			*x = SysDep::rand_int(MAP_W);
			*y = SysDep::rand_int(MAP_H);
		} while (Dungeon::tile_at(*x, *y) != FLOOR ||
			Monster::los(*x, *y) ||
			midx[*x][*y] != -1);
	}



	/*
	 * Return an initial hitpoint number for a monster of a given type.
	 */

	short Monster::mhits(byte midx)
	{
		return Misc::dice(md[midx].hits);
	}



	/*
	 * Return the first potentially empty monster slot.
	 */

	byte Monster::get_monster_index(void)
	{
		return m.eidx[Dungeon::d.dl];
	}



	/*
	 * Check whether a PC is able to see a position.
	 */

	bool Monster::los(coord x, coord y)
	{
		coord sx, sy, psx, psy;

		/* Adjacent to the PC? */
		if (Misc::iabs(x - Dungeon::d.px) <= 1 && Misc::iabs(y - Dungeon::d.py) <= 1)
			return true;

		/* Get the section for the given position. */
		Dungeon::get_current_section(x, y, &sx, &sy);

		/* Get the section for the player. */
		Dungeon::get_current_section(Dungeon::d.px, Dungeon::d.py, &psx, &psy);

		/* In the same room section? */
		return (sx == psx && sy == psy && sx != -1);
	}



	/*
	 * Get a monster at a specific position.
	 */

	struct monster *Monster::get_monster_at(coord x, coord y)
	{
		/* Paranoia. */
		if (midx[x][y] == -1)
			Error::Die("No monster to retrieve");

		/* Return the requested monster. */
		return &m.m[Dungeon::d.dl][midx[x][y]];
	}



	/*
	 * Return the color for an indexed monster.
	 */

	ConsoleColor Monster::monster_color(byte midx)
	{
		return md[m.m[Dungeon::d.dl][midx].midx].color;
	}



	/*
	 * Return the picture for an indexed monster.
	 */

	byte Monster::monster_tile(byte midx)
	{
		return md[m.m[Dungeon::d.dl][midx].midx].symbol;
	}



	/*
	 * Determine whether a monster holds a given position.
	 */

	bool Monster::is_monster_at(coord x, coord y)
	{
		return (midx[x][y] != -1);
	}



	/*
	 * Handle the monster turn: movement, combat, etc.
	 */

	void Monster::move_monsters(void)
	{
	}
}