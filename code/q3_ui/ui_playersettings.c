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
//
#include "ui_local.h"

#define ART_FRAMEL			"menu/art/frame2_l"
#define ART_FRAMER			"menu/art/frame1_r"
#define ART_MODEL0			"menu/art/model_0"
#define ART_MODEL1			"menu/art/model_1"
#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"
#define ART_FX_BASE			"menu/art/fx_base"
#define ART_FX_BLUE			"menu/art/fx_blue"
#define ART_FX_CYAN			"menu/art/fx_cyan"
#define ART_FX_GREEN		"menu/art/fx_grn"
#define ART_FX_RED			"menu/art/fx_red"
#define ART_FX_TEAL			"menu/art/fx_teal"
#define ART_FX_WHITE		"menu/art/fx_white"
#define ART_FX_YELLOW		"menu/art/fx_yel"

#define NUM_COLOR_EFFECTS 7

#define ID_NAME			10
#define ID_CROSSHAIR		11
#define ID_EFFECTS		12
#define ID_BACK			13
#define ID_MODEL		14

#define MAX_NAMELENGTH	20
#define NUM_CROSSHAIRS                  10

typedef struct {
	menuframework_s		menu;

	menutext_s			banner;
	menubitmap_s		framel;
	menubitmap_s		framer;
	menubitmap_s		player;

	menufield_s			name;
	menulist_s			effects;

	menubitmap_s		back;
	menubitmap_s		model;
	menubitmap_s		item_null;

	qhandle_t			fxBasePic;
	qhandle_t			fxPic[NUM_COLOR_EFFECTS];
	playerInfo_t		playerinfo;
	int					current_fx;
	char				playerModel[MAX_QPATH];
	int					localClient;
	char				bannerString[32];

	menulist_s		crosshair;
	qhandle_t               crosshairShader[NUM_CROSSHAIRS];
} playersettings_t;

static playersettings_t	s_playersettings;

static int gamecodetoui[NUM_COLOR_EFFECTS] = {4,2,3,0,5,1,6};
static int uitogamecode[NUM_COLOR_EFFECTS] = {4,6,2,3,1,5,7};

/*
=================
PlayerSettings_DrawName
=================
*/
static void PlayerSettings_DrawName( void *self ) {
	menufield_s		*f;
	qboolean		focus;
	int				style;
	char			*txt;
	char			c;
	float			*color;
	int				n;
	int				basex, x, y;
	char			name[32];

	f = (menufield_s*)self;
	basex = f->generic.x;
	y = f->generic.y;
	focus = (f->generic.parent->cursor == f->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( basex, y, "Name", style, color );

	// draw the actual name
	basex += 64;
	y += PROP_HEIGHT;
	txt = f->field.buffer;
	color = g_color_table[ColorIndex(COLOR_WHITE)];
	x = basex;
	while ( (c = *txt) != 0 ) {
		if ( !focus && Q_IsColorString( txt ) ) {
			n = ColorIndex( *(txt+1) );
			if( n == 0 ) {
				n = 7;
			}
			color = g_color_table[n];
			txt += 2;
			continue;
		}
		UI_DrawChar( x, y, c, style, color );
		txt++;
		x += SMALLCHAR_WIDTH;
	}

	// draw cursor if we have focus
	if( focus ) {
		if ( trap_Key_GetOverstrikeMode() ) {
			c = 11;
		} else {
			c = 10;
		}

		style &= ~UI_PULSE;
		style |= UI_BLINK;

		UI_DrawChar( basex + f->field.cursor * SMALLCHAR_WIDTH, y, c, style, color_white );
	}

	// draw at bottom also using proportional font
	Q_strncpyz( name, f->field.buffer, sizeof(name) );
	Q_CleanStr( name );
	UI_DrawProportionalString( 320, 440, name, UI_CENTER|UI_BIGFONT, text_color_normal );
}

/*
=================
Crosshair_Draw
=================
*/
static void Crosshair_Draw( void *self ) {
        menulist_s      *s;
        qboolean                focus;
        int                             style;
        float                   *color;
        int                             basex, x, y;

        s = (menulist_s*)self;
        basex = s->generic.x;
        y = s->generic.y;
        focus = (s->generic.parent->cursor == s->generic.menuPosition);

        style = UI_LEFT|UI_SMALLFONT;
        color = text_color_normal;
        if( focus ) {
                style |= UI_PULSE;
                color = text_color_highlight;
        }

        UI_DrawProportionalString( basex, y, "CROSSHAIR", style, color );
        
	x = s->generic.x;

        style = UI_SMALLFONT;
        focus = (s->generic.parent->cursor == s->generic.menuPosition);

        if ( s->generic.flags & QMF_GRAYED )
                color = text_color_disabled;
        else if ( focus )
        {
                color = text_color_highlight;
                style |= UI_PULSE;
        }
        else if ( s->generic.flags & QMF_BLINK )
        {
                color = text_color_highlight;
                style |= UI_BLINK;
        }
        else
                color = text_color_normal;

        if( !s->curvalue ) {
                return;
        }
        UI_DrawHandlePic( x + 64, y + PROP_HEIGHT + 8, 24, 24, s_playersettings.crosshairShader[s->curvalue] );
}

/*
=================
PlayerSettings_DrawEffects
=================
*/
static void PlayerSettings_DrawEffects( void *self ) {
	menulist_s		*item;
	qboolean		focus;
	int				style;
	float			*color;
	float			xOffset;

	item = (menulist_s *)self;
	focus = (item->generic.parent->cursor == item->generic.menuPosition);

	style = UI_LEFT|UI_SMALLFONT;
	color = text_color_normal;
	if( focus ) {
		style |= UI_PULSE;
		color = text_color_highlight;
	}

	UI_DrawProportionalString( item->generic.x, item->generic.y, "Effects", style, color );

	xOffset = 128.0f / (NUM_COLOR_EFFECTS + 1);

	UI_DrawHandlePic( item->generic.x + 64, item->generic.y + PROP_HEIGHT + 8, 128, 8, s_playersettings.fxBasePic );
	UI_DrawHandlePic( item->generic.x + 64 + item->curvalue * xOffset + xOffset * 0.5f, item->generic.y + PROP_HEIGHT + 6, 16, 12, s_playersettings.fxPic[item->curvalue] );
}


/*
=================
PlayerSettings_DrawPlayer
=================
*/
static void PlayerSettings_DrawPlayer( void *self ) {
	menubitmap_s	*b;
	vec3_t			viewangles;
	char			buf[MAX_QPATH];

	trap_Cvar_VariableStringBuffer( Com_LocalClientCvarName(s_playersettings.localClient, "model"), buf, sizeof( buf ) );
	if ( strcmp( buf, s_playersettings.playerModel ) != 0 ) {
		UI_PlayerInfo_SetModel( &s_playersettings.playerinfo, buf );
		strcpy( s_playersettings.playerModel, buf );

		viewangles[YAW]   = 180 - 30;
		viewangles[PITCH] = 0;
		viewangles[ROLL]  = 0;
		UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );
	}

	b = (menubitmap_s*) self;
	UI_DrawPlayer( b->generic.x, b->generic.y, b->width, b->height, &s_playersettings.playerinfo, uis.realtime/2 );
}


/*
=================
PlayerSettings_SaveChanges
=================
*/
static void PlayerSettings_SaveChanges( void ) {
	// name
	trap_Cvar_Set( Com_LocalClientCvarName(s_playersettings.localClient, "name"), s_playersettings.name.field.buffer );

	// crosshair
	trap_Cvar_SetValue( Com_LocalClientCvarName(s_playersettings.localClient, "cg_drawCrosshair"), s_playersettings.crosshair.curvalue );

	// effects color
	trap_Cvar_SetValue( Com_LocalClientCvarName(s_playersettings.localClient, "color1"),
			uitogamecode[s_playersettings.effects.curvalue] );
}


/*
=================
PlayerSettings_MenuKey
=================
*/
static sfxHandle_t PlayerSettings_MenuKey( int key ) {
	if( key == K_MOUSE2 || key == K_ESCAPE ) {
		PlayerSettings_SaveChanges();
	}
	return Menu_DefaultKey( &s_playersettings.menu, key );
}


/*
=================
PlayerSettings_SetMenuItems
=================
*/
static void PlayerSettings_SetMenuItems( void ) {
	vec3_t	viewangles;
	int		c;

	// name
	Q_strncpyz( s_playersettings.name.field.buffer, UI_Cvar_VariableString(
			Com_LocalClientCvarName(s_playersettings.localClient, "name")), sizeof(s_playersettings.name.field.buffer) );

	// effects color
	c = trap_Cvar_VariableValue( Com_LocalClientCvarName(s_playersettings.localClient, "color1") ) - 1;
	if( c < 0 || c > NUM_COLOR_EFFECTS-1 ) {
		c = NUM_COLOR_EFFECTS-1;
	}
	s_playersettings.effects.curvalue = gamecodetoui[c];

	// model/skin
	memset( &s_playersettings.playerinfo, 0, sizeof(playerInfo_t) );
	
	viewangles[YAW]   = 180 - 30;
	viewangles[PITCH] = 0;
	viewangles[ROLL]  = 0;

	UI_PlayerInfo_SetModel( &s_playersettings.playerinfo,
			UI_Cvar_VariableString( Com_LocalClientCvarName(s_playersettings.localClient, "model") ) );
	UI_PlayerInfo_SetInfo( &s_playersettings.playerinfo, LEGS_IDLE, TORSO_STAND, viewangles, vec3_origin, WP_MACHINEGUN, qfalse );

	// crosshair
	s_playersettings.crosshair.curvalue = (int)trap_Cvar_VariableValue( Com_LocalClientCvarName(s_playersettings.localClient, "cg_drawCrosshair" )) % NUM_CROSSHAIRS;
}


/*
=================
PlayerSettings_MenuEvent
=================
*/
static void PlayerSettings_MenuEvent( void* ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}

	switch( ((menucommon_s*)ptr)->id ) {
	case ID_CROSSHAIR:
		trap_Cvar_Set( Com_LocalClientCvarName(s_playersettings.localClient, "cg_drawCrosshair"), s_playersettings.crosshair.generic.name  );
		break;

	case ID_MODEL:
		PlayerSettings_SaveChanges();
		UI_PlayerModelMenu(s_playersettings.localClient);
		break;

	case ID_BACK:
		PlayerSettings_SaveChanges();
		UI_PopMenu();
		break;
	}
}


/*
=================
PlayerSettings_MenuInit
=================
*/
static void PlayerSettings_MenuInit( int localClient )
{
	int		y;

	memset(&s_playersettings,0,sizeof(playersettings_t));

	s_playersettings.localClient = localClient;
	if (UI_MaxSplitView() == 1) {
		Com_sprintf(s_playersettings.bannerString, sizeof (s_playersettings.bannerString), "PLAYER SETTINGS");
	} else {
		Com_sprintf(s_playersettings.bannerString, sizeof (s_playersettings.bannerString), "PLAYER %d SETTINGS", s_playersettings.localClient+1);
	}

	PlayerSettings_Cache();

	s_playersettings.menu.key        = PlayerSettings_MenuKey;
	s_playersettings.menu.wrapAround = qtrue;
	s_playersettings.menu.fullscreen = qtrue;

	if (UI_MaxSplitView() == 1) {
		s_playersettings.banner.generic.type  = MTYPE_BTEXT;
	} else {
		// ZTM: Q3's BTEXT doesn't have numbers sadly.
		s_playersettings.banner.generic.type  = MTYPE_PTEXT;
	}
	s_playersettings.banner.generic.x     = 320;
	s_playersettings.banner.generic.y     = 16;
	s_playersettings.banner.string = s_playersettings.bannerString;
	s_playersettings.banner.color         = color_red;
	s_playersettings.banner.style         = UI_CENTER;

	s_playersettings.framel.generic.type  = MTYPE_BITMAP;
	s_playersettings.framel.generic.name  = ART_FRAMEL;
	s_playersettings.framel.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framel.generic.x     = 0;
	s_playersettings.framel.generic.y     = 78;
	s_playersettings.framel.width         = 256;
	s_playersettings.framel.height        = 329;

	s_playersettings.framer.generic.type  = MTYPE_BITMAP;
	s_playersettings.framer.generic.name  = ART_FRAMER;
	s_playersettings.framer.generic.flags = QMF_LEFT_JUSTIFY|QMF_INACTIVE;
	s_playersettings.framer.generic.x     = 376;
	s_playersettings.framer.generic.y     = 76;
	s_playersettings.framer.width         = 256;
	s_playersettings.framer.height        = 334;

	y = 144;
	s_playersettings.name.generic.type			= MTYPE_FIELD;
	s_playersettings.name.generic.flags			= QMF_NODEFAULTINIT;
	s_playersettings.name.generic.ownerdraw		= PlayerSettings_DrawName;
	s_playersettings.name.field.widthInChars	= MAX_NAMELENGTH;
	s_playersettings.name.field.maxchars		= MAX_NAMELENGTH;
	s_playersettings.name.generic.x				= 192;
	s_playersettings.name.generic.y				= y;
	s_playersettings.name.generic.left			= 192 - 8;
	s_playersettings.name.generic.top			= y - 8;
	s_playersettings.name.generic.right			= 192 + 200;
	s_playersettings.name.generic.bottom		= y + 2 * PROP_HEIGHT;

	y += 3 * PROP_HEIGHT;
	s_playersettings.crosshair.generic.type        = MTYPE_SPINCONTROL;
        s_playersettings.crosshair.generic.flags       = QMF_PULSEIFFOCUS|QMF_SMALLFONT|QMF_NODEFAULTINIT|QMF_OWNERDRAW;
        s_playersettings.crosshair.generic.x           = 192;
        s_playersettings.crosshair.generic.y           = y;
        s_playersettings.crosshair.generic.name        = "Crosshair:";
        s_playersettings.crosshair.generic.callback    = PlayerSettings_MenuEvent;
        s_playersettings.crosshair.generic.ownerdraw   = Crosshair_Draw;
        s_playersettings.crosshair.generic.id          = ID_CROSSHAIR;
        s_playersettings.crosshair.generic.top         = y - 8;
        s_playersettings.crosshair.generic.bottom      = y + 2 * PROP_HEIGHT;
        s_playersettings.crosshair.generic.left        = 192 - 8;
        s_playersettings.crosshair.generic.right       = 192 + 200;
        s_playersettings.crosshair.numitems            = NUM_CROSSHAIRS;

	y += 3 * PROP_HEIGHT;
	s_playersettings.effects.generic.type		= MTYPE_SPINCONTROL;
	s_playersettings.effects.generic.flags		= QMF_NODEFAULTINIT;
	s_playersettings.effects.generic.id			= ID_EFFECTS;
	s_playersettings.effects.generic.ownerdraw	= PlayerSettings_DrawEffects;
	s_playersettings.effects.generic.x			= 192;
	s_playersettings.effects.generic.y			= y;
	s_playersettings.effects.generic.left		= 192 - 8;
	s_playersettings.effects.generic.top		= y - 8;
	s_playersettings.effects.generic.right		= 192 + 200;
	s_playersettings.effects.generic.bottom		= y + 2* PROP_HEIGHT;
	s_playersettings.effects.numitems			= NUM_COLOR_EFFECTS;

	s_playersettings.model.generic.type			= MTYPE_BITMAP;
	s_playersettings.model.generic.name			= ART_MODEL0;
	s_playersettings.model.generic.flags		= QMF_RIGHT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.model.generic.id			= ID_MODEL;
	s_playersettings.model.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.model.generic.x			= 640;
	s_playersettings.model.generic.y			= 480-64;
	s_playersettings.model.width				= 128;
	s_playersettings.model.height				= 64;
	s_playersettings.model.focuspic				= ART_MODEL1;

	s_playersettings.player.generic.type		= MTYPE_BITMAP;
	s_playersettings.player.generic.flags		= QMF_INACTIVE;
	s_playersettings.player.generic.ownerdraw	= PlayerSettings_DrawPlayer;
	s_playersettings.player.generic.x			= 400;
	s_playersettings.player.generic.y			= -40;
	s_playersettings.player.width				= 32*10;
	s_playersettings.player.height				= 56*10;

	s_playersettings.back.generic.type			= MTYPE_BITMAP;
	s_playersettings.back.generic.name			= ART_BACK0;
	s_playersettings.back.generic.flags			= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_playersettings.back.generic.id			= ID_BACK;
	s_playersettings.back.generic.callback		= PlayerSettings_MenuEvent;
	s_playersettings.back.generic.x				= 0;
	s_playersettings.back.generic.y				= 480-64;
	s_playersettings.back.width					= 128;
	s_playersettings.back.height				= 64;
	s_playersettings.back.focuspic				= ART_BACK1;

	s_playersettings.item_null.generic.type		= MTYPE_BITMAP;
	s_playersettings.item_null.generic.flags	= QMF_LEFT_JUSTIFY|QMF_MOUSEONLY|QMF_SILENT;
	s_playersettings.item_null.generic.x		= 0;
	s_playersettings.item_null.generic.y		= 0;
	s_playersettings.item_null.width			= 640;
	s_playersettings.item_null.height			= 480;

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.banner );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framel );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.framer );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.name );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.crosshair );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.effects );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.model );
	Menu_AddItem( &s_playersettings.menu, &s_playersettings.back );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.player );

	Menu_AddItem( &s_playersettings.menu, &s_playersettings.item_null );

	PlayerSettings_SetMenuItems();
}


/*
=================
PlayerSettings_Cache
=================
*/
void PlayerSettings_Cache( void ) {
	int n;
	
	trap_R_RegisterShaderNoMip( ART_FRAMEL );
	trap_R_RegisterShaderNoMip( ART_FRAMER );
	trap_R_RegisterShaderNoMip( ART_MODEL0 );
	trap_R_RegisterShaderNoMip( ART_MODEL1 );
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );

	for( n = 0; n < NUM_CROSSHAIRS; n++ ) {
                s_playersettings.crosshairShader[n] = trap_R_RegisterShaderNoMip( va("gfx/2d/crosshair%c", 'a' + n ) );
        }

	s_playersettings.fxBasePic = trap_R_RegisterShaderNoMip( ART_FX_BASE );
	s_playersettings.fxPic[0] = trap_R_RegisterShaderNoMip( ART_FX_RED );
	s_playersettings.fxPic[1] = trap_R_RegisterShaderNoMip( ART_FX_YELLOW );
	s_playersettings.fxPic[2] = trap_R_RegisterShaderNoMip( ART_FX_GREEN );
	s_playersettings.fxPic[3] = trap_R_RegisterShaderNoMip( ART_FX_TEAL );
	s_playersettings.fxPic[4] = trap_R_RegisterShaderNoMip( ART_FX_BLUE );
	s_playersettings.fxPic[5] = trap_R_RegisterShaderNoMip( ART_FX_CYAN );
	s_playersettings.fxPic[6] = trap_R_RegisterShaderNoMip( ART_FX_WHITE );
}


/*
=================
UI_PlayerSettingsMenu
=================
*/
void UI_PlayerSettingsMenu( int localClient ) {
	PlayerSettings_MenuInit(localClient);
	UI_PushMenu( &s_playersettings.menu );
}
