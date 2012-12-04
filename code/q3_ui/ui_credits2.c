/*
===========================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License,
or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Spearmint Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms.
You should have received a copy of these additional terms immediately following
the terms and conditions of the GNU General Public License.  If not, please
request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional
terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc.,
Suite 120, Rockville, Maryland 20850 USA.
===========================================================================
*/

// INCLUDE FILES
#include "ui_local.h"

// CONSTANT DEFINITIONS

#define SCROLLSPEED	4.75 // The scrolling speed in pixels per second.
                             // modify as appropriate for our credits
// #define BACKGROUND_SHADER 
// uncomment this to use a background shader, otherwise a solid color
// defined in the vec4_t "color_background" is filled to the screen
                            
// STRUCTURES

typedef struct {
	menuframework_s	menu;
	int frame;
} creditsmenu_t;

static creditsmenu_t	s_credits;

int starttime; // game time at which credits are started
float mvolume; // records the original music volume level, as we will
               // modify it for the credits

// change this to change the background colour on credits
vec4_t color_background		        = {0.00, 0.00, 0.00, 0.00};
// these are just example colours that are used in credits[] 
vec4_t color_headertext			= {0.53, 0.77, 1.00, 1.00};
vec4_t color_maintext			= {1.00, 1.00, 1.00, 1.00};

qhandle_t	BackgroundShader; // definition of the background shader pointer

typedef struct
{
	char *string;
	int style;
	vec4_t *colour;
} cr_line;

cr_line credits[] = { // edit this as necessary for your credits
	
{ "Scrolling Credits Sequence", UI_CENTER|UI_BIGFONT|UI_PULSE, &color_headertext },
{ "Design by Iain McGinniss", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Special Thanks To:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "ID Software", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "Code 3 Arena", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "The Gamespy Network", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "iD Software is:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Programming:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "John Carmack, Robert A. Duffy,", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "Jim Dose", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Art:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Adrian Carmack, Kevin Cloud,", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "Kenneth Scott, Seneca Menard,", UI_CENTER|UI_SMALLFONT, &color_maintext }, 
{ "Fred Nilsson", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Game Designer:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Graeme Devine", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Level Design:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Tim Willits, Christian Antkow,", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "Paul Jaquays", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "CEO:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Todd Hollenshead", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Director of Business Development:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Marty Stratton", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Biz Assist and id mom:", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Donna Jackson", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Development Assistance", UI_CENTER|UI_SMALLFONT, &color_headertext },
{ "Eric Webb", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "To order: 1-800-idgames", UI_CENTER|UI_SMALLFONT, &color_maintext }, 
{ "www.quake3arena.com  www.idsoftware.com", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ "", UI_CENTER|UI_SMALLFONT, &color_blue },
{ "Quake III Arena(c) 1999-2000, Id Software, Inc.", UI_CENTER|UI_SMALLFONT, &color_maintext },
{ " All Rights Reserved", UI_CENTER|UI_SMALLFONT, &color_maintext },
  {NULL}
};

/*
===============
UI_CreditMenu_Draw_ioq3
===============
*/
static void UI_CreditMenu_Draw_ioq3( void ) {
        int             y;
        int             i;

        // These are all people that have made commits to Subversion, and thus
        //  probably incomplete.
        // (These are in alphabetical order, for the defense of everyone's egos.)
        static const char *names[] = {
                "Tim Angus",
                "James Canete",
                "Vincent Cojot",
                "Ryan C. Gordon",
                "Aaron Gyes",
                "Zack Middleton",
                "Ludwig Nussel",
                "Julian Priestley",
                "Scirocco Six",
                "Thilo Schulz",
                "Zachary J. Slater",
                "Tony J. White",
                "...and many, many others!",  // keep this one last.
                NULL
        };

        // Center text vertically on the screen
        y = (SCREEN_HEIGHT - ARRAY_LEN(names) * (1.42 * PROP_HEIGHT * PROP_SMALL_SIZE_SCALE)) / 2;

        UI_DrawProportionalString( 320, y, "ioquake3 contributors:", UI_CENTER|UI_SMALLFONT, color_white );
        y += 1.42 * PROP_HEIGHT * PROP_SMALL_SIZE_SCALE;

        for (i = 0; names[i]; i++) {
                UI_DrawProportionalString( 320, y, names[i], UI_CENTER|UI_SMALLFONT, color_white );
                y += 1.42 * PROP_HEIGHT * PROP_SMALL_SIZE_SCALE;
        }

        UI_DrawString( 320, 459, "http://www.ioquake3.org/", UI_CENTER|UI_SMALLFONT, color_red );
}


/*
=================
UI_CreditMenu_Key
=================
*/
static sfxHandle_t UI_CreditMenu_Key( int key ) {
	if( key & K_CHAR_FLAG ) {
                return 0;
        }

        s_credits.frame++;
        if (s_credits.frame == 1) {
                s_credits.menu.draw = UI_CreditMenu_Draw_ioq3;
        } else {
		trap_Cmd_ExecuteText( EXEC_APPEND, va("s_musicvolume %f; quit\n", mvolume));
	}
	
	return 0;
}

/*
=================
ScrollingCredits_Draw
This is the main drawing function for the credits. 
Most of the code is self-explanatory.
=================
*/
static void ScrollingCredits_Draw(void)
{
  int x = 320, y, n, ysize = 0, fadetime = 0;
  vec4_t fadecolour = { 0.00, 0.00, 0.00, 0.00 };

  // ysize is used to determine the entire length 
  // of the credits in pixels. 
  // We can then use this in further calculations
  if(!ysize) // ysize not calculated, so calculate it dammit!
  {
    // loop through entire credits array
    for(n = 0; n <= sizeof(credits) - 1; n++) 
    {
      // it is a small character
      if(credits[n].style & UI_SMALLFONT) 
      {
        // add small character height
        ysize += PROP_HEIGHT * PROP_SMALL_SIZE_SCALE;
        
      // it is a big character
      }else if(credits[n].style & UI_BIGFONT) 
      {
        // add big character size
        ysize += PROP_HEIGHT;
        
      // it is a huge character
      }else if(credits[n].style & UI_GIANTFONT) 
      {
        // add giant character size.
        ysize += PROP_HEIGHT * (1 / PROP_SMALL_SIZE_SCALE); 
      }
    }
  }

  // first, fill the background with the specified colour/shader
  // we are drawing a shader
#ifdef BACKGROUND_SHADER 
    UI_DrawHandlePic(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BackgroundShader);
  
  // we are just filling a color
#else 
    UI_FillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, color_background);
#endif

  // let's draw the stuff
  // set initial y location
  y = 480 - SCROLLSPEED * (float)(uis.realtime - starttime) / 100;
  
  // loop through the entire credits sequence
  for(n = 0; n <= sizeof(credits) - 1; n++)
  {
    // this NULL string marks the end of the credits struct
    if(credits[n].string == NULL) 
    {
      if(y < -16) // credits sequence is completely off screen
      {
        trap_Cmd_ExecuteText( EXEC_APPEND, 
                         va("s_musicvolume %f; quit\n", mvolume));
        break; // end of credits
      }
      break;
    }
		
    if( strlen(credits[n].string) == 1) // spacer string, no need to draw
      continue;

    if( y > -(PROP_HEIGHT * (1 / PROP_SMALL_SIZE_SCALE))) 
      // the line is within the visible range of the screen
      UI_DrawProportionalString(x, y, credits[n].string, 
                                credits[n].style, *credits[n].colour );
		
    // re-adjust y for next line
    if(credits[n].style & UI_SMALLFONT)
    {
      y += PROP_HEIGHT * PROP_SMALL_SIZE_SCALE;
    }else if(credits[n].style & UI_BIGFONT)
    {
      y += PROP_HEIGHT;
    }else if(credits[n].style & UI_GIANTFONT)
    {
      y += PROP_HEIGHT * (1 / PROP_SMALL_SIZE_SCALE);
    }

    // if y is off the screen, break out of loop
    if (y > 480)
    break;
  }
}

/*
===============
UI_CreditMenu
===============
*/
void UI_CreditMenu( void ) {
	memset( &s_credits, 0 ,sizeof(s_credits) );

	s_credits.menu.draw = ScrollingCredits_Draw;
	s_credits.menu.key = UI_CreditMenu_Key;
	s_credits.menu.fullscreen = qtrue;
	UI_PushMenu ( &s_credits.menu );

	starttime = uis.realtime; // record start time for credits to scroll properly
	mvolume = trap_Cvar_VariableValue( "s_musicvolume" );
	if(mvolume < 0.5)
		trap_Cmd_ExecuteText( EXEC_APPEND, "s_musicvolume 0.5\n" );
	trap_Cmd_ExecuteText( EXEC_APPEND, "music music/fla22k_02\n" );

	// load the background shader
#ifdef BACKGROUND_SHADER
	BackgroundShader = 
	  trap_R_RegisterShaderNoMip("*YOURSHADER_HERE*");
#endif
}
