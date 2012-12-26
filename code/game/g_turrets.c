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

#include "g_local.h"

/*
=======================
Turret code. for spawning and seting the spawn location of turrets
=======================
*/
#define RANGE 500

#define HARC 90
#define DARC 10
#define UARC 45

#define TURRET_BASE "models/weapons2/railgun/railgun.md3"
#define TURRET_GUN  "models/weapons2/machinegun/machinegun.md3"

qboolean checktarget(gentity_t *firer, gentity_t *target){
	vec3_t 		distance,forward;
	trace_t         trace;
	int		angle;

	/*
	returns qfalse if the target is not valid. returns qtrue if it is
	*/

	if (!target) // Do we have a target?
		return qfalse;
	if (!target->inuse) // Does the target still exist?
		return qfalse;
	if (target == firer) // is the target us?
		return qfalse;
	if(!target->client) // is the target a bot or player?
		return qfalse;
	if (target == firer->parent) // is the target the person that created the turret?
		return qfalse;
	if (OnSameTeam(firer->parent, target)) // is the target one of us?
		return qfalse;

	if (target->health < 0) // is the target still alive?
		return qfalse;

	VectorSubtract(target->r.currentOrigin, firer->r.currentOrigin,distance);
	if (VectorLength(distance) > RANGE) // is the target within range?
		return qfalse;

	trap_Trace (&trace, firer->s.pos.trBase, NULL, NULL, target->s.pos.trBase, firer->s.number, MASK_SHOT );
	if ( trace.contents & CONTENTS_SOLID ) // can we see the target?
		return qfalse;
	/*
	The last two checks are done last as they require more processing power than the others.
	this order is just better from a proccesing load perspective
	*/

	vectoangles (distance, distance);
	VectorSubtract(firer->centerpoint, distance, distance);
	angle = abs((int)distance[1]);
	while (angle >= 360) {
		angle -= 360;
	}
	if ((angle >= HARC) && (angle <= (360 - HARC)))
		return qfalse;
	angle = abs((int)distance[0]);
	while (angle >= 360) {
		angle -= 360;
	}
	if ((angle > UARC) && (angle < (360 - DARC)))
		return qfalse;

	return qtrue;
}




void turret_findenemy( gentity_t *ent) {
	gentity_t *target;

	target = g_entities;

	for (; target < &g_entities[level.num_entities]; target++)
	{
		if(!checktarget(ent, target))
			continue;
		ent->enemy = target;
		return;
	}

	ent->enemy = NULL;
}



void turret_trackenemy( gentity_t *ent){
	vec3_t dir;


	VectorSubtract(ent->enemy->r.currentOrigin, ent->r.currentOrigin, dir);
	VectorNormalize(dir);
	VectorCopy(dir, ent->turloc);
	vectoangles(dir, dir);
	VectorCopy(dir, ent->s.angles);
	VectorCopy(dir, ent->r.currentAngles);
	VectorCopy( dir, ent->s.apos.trBase );
	trap_LinkEntity (ent);

}

void turret_fireonenemy( gentity_t *ent){
	fire_rocket( ent->activator, ent->r.currentOrigin, ent->turloc );
	G_AddEvent( ent, EV_FIRE_WEAPON, 0 );
	ent->count = level.time + 200;
	// decloaks a cloaked turret when firing.
	if (ent->s.time2 == 2){
		ent->s.time2 = 3;
		ent->chain->s.time2 = 3;
	}
}




void turret_think( gentity_t *ent){

	ent->nextthink = level.time + 10;


	if (!checktarget(ent, ent->enemy))
		turret_findenemy(ent);
	if(!ent->enemy)
		return;
	turret_trackenemy(ent);
	if (ent->count < level.time)
		turret_fireonenemy(ent);
}

void Base_think(gentity_t *ent){

	// for shielded turrets. regenerates health to 400 at 10 health a second
	if ((ent->s.time2 == 1) & (ent->health < 4000)){
		ent->health += 1;
		ent->nextthink = level.time + 100;
		//recloaks a turret without a valid target. checks once a second
		if ((ent->s.time2==3)&(!ent->chain->enemy))
		{
			ent->s.time2 = 2;
			ent->chain->s.time2 = 2;
			ent->nextthink = level.time + 1000;
		}

	}
}

void createturretgun(gentity_t *ent){
	gentity_t *turret; 	// The object to hold the turrets details.
	int			num;
	int			touch[MAX_GENTITIES];

	// code to check there is noone within the base before making it solid
	vec3_t		mins, maxs;

	VectorAdd( ent->r.currentOrigin, ent->r.absmin, mins );
	VectorAdd( ent->r.currentOrigin, ent->r.absmax, maxs );
	num = trap_EntitiesInBox( mins, maxs, touch, MAX_GENTITIES );
	if (num > 1) {
		ent->nextthink = level.time + 1000;
		return;
	}

	ent->nextthink = level.time + 100; // sets up the thinking for the cloaking or regeneration/
	ent->think = Base_think; // handles cloaking or regeneration
	ent->clipmask = CONTENTS_SOLID | CONTENTS_PLAYERCLIP;
	ent->s.contents = CONTENTS_SOLID;
	turret = G_Spawn();
	turret->parent = ent->parent;
	turret->chain = ent;
	ent->chain = turret;
	turret->s.eType = ET_TURRET;
	turret->s.time2 = 0;
	turret->eventTime = 200;
	turret->s.number = turret - g_entities;
	turret->s.weapon = WP_PLASMAGUN;;
	turret->classname = "turret";	
	turret->s.modelindex = G_ModelIndex(TURRET_GUN);
	turret->model = TURRET_GUN;
	turret->s.modelindex2 = G_ModelIndex(TURRET_GUN);
	turret->think = turret_think;
	turret->nextthink = level.time + 100;
	G_SetOrigin( turret, ent->r.currentOrigin );
	VectorCopy(ent->s.apos.trBase, turret->s.apos.trBase);
	VectorCopy(turret->s.apos.trBase, turret->centerpoint);
	trap_LinkEntity (turret);
}

void turret_retaliate(gentity_t *self, gentity_t *attacker, int damage){
	// set the guns enemy to the person that shot it. (does not override targeting rules)
	if (self->chain)
		self->chain->enemy=attacker;

	// this is here to casue the turret to unshield when its taken a certain amount of damage. (enough to reduce health to below 100)
	if (self->s.time2 == 1) {
		if (self->health < 100) {
			self->s.time2 = 0;
			if (self->chain)
				self->chain->s.time2 = 0;
		}
	}
}



void turret_explode(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, int mod){
	/* This just blows up the base when its destroyed. took me some time to work it out */
	vec3_t dir; // needed by the event being added

	dir[0] = dir[1] = 0;
	dir[2] = 1;

	if (self->chain)
		G_FreeEntity(self->chain); // get rid of the gun. // the gun just vanishes

	self->s.weapon = WP_ROCKET_LAUNCHER; // to tell it what kind of explosion to use
	G_AddEvent( self, EV_MISSILE_MISS, DirToByte( dir ) ); // to tell it to spawn an explosion here
	self->freeAfterEvent = qtrue; // so the base goes away after the explosion
}

/*
==================
Cmd_SpawnTurret_f
==================
*/
void Cmd_SpawnTurret_f( gentity_t *ent ){
        gentity_t       *base;
        vec3_t          forward,up;

        base = G_Spawn();
        base->parent = ent;
        base->s.modelindex = G_ModelIndex(TURRET_BASE);
        base->model = TURRET_BASE;
        base->s.modelindex2 = G_ModelIndex(TURRET_BASE);
        G_SetOrigin(base, ent->r.currentOrigin);
        VectorSet(base->s.apos.trBase, 0, ent->s.apos.trBase[1], 0);
        base->think = createturretgun;
        base->health = 300; // change this to make the turrets tougher or weaker
        base->s.eType = ET_TURRET;
        base->s.time2 = 0;
        base->takedamage = qtrue; // so they can be destoryed
        base->die = turret_explode; // so they actually explode when destroyed
        base->pain = turret_retaliate; // if they are damaged they switch target to the person attacking (if its a valid target)
        base->nextthink = level.time+5000;
        VectorSet( base->r.absmin, -15, -15, -20 );
        VectorSet( base->r.absmax, 35, 15, -5);
        trap_LinkEntity (base);

}
