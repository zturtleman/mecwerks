/*
===========================================================================
(C) 2010 by Sago007 and Zimmermann Gyula

This file is part of OpenArena and is based upon Mr. Elusive's fuzzy logic
system found in Quake 3 Arena. This file is aimed to make all Quake 3 bots
compatible without any required modifying from the user.

OpenArena is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

OpenArena is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

#if 1
#define GTW W_GAUNTLET
#define SGW W_SHOTGUN
#define MGW W_MACHINEGUN
#define GLW W_GRENADELAUNCHER
#define RLW W_ROCKETLAUNCHER
#define RGW W_RAILGUN
#define BFW W_BFG10K
#define LGW W_LIGHTNING
#define PGW W_PLASMAGUN
#define GRW W_GRAPPLE
#define PXW W_GRENADELAUNCHER
#define NGW W_SHOTGUN
#define CGW W_PLASMAGUN

#else

//MEC
#define W_GAUNTLET GTW
#define W_SHOTGUN SGW
#define W_MACHINEGUN MGW
#define W_GRENADELAUNCHER GLW
#define W_ROCKETLAUNCHER RLW
#define W_RAILGUN RGW
#define W_BFG10K BFW
#define W_LIGHTNING LGW
#define W_PLASMAGUN PGW
#define W_GRAPPLE GRW
#endif

#include "fuzw.c"
