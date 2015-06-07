/*                               -*- Mode: C -*-
 * sysdep.c --
 * ITIID           : $ITI$ $Header $__Header$
 * Author          : Thomas Biskup
 * Created On      : Sun Dec 29 22:29:49 1996
 * Last Modified By: Thomas Biskup
 * Last Modified On: Thu Jan  9 22:20:44 1997
 * Update Count    : 36
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

#include "SYSDEP.H"
#include <conio.h>
#using <mscorlib.dll>

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace System;

namespace QHack {

	/*
	 * Initialize the random number generator.
	 */

	void SysDep::init_rand(void)
	{
		srand(time(NULL));
	}



	/*
	 * Return a random 8-bit number.
	 */

	byte SysDep::rand_byte(byte max)
	{
		return (byte) (rand() % max);
	}


	/*
	 * Return a random 16-bit number.
	 */

	ushort SysDep::rand_int(ushort max)
	{
		return (ushort) (rand() % max);
	}


	/*
	 * Return a random 32-bit number.
	 */

	uint SysDep::rand_long(uint max)
	{
		return (uint) (rand() % max);
	}


	/*
	 * Initialize the system-dependent I/O stuff.
	 */

	void SysDep::init_io(void)
	{
	}



	/*
	 * Clean up the IO stuff.
	 */

	void SysDep::clean_up_io(void)
	{
		set_color(ConsoleColor::Gray);
		clear_screen();
	}



	/*
	 * Clear the screen.
	 *
	 * The cursor is expected to be at position (0, 0) afterwards.
	 */

	void SysDep::clear_screen(void)
	{
		Console::Clear();
	}


	/*
	 * Set the cursor to a specified scren position.
	 *
	 * Start at position (0, 0).
	 */
	void SysDep::cursor(byte x, byte y)
	{
		Console::SetCursorPosition(x + 1, y + 1);
	}


	/*
	 * Print one character.
	 */
	void SysDep::prtchar(byte c)
	{
		_cprintf("%c", c);
	}



	/*
	 * Read one character from the keyboard without echoing it.
	 */
	char SysDep::getkey(void)
	{
		return (char) _getch();
	}



	/*
	 * Print a string to the screen.
	 */

	void SysDep::prtstr(char *fmt, ...)
	{
		va_list vl;
		static char buffer[1000];

		va_start(vl, fmt);
		vsprintf_s(buffer, sizeof(buffer), fmt, vl);
		va_end(vl);

		_cprintf("%s", buffer);
		update();
	}


	/*
	 * Print a string to the screen using the standard I/O functionality.
	 */

	void SysDep::stdprtstr(char *fmt)
	{
		printf("%s", fmt);
		fflush(stdout);
	}


	/*
	 * Update the screen.
	 */

	void SysDep::update(void)
	{
	}



	/*
	 * Clear the current line up to its end without moving the cursor.
	 */

	void SysDep::clear_to_eol(void)
	{
		int cursorleft = Console::CursorLeft;
		String^ blank = gcnew String(' ', Console::WindowWidth - cursorleft);
		Console::Write(blank);
		Console::SetCursorPosition(cursorleft, Console::CursorTop);
	}


	/*
	 * Set a given color.
	 */
	void SysDep::set_color(ConsoleColor c)
	{
		Console::ForegroundColor = c;
	}
}