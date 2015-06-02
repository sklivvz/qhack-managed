/*                               -*- Mode: C -*- 
 * dungeon.h -- 
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Sun Dec 29 21:49:27 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 20:46:20 1997
 * Update Count    : 44
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

#ifndef __DUNGEON__

#define __DUNGEON__

 /*
 * The complete dungeon structure
 */

#include "config.h"
#include "PLAYER.H"

namespace QHack {
	/*
	* One section of a level: each dungeon map consists of NSECT_W * NSECT_H
	* sections (see config.g).  A section either contains one room with up
	* to four doors or a tunnel intersection.
	*
	* NOTE: The use of a sectioning approach prevents things like digging, etc.
	*/

	public struct Section
	{
		/* Room available? */
		_BOOL exists;

		/* Room coordinates. */
		coord rx1, rx2, ry1, ry2;

		/* Door positions. */
		coord dx[4], dy[4];

		/* Door types. */
		byte dt[4];
	};

	/*
	* QHack uses one large structure for the complete dungeon.  There are
	* no pointers or other fancy stuff involved since this game should be
	* simple and easy to use.
	*
	* Naturally this prevents some useful things and is not the way a big
	* roguelike game should be written (you sacrifice too much in flexibility),
	* but since it's easy to use I headed into this direction.
	*/

	public struct DungeonComplex
	{
		/* The current level number. */
		byte dl;

		/* NSECT_W * NSECT_H sections for each level. */
		struct QHack::Section s[MAX_DUNGEON_LEVEL][NSECT_W][NSECT_H];

		/* Coordinates for stairways. */
		coord stxu[MAX_DUNGEON_LEVEL];
		coord styu[MAX_DUNGEON_LEVEL];
		coord stxd[MAX_DUNGEON_LEVEL - 1];
		coord styd[MAX_DUNGEON_LEVEL - 1];

		/* Player coordinates. */
		coord px, py;

		/* Last player coordinates. */
		coord opx, opy;

		/* The knowledge map. */
		byte known[MAX_DUNGEON_LEVEL][MAP_BIT_W][MAP_H];

		/* The panel positions. */
		coord psx, psy;

		/* Level was already visited? */
		_BOOL visited[MAX_DUNGEON_LEVEL];

		/* The player data. */
		struct player pc;
	};
	/*
	 * Directions for the dungeon.
	 */

	enum direction
	{
		N, W, S, E, NW, SW, NE, SE, MAX_DIRECTION
	};



	/*
	 * Tiles for the dungeon.
	 */

#define NO_DOOR     -1
#define OPEN_DOOR   '/'
#define CLOSED_DOOR '+'
#define LOCKED_DOOR '\\'
#define ROCK        '#'
#define FLOOR       '.'
#define STAIR_DOWN  '>'
#define STAIR_UP    '<'

	public class Dungeon {

	public:
		static  byte dl;
		static DungeonComplex d;

		static int room_width(coord, coord);
		static int room_height(coord, coord);

		static _BOOL is_open(coord, coord);
		static _BOOL might_be_open(coord, coord);
		static _BOOL is_known(coord, coord);

		static char tile_at(coord, coord);

		static void init_dungeon(void);
		static void build_map(void);
		static void print_map(void);
		static void know(coord, coord);
		static void know_section(coord, coord);
		static void get_current_section(coord, coord, coord *, coord *);
		static void get_current_section_coordinates(coord, coord, coord *, coord *);
		static void paint_map(void);
		static void map_cursor(coord, coord);
		static void print_tile(coord, coord);
		static void print_tile_at_position(coord, coord);
		static void change_door(coord, coord, byte);
		static void set_knowledge(coord, coord, byte);
	private:

		static byte existence_chance;
		static byte map[MAP_W][MAP_H];

		static void create_complete_dungeon(void);
		static void dig_level(void);
		static void dig_section(coord, coord);
		static void dig_stairs(void);
		static void connect_sections(coord, coord, coord, coord, byte);
		static void get_random_section(coord *, coord *);
		static void set_color_for_tile(char);

		static byte rand_door(void);
		static _BOOL dir_possible(coord, coord, byte);

	};
}



#endif
