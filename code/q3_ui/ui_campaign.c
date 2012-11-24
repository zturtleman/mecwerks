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
/*
 =============================================================================
 
 NEW CAMPAIGN MENU
 
 =============================================================================
 */

#include "ui_local.h"


#define ART_BACK0			"menu/art/back_0"
#define ART_BACK1			"menu/art/back_1"

#define ID_BACK				10
#define ID_FFA              11

typedef struct {
	menuframework_s	menu;
    
	menutext_s		banner;
    menutext_s      ffagame;
	menubitmap_s	back;
    
} campaign_t;

static campaign_t	s_campaign;


/*
 ===============
 Campaign_MenuEvent
 ===============
 */
static void Campaign_MenuEvent( void *ptr, int event ) {
	if( event != QM_ACTIVATED ) {
		return;
	}
    
	switch ( ((menucommon_s*)ptr)->id ) {
        case ID_FFA:
            //UI_FFAMenu();
            UI_PopMenu();
            break;
            
        case ID_BACK:
            UI_PopMenu();
            break;
	}
}


/*
 ===============
 Campaign_MenuInit
 ===============
 */
static void Campaign_MenuInit( void ) {
	UI_Campaign_Cache();
    
	memset( &s_campaign, 0 ,sizeof(campaign_t) );
	s_campaign.menu.wrapAround = qtrue;
	s_campaign.menu.fullscreen = qtrue;
    
	s_campaign.banner.generic.type	= MTYPE_BTEXT;
	s_campaign.banner.generic.x		= 320;
	s_campaign.banner.generic.y		= 16;
	s_campaign.banner.string			= "CAMPAIGN";
	s_campaign.banner.color			= color_white;
	s_campaign.banner.style			= UI_CENTER;
    
	s_campaign.ffagame.generic.type     = MTYPE_PTEXT;
    s_campaign.ffagame.generic.flags    = QMF_PULSEIFFOCUS;
	s_campaign.ffagame.generic.x		= 200;
	s_campaign.ffagame.generic.y		= 200;
    s_campaign.ffagame.generic.id       = ID_FFA;
    s_campaign.ffagame.generic.callback = Campaign_MenuEvent;
	s_campaign.ffagame.string			= "Free For All";
	s_campaign.ffagame.color			= color_blue;
	s_campaign.ffagame.style			= UI_DROPSHADOW;
        
    s_campaign.back.generic.type		= MTYPE_BITMAP;
	s_campaign.back.generic.name		= ART_BACK0;
	s_campaign.back.generic.flags	= QMF_LEFT_JUSTIFY|QMF_PULSEIFFOCUS;
	s_campaign.back.generic.id		= ID_BACK;
	s_campaign.back.generic.callback	= Campaign_MenuEvent;
	s_campaign.back.generic.x		= 0;
	s_campaign.back.generic.y		= 480-64;
	s_campaign.back.width			= 128;
	s_campaign.back.height			= 64;
	s_campaign.back.focuspic			= ART_BACK1;
    
	Menu_AddItem( &s_campaign.menu, &s_campaign.banner );
	Menu_AddItem( &s_campaign.menu, &s_campaign.ffagame );
	Menu_AddItem( &s_campaign.menu, &s_campaign.back );
}

/*
 =================
 UI_Campaign_Cache
 =================
 */
void UI_Campaign_Cache( void ) {
	trap_R_RegisterShaderNoMip( ART_BACK0 );
	trap_R_RegisterShaderNoMip( ART_BACK1 );
}


/*
 ===============
 UI_CampaignMenu
 ===============
 */
void UI_CampaignMenu( void ) {
	Campaign_MenuInit();
	UI_PushMenu( &s_campaign.menu );
}

