/*                               -*- Mode: C -*- 
 * monster.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Mon Dec 30 18:08:50 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 20:12:30 1997
 * Update Count    : 24
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

#ifndef __MONSTER__

#define __MONSTER__

#include "CONFIG.H"
namespace QHack {

	/*
	 * Constants for various monster states.
	 */

	enum monster_state
	{
		ASLEEP, NEUTRAL, ANGRY
	};


	/*
	 * Constants for monster frequencies.
	 */

	enum monster_rarity
	{
		UNIQUE = 0, VERY_RARE = 100, RARE = 500, UNCOMMON = 1000, COMMON = 2000,
		VERY_COMMON = 3000
	};


	/*
	 * A structure for a given monster.
	 */

	struct monster
	{
		/* Is the entry occupied by a monster? */
		_BOOL used;

		/* Monster type. */
		byte midx;

		/* Position on the map. */
		coord x, y;

		/* Hitpoint data. */
		int16 hp, max_hp;

		/* The current state (see above). */
		byte state;
	};



	/*
	 * The structure for a basic monster.
	 */

	struct monster_def
	{
		/* The character symbol. */
		char symbol;

		/* The color of the monster. */
		ConsoleColor color;

		/* It's name. */
		char *name;

		/* Armor class. */
		byte ac;

		/* Initial hitpoints. */
		char *hits;

		/* Number of attacks. */
		byte attacks;

		/* To-hit bonus. */
		byte to_hit;

		/* Damage dice. */
		char *damage;

		/* Frequency for the basic level. */
		int16 rarity;
	};


	struct monster_struct
	{
		/* Index to the first empty monster slow; later ones use 'midx'. */
		byte eidx[MAX_DUNGEON_LEVEL];

		/* The monster slots for each level. */
		struct monster m[MAX_DUNGEON_LEVEL][MONSTERS_PER_LEVEL];
	};


	/* The global monster structure. */
	extern struct monster_struct m;



	/*
	 * Global functions.
	 */

	public class Monster
	{
	public:
		static _BOOL los(coord, coord);
		static _BOOL is_monster_at(coord, coord);

		static byte monster_tile(byte);
		static ConsoleColor monster_color(byte);

		static struct monster *get_monster_at(coord, coord);

		static void init_monsters(void);
		static void initialize_monsters(void);
		static void build_monster_map(void);
		static void create_monster_in(byte);
		static void create_population(void);
		static void move_monsters(void);

	private:
		static byte random_monster_type(void);
		static byte monster_level(byte midx);
		static int16 monster_rarity(byte midx);
		static byte max_monster(void);
		static byte get_monster_index(void);
		static int16 mhits(byte midx);
		static void get_monster_coordinates(coord *x, coord *y);
		
		static int Monster::lmod[14];
	};


}
#endif
