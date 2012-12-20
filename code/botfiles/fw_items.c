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

#if 0
#define FPH FS_HEALTH
#define FPA FS_ARMOR
  
#define SGW W_SHOTGUN
#define MGW W_MACHINEGUN
#define GLW W_GRENADELAUNCHER
#define RLW W_ROCKETLAUNCHER
#define RGW W_RAILGUN
#define BFW W_BFG10K
#define LGW W_LIGHTNING
#define PGW W_PLASMAGUN
#define PXW W_GRENADELAUNCHER
#define NGW W_SHOTGUN
#define CGW W_PLASMAGUN
  
#define GSGW GWW_SHOTGUN
#define GMGW GWW_MACHINEGUN
#define GGLW GWW_GRENADELAUNCHER
#define GRLW GWW_ROCKETLAUNCHER
#define GRGW GWW_RAILGUN
#define GBFW GWW_BFG10K
#define GLGW GWW_LIGHTNING
#define GPGW GWW_PLASMAGUN
#define GPXW GWW_GRENADELAUNCHER
#define GNGW GWW_SHOTGUN
#define GCGW GWW_PLASMAGUN
  
#define TELW W_TELEPORTER
#define MEDW W_MEDKIT
#define QW W_QUAD
#define ENVW W_ENVIRO
#define HAW W_HASTE
#define INW W_INVISIBILITY
#define REGW W_REGEN
#define FLW W_FLIGHT
#define KAMW W_FLIGHT
#define IBW W_REGEN
#define PORW W_TELEPORTER
#define SCW W_ENVIRO
#define GUW W_REGEN
#define DUBW W_QUAD
#define AMRW W_REGEN
#define REDCW FLAG_WEIGHT
#define BLCW FLAG_WEIGHT
  
#define FGW FLAG_WEIGHT
 
#else

//MEC
#define FS_HEALTH FPH
#define FS_ARMOR FPA
  
#define W_SHOTGUN SGW
#define W_MACHINEGUN MGW
#define W_GRENADELAUNCHER GLW
#define W_ROCKETLAUNCHER RLW
#define W_RAILGUN RGW
#define W_BFG10K BFW
#define W_LIGHTNING LGW
#define W_PLASMAGUN PGW
#define W_GRENADELAUNCHER PXW
#define W_SHOTGUN NGW
#define W_PLASMAGUN CGW
  
#define GWW_SHOTGUN GSGW
#define GWW_MACHINEGUN GMGW
#define GWW_GRENADELAUNCHER GGLW
#define GWW_ROCKETLAUNCHER GRLW
#define GWW_RAILGUN GRGW
#define GWW_BFG10K GBFW
#define GWW_LIGHTNING GLGW
#define GWW_PLASMAGUN GPGW
#define GWW_GRENADELAUNCHER GPXW
#define GWW_SHOTGUN GNGW
#define GWW_PLASMAGUN GCGW
  
#define W_TELEPORTER TELW
#define W_MEDKIT MEDW
#define W_QUAD QW
#define W_ENVIRO ENVW
#define W_HASTE HAW
#define W_INVISIBILITY INW
#define W_REGEN REGW
#define W_FLIGHT FLW
#define W_FLIGHT KAMW
#define W_REGEN IBW
#define W_TELEPORTER PORW
#define W_ENVIRO SCW
#define W_REGEN GUW
#define W_QUAD DUBW
#define W_REGEN AMRW
#define FLAG_WEIGHT REDCW
#define FLAG_WEIGHT BLCW
  
#define FLAG_WEIGHT FGW
#endif

#include "fuzi.c"
