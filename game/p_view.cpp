/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#include "g_local.h"
#include "m_player.h"

float	bobmove;
int		bobcycle;		// odd cycles are right foot going forward
float	bobfracsin;		// sin(bobfrac*M_PI)

/*
===============
SV_CalcRoll

===============
*/
static inline float SV_CalcRoll (vec3_t angles, vec3_t velocity, vec3_t right)
{
	float	sign;
	float	side;
	float	value;
	
	side = DotProduct (velocity, right);
	sign = side < 0 ? -1 : 1;
	side = fabs(side);
	
	value = sv_rollangle->Float();

	if (side < sv_rollspeed->Float())
		side = side * value / sv_rollspeed->Float();
	else
		side = value;
	
	return side*sign;
	
}


/*
===============
P_DamageFeedback

Handles color blends and view kicks
===============
*/
static inline void P_DamageFeedback (edict_t *player, vec3_t forward, vec3_t right, vec3_t up)
{
	gclient_t	*client;
	float	side;
	float	realcount, count, kick;
	vec3_t	v;
	int		r, l;
	static	vec3_t	power_color = {0.0, 1.0, 0.0};
	static	vec3_t	acolor = {1.0, 1.0, 1.0};
	static	vec3_t	bcolor = {1.0, 0.0, 0.0};

	client = player->client;

	// flash the backgrounds behind the status numbers
	client->ps.stats[STAT_FLASHES] = 0;
	if (client->damage_blood)
		client->ps.stats[STAT_FLASHES] |= 1;
	if (client->damage_armor && !(player->flags & FL_GODMODE) && (client->invincible_framenum <= level.framenum))
		client->ps.stats[STAT_FLASHES] |= 2;

	// total points of damage shot at the player this frame
	count = (client->damage_blood + client->damage_armor + client->damage_parmor);
	if (count == 0)
		return;		// didn't take any damage

	// start a pain animation if still in the player model
	if (client->anim_priority < ANIM_PAIN && player->s.modelIndex == 255)
	{
		static int		i;

		client->anim_priority = ANIM_PAIN;
		if (client->ps.pMove.pmFlags & PMF_DUCKED)
		{
			player->s.frame = FRAME_crpain1-1;
			client->anim_end = FRAME_crpain4;
		}
		else
		{
			i = (i+1)%3;
			switch (i)
			{
			case 0:
				player->s.frame = FRAME_pain101-1;
				client->anim_end = FRAME_pain104;
				break;
			case 1:
				player->s.frame = FRAME_pain201-1;
				client->anim_end = FRAME_pain204;
				break;
			case 2:
				player->s.frame = FRAME_pain301-1;
				client->anim_end = FRAME_pain304;
				break;
			}
		}
	}

	realcount = count;
	if (count < 10)
		count = 10;	// always make a visible effect

	// play an apropriate pain sound
	if ((level.time > player->pain_debounce_time) && !(player->flags & FL_GODMODE) && (client->invincible_framenum <= level.framenum))
	{
		r = 1 + (rand()&1);
		player->pain_debounce_time = level.time + 0.7;
		if (player->health < 25)
			l = 25;
		else if (player->health < 50)
			l = 50;
		else if (player->health < 75)
			l = 75;
		else
			l = 100;
		Sound (player, CHAN_VOICE, SoundIndex(Q_VarArgs ("*pain%i_%i.wav", l, r)));
	}

	// the total alpha of the blend is always proportional to count
	if (client->damage_alpha < 0)
		client->damage_alpha = 0;
	client->damage_alpha += count*0.01f;
	if (client->damage_alpha < 0.2f)
		client->damage_alpha = 0.2f;
	if (client->damage_alpha > 0.6f)
		client->damage_alpha = 0.6f;		// don't go too saturated

	// the color of the blend will vary based on how much was absorbed
	// by different armors
	Vec3Clear (v);
	if (client->damage_parmor)
		Vec3MA (v, (float)client->damage_parmor/realcount, power_color, v);
	if (client->damage_armor)
		Vec3MA (v, (float)client->damage_armor/realcount,  acolor, v);
	if (client->damage_blood)
		Vec3MA (v, (float)client->damage_blood/realcount,  bcolor, v);
	Vec3Copy (v, client->damage_blend);


	//
	// calculate view angle kicks
	//
	kick = abs(client->damage_knockback);
	if (kick && player->health > 0)	// kick of 0 means no view adjust at all
	{
		kick = kick * 100 / player->health;

		if (kick < count*0.5)
			kick = count*0.5;
		if (kick > 50)
			kick = 50;

		Vec3Subtract (client->damage_from, player->s.origin, v);
		VectorNormalizef (v, v);
		
		side = DotProduct (v, right);
		client->v_dmg_roll = kick*side*0.3;
		
		side = -DotProduct (v, forward);
		client->v_dmg_pitch = kick*side*0.3;

		client->v_dmg_time = level.time + DAMAGE_TIME;
	}

	//
	// clear totals
	//
	client->damage_blood = 0;
	client->damage_armor = 0;
	client->damage_parmor = 0;
	client->damage_knockback = 0;
}




/*
===============
SV_CalcViewOffset

Auto pitching on slopes?

  fall from 128: 400 = 160000
  fall from 256: 580 = 336400
  fall from 384: 720 = 518400
  fall from 512: 800 = 640000
  fall from 640: 960 = 

  damage = deltavelocity*deltavelocity  * 0.0001

===============
*/
static inline void SV_CalcViewOffset (edict_t *ent, vec3_t forward, vec3_t right, vec3_t up, float xyspeed)
{
	float		*angles;
	float		bob;
	float		ratio;
	float		delta;
	vec3_t		v;


//===================================

	// base angles
	angles = ent->client->ps.kickAngles;

	// if dead, fix the angle and don't add any kick
	if (ent->deadflag)
	{
		Vec3Clear (angles);

		ent->client->ps.viewAngles[ROLL] = 40;
		ent->client->ps.viewAngles[PITCH] = -15;
		ent->client->ps.viewAngles[YAW] = ent->client->killer_yaw;
	}
	else
	{
		// add angles based on weapon kick

		Vec3Copy (ent->client->kick_angles, angles);

		// add angles based on damage kick

		ratio = (ent->client->v_dmg_time - level.time) / DAMAGE_TIME;
		if (ratio < 0)
		{
			ratio = 0;
			ent->client->v_dmg_pitch = 0;
			ent->client->v_dmg_roll = 0;
		}
		angles[PITCH] += ratio * ent->client->v_dmg_pitch;
		angles[ROLL] += ratio * ent->client->v_dmg_roll;

		// add pitch based on fall kick

		ratio = (ent->client->fall_time - level.time) / FALL_TIME;
		if (ratio < 0)
			ratio = 0;
		angles[PITCH] += ratio * ent->client->fall_value;

		// add angles based on velocity

		delta = DotProduct (ent->velocity, forward);
		angles[PITCH] += delta*run_pitch->Float();
		
		delta = DotProduct (ent->velocity, right);
		angles[ROLL] += delta*run_roll->Float();

		// add angles based on bob

		delta = bobfracsin * bob_pitch->Float() * xyspeed;
		if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		angles[PITCH] += delta;
		delta = bobfracsin * bob_roll->Float() * xyspeed;
		if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		if (bobcycle & 1)
			delta = -delta;
		angles[ROLL] += delta;
	}

//===================================

	// base origin

	Vec3Clear (v);

	// add view height

	v[2] += ent->viewheight;

	// add fall height

	ratio = (ent->client->fall_time - level.time) / FALL_TIME;
	if (ratio < 0)
		ratio = 0;
	v[2] -= ratio * ent->client->fall_value * 0.4;

	// add bob height

	bob = bobfracsin * xyspeed * bob_up->Float();
	if (bob > 6)
		bob = 6;
	//gi.DebugGraph (bob *2, 255);
	v[2] += bob;

	// add kick offset

	Vec3Add (v, ent->client->kick_origin, v);

	// absolutely bound offsets
	// so the view can never be outside the player box

	if (v[0] < -14)
		v[0] = -14;
	else if (v[0] > 14)
		v[0] = 14;
	if (v[1] < -14)
		v[1] = -14;
	else if (v[1] > 14)
		v[1] = 14;
	if (v[2] < -22)
		v[2] = -22;
	else if (v[2] > 30)
		v[2] = 30;

	Vec3Copy (v, ent->client->ps.viewOffset);
}

/*
==============
SV_CalcGunOffset
==============
*/
static inline void SV_CalcGunOffset (edict_t *ent, vec3_t forward, vec3_t right, vec3_t up, float xyspeed)
{
	int		i;
	float	delta;

	// gun angles from bobbing
	ent->client->ps.gunAngles[ROLL] = xyspeed * bobfracsin * 0.005;
	ent->client->ps.gunAngles[YAW] = xyspeed * bobfracsin * 0.01;
	if (bobcycle & 1)
	{
		ent->client->ps.gunAngles[ROLL] = -ent->client->ps.gunAngles[ROLL];
		ent->client->ps.gunAngles[YAW] = -ent->client->ps.gunAngles[YAW];
	}

	ent->client->ps.gunAngles[PITCH] = xyspeed * bobfracsin * 0.005;

	// gun angles from delta movement
	for (i=0 ; i<3 ; i++)
	{
		delta = ent->client->oldviewangles[i] - ent->client->ps.viewAngles[i];
		if (delta > 180)
			delta -= 360;
		if (delta < -180)
			delta += 360;
		if (delta > 45)
			delta = 45;
		if (delta < -45)
			delta = -45;
		if (i == YAW)
			ent->client->ps.gunAngles[ROLL] += 0.1*delta;
		ent->client->ps.gunAngles[i] += 0.2 * delta;
	}

	// gun height
	Vec3Clear (ent->client->ps.gunOffset);
//	ent->ps->gunorigin[2] += bob;

	// gun_x / gun_y / gun_z are development tools
	for (i=0 ; i<3 ; i++)
	{
		ent->client->ps.gunOffset[i] += forward[i]*(gun_y->Float());
		ent->client->ps.gunOffset[i] += right[i]*gun_x->Float();
		ent->client->ps.gunOffset[i] += up[i]* (-gun_z->Float());
	}
}


/*
=============
SV_AddBlend
=============
*/
static inline void SV_AddBlend (float r, float g, float b, float a, float *v_blend)
{
	float	a2, a3;

	if (a <= 0)
		return;
	a2 = v_blend[3] + (1-v_blend[3])*a;	// new total alpha
	a3 = v_blend[3]/a2;		// fraction of color from old

	v_blend[0] = v_blend[0]*a3 + r*(1-a3);
	v_blend[1] = v_blend[1]*a3 + g*(1-a3);
	v_blend[2] = v_blend[2]*a3 + b*(1-a3);
	v_blend[3] = a2;
}


/*
=============
SV_CalcBlend
=============
*/
static inline void SV_CalcBlend (edict_t *ent)
{
	int		contents;
	vec3_t	vieworg;
	int		remaining;

	ent->client->ps.viewBlend[0] = ent->client->ps.viewBlend[1] = 
		ent->client->ps.viewBlend[2] = ent->client->ps.viewBlend[3] = 0;

	// add for contents
	Vec3Add (ent->s.origin, ent->client->ps.viewOffset, vieworg);
	contents = gi.pointcontents (vieworg);
	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		ent->client->ps.rdFlags |= RDF_UNDERWATER;
	else
		ent->client->ps.rdFlags &= ~RDF_UNDERWATER;

	if (contents & (CONTENTS_SOLID|CONTENTS_LAVA))
		SV_AddBlend (1.0f, 0.3f, 0.0f, 0.6f, ent->client->ps.viewBlend);
	else if (contents & CONTENTS_SLIME)
		SV_AddBlend (0.0f, 0.1f, 0.05f, 0.6f, ent->client->ps.viewBlend);
	else if (contents & CONTENTS_WATER)
		SV_AddBlend (0.5f, 0.3f, 0.2f, 0.4f, ent->client->ps.viewBlend);

	// add for powerups
	if (ent->client->quad_framenum > level.framenum)
	{
		remaining = ent->client->quad_framenum - level.framenum;
		if (remaining == 30)	// beginning to fade
			Sound(ent, CHAN_ITEM, SoundIndex("items/damage2.wav"));
		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (0, 0, 1, 0.08f, ent->client->ps.viewBlend);
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		remaining = ent->client->invincible_framenum - level.framenum;
		if (remaining == 30)	// beginning to fade
			Sound(ent, CHAN_ITEM, SoundIndex("items/protect2.wav"));
		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (1, 1, 0, 0.08f, ent->client->ps.viewBlend);
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		remaining = ent->client->enviro_framenum - level.framenum;
		if (remaining == 30)	// beginning to fade
			Sound(ent, CHAN_ITEM, SoundIndex("items/airout.wav"));
		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (0, 1, 0, 0.08f, ent->client->ps.viewBlend);
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		remaining = ent->client->breather_framenum - level.framenum;
		if (remaining == 30)	// beginning to fade
			Sound(ent, CHAN_ITEM, SoundIndex("items/airout.wav"));
		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (0.4f, 1, 0.4f, 0.04f, ent->client->ps.viewBlend);
	}

	// add for damage
	if (ent->client->damage_alpha > 0)
		SV_AddBlend (ent->client->damage_blend[0],ent->client->damage_blend[1]
		,ent->client->damage_blend[2], ent->client->damage_alpha, ent->client->ps.viewBlend);

	if (ent->client->bonus_alpha > 0)
		SV_AddBlend (0.85f, 0.7f, 0.3f, ent->client->bonus_alpha, ent->client->ps.viewBlend);

	// drop the damage value
	ent->client->damage_alpha -= 0.06f;
	if (ent->client->damage_alpha < 0)
		ent->client->damage_alpha = 0;

	// drop the bonus value
	ent->client->bonus_alpha -= 0.1f;
	if (ent->client->bonus_alpha < 0)
		ent->client->bonus_alpha = 0;
}


/*
=================
P_FallingDamage
=================
*/
static inline void P_FallingDamage (edict_t *ent)
{
	float	delta;
	int		damage;
	vec3_t	dir;

	if (dmFlags.dfNoFallingDamage)
		return;

	if (ent->s.modelIndex != 255)
		return;		// not in the player model

	if (ent->movetype == MOVETYPE_NOCLIP)
		return;

	if ((ent->client->oldvelocity[2] < 0) && (ent->velocity[2] > ent->client->oldvelocity[2]) && (!ent->groundentity))
	{
		delta = ent->client->oldvelocity[2];
	}
	else
	{
		if (!ent->groundentity)
			return;
		delta = ent->velocity[2] - ent->client->oldvelocity[2];
	}
	delta = delta*delta * 0.0001;

	// never take falling damage if completely underwater
	if (ent->waterlevel == 3)
		return;
	if (ent->waterlevel == 2)
		delta *= 0.25;
	if (ent->waterlevel == 1)
		delta *= 0.5;

	if (delta < 1)
		return;

	if (delta < 15)
	{
		ent->s.event = EV_FOOTSTEP;
		return;
	}

	ent->client->fall_value = delta*0.5;
	if (ent->client->fall_value > 40)
		ent->client->fall_value = 40;
	ent->client->fall_time = level.time + FALL_TIME;

	if (delta > 30)
	{
		if (ent->health > 0)
		{
			if (delta >= 55)
				ent->s.event = EV_FALLFAR;
			else
				ent->s.event = EV_FALL;
		}
		ent->pain_debounce_time = level.time;	// no normal pain sound
		damage = (delta-30)/2;
		if (damage < 1)
			damage = 1;
		Vec3Set (dir, 0, 0, 1);

		if (!deathmatch->Integer() || !dmFlags.dfNoFallingDamage )
			T_Damage (ent, world, world, dir, ent->s.origin, vec3Origin, damage, 0, 0, MOD_FALLING);
	}
	else
	{
		ent->s.event = EV_FALLSHORT;
		return;
	}
}



/*
=============
P_WorldEffects
=============
*/
static inline void P_WorldEffects (edict_t *ent)
{
	bool	breather;
	bool	envirosuit;
	int			waterlevel, old_waterlevel;

	if (ent->movetype == MOVETYPE_NOCLIP)
	{
		ent->air_finished = level.time + 12;	// don't need air
		return;
	}

	waterlevel = ent->waterlevel;
	old_waterlevel = ent->client->old_waterlevel;
	ent->client->old_waterlevel = waterlevel;

	breather = (bool)(ent->client->breather_framenum > level.framenum);
	envirosuit = (bool)(ent->client->enviro_framenum > level.framenum);

	//
	// if just entered a water volume, play a sound
	//
	if (!old_waterlevel && waterlevel)
	{
		PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
		if (ent->watertype & CONTENTS_LAVA)
			Sound (ent, CHAN_BODY, SoundIndex("player/lava_in.wav"));
		else if (ent->watertype & CONTENTS_SLIME)
			Sound (ent, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		else if (ent->watertype & CONTENTS_WATER)
			Sound (ent, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		ent->flags |= FL_INWATER;

		// clear damage_debounce, so the pain sound will play immediately
		ent->pain_debounce_time = level.time - 1;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (old_waterlevel && ! waterlevel)
	{
		PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
		Sound (ent, CHAN_BODY, SoundIndex("player/watr_out.wav"));
		ent->flags &= ~FL_INWATER;
	}

	//
	// check for head just going under water
	//
	if (old_waterlevel != 3 && waterlevel == 3)
		Sound (ent, CHAN_BODY, SoundIndex("player/watr_un.wav"));

	//
	// check for head just coming out of water
	//
	if (old_waterlevel == 3 && waterlevel != 3)
	{
		if (ent->air_finished < level.time)
		{	// gasp for air
			Sound (ent, CHAN_VOICE, SoundIndex("player/gasp1.wav"));
			PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
		}
		else  if (ent->air_finished < level.time + 11) // just break surface
			Sound (ent, CHAN_VOICE, SoundIndex("player/gasp2.wav"));
	}

	//
	// check for drowning
	//
	if (waterlevel == 3)
	{
		// breather or envirosuit give air
		if (breather || envirosuit)
		{
			ent->air_finished = level.time + 10;

			if (((int)(ent->client->breather_framenum - level.framenum) % 25) == 0)
			{
				Sound (ent, CHAN_AUTO, SoundIndex((!ent->client->breather_sound) ? "player/u_breath1.wav" : "player/u_breath2.wav"));
				ent->client->breather_sound = !ent->client->breather_sound;
				PlayerNoise(ent, ent->s.origin, PNOISE_SELF);
				//FIXME: release a bubble?
			}
		}

		// if out of air, start drowning
		if (ent->air_finished < level.time)
		{	// drown!
			if (ent->client->next_drown_time < level.time 
				&& ent->health > 0)
			{
				ent->client->next_drown_time = level.time + 1;

				// take more damage the longer underwater
				ent->dmg += 2;
				if (ent->dmg > 15)
					ent->dmg = 15;

				// play a gurp sound instead of a normal pain sound
				if (ent->health <= ent->dmg)
					Sound (ent, CHAN_VOICE, SoundIndex("player/drown1.wav"));
				else if (rand()&1)
					Sound (ent, CHAN_VOICE, SoundIndex("*gurp1.wav"));
				else
					Sound (ent, CHAN_VOICE, SoundIndex("*gurp2.wav"));

				ent->pain_debounce_time = level.time;

				T_Damage (ent, world, world, vec3Origin, ent->s.origin, vec3Origin, ent->dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
			}
		}
	}
	else
	{
		ent->air_finished = level.time + 12;
		ent->dmg = 2;
	}

	//
	// check for sizzle damage
	//
	if (waterlevel && (ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
	{
		if (ent->watertype & CONTENTS_LAVA)
		{
			if (ent->health > 0
				&& ent->pain_debounce_time <= level.time
				&& ent->client->invincible_framenum < level.framenum)
			{
				Sound (ent, CHAN_VOICE, SoundIndex((rand()&1) ? "player/burn1.wav" : "player/burn2.wav"));
				ent->pain_debounce_time = level.time + 1;
			}

			// take 1/3 damage with envirosuit
			T_Damage (ent, world, world, vec3Origin, ent->s.origin, vec3Origin, (envirosuit) ? 1*waterlevel : 3*waterlevel, 0, 0, MOD_LAVA);
		}

		if (ent->watertype & CONTENTS_SLIME)
		{
			if (!envirosuit) // no damage from slime with envirosuit
				T_Damage (ent, world, world, vec3Origin, ent->s.origin, vec3Origin, 1*waterlevel, 0, 0, MOD_SLIME);
		}
	}
}


/*
===============
G_SetClientEffects
===============
*/
int PowerArmorType (edict_t *ent);
static inline void G_SetClientEffects (edict_t *ent)
{
	int		pa_type;
	int		remaining;

	ent->s.effects = 0;
	ent->s.renderFx = 0;

	if (ent->health <= 0 || level.intermissiontime)
		return;

	if (ent->powerarmor_time > level.time)
	{
		pa_type = PowerArmorType (ent);
		if (pa_type == POWER_ARMOR_SCREEN)
		{
			ent->s.effects |= EF_POWERSCREEN;
		}
		else if (pa_type == POWER_ARMOR_SHIELD)
		{
			ent->s.effects |= EF_COLOR_SHELL;
			ent->s.renderFx |= RF_SHELL_GREEN;
		}
	}

	if (ent->client->quad_framenum > level.framenum)
	{
		remaining = ent->client->quad_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			ent->s.effects |= EF_QUAD;
	}

	if (ent->client->invincible_framenum > level.framenum)
	{
		remaining = ent->client->invincible_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			ent->s.effects |= EF_PENT;
	}

	// show cheaters!!!
	if (ent->flags & FL_GODMODE)
	{
		ent->s.effects |= EF_COLOR_SHELL;
		ent->s.renderFx |= (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);
	}
}


/*
===============
G_SetClientEvent
===============
*/
static inline void G_SetClientEvent (edict_t *ent, float xyspeed)
{
	if (ent->s.event)
		return;

	if ( ent->groundentity && xyspeed > 225)
	{
		if ( (int)(ent->client->bobtime+bobmove) != bobcycle )
			ent->s.event = EV_FOOTSTEP;
	}
}

/*
===============
G_SetClientSound
===============
*/
void G_SetClientSound (edict_t *ent)
{
	char	*weap;

	if (ent->client->pers.game_helpchanged != game.helpchanged)
	{
		ent->client->pers.game_helpchanged = game.helpchanged;
		ent->client->pers.helpchanged = 1;
	}

	// help beep (no more than three times)
	if (ent->client->pers.helpchanged && ent->client->pers.helpchanged <= 3 && !(level.framenum&63) )
	{
		ent->client->pers.helpchanged++;
		Sound (ent, CHAN_VOICE, SoundIndex ("misc/pc_up.wav"), 1, ATTN_STATIC);
	}


	/*if (ent->client->pers.weapon)
		weap = ent->client->pers.weapon->classname;
	else*/
		weap = "";

	if (ent->waterlevel && (ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
		ent->s.sound = snd_fry;
	/*else if (strcmp(weap, "weapon_railgun") == 0)
		ent->s.sound = SoundIndex("weapons/rg_hum.wav");
	else if (strcmp(weap, "weapon_bfg") == 0)
		ent->s.sound = SoundIndex("weapons/bfg_hum.wav");*/
	else if (ent->client->pers.Weapon && ent->client->pers.Weapon->WeaponSound)
		ent->s.sound = SoundIndex(ent->client->pers.Weapon->WeaponSound);
	else if (ent->client->weapon_sound)
		ent->s.sound = ent->client->weapon_sound;
	else
		ent->s.sound = 0;
}

/*
===============
G_SetClientFrame
===============
*/
static inline void G_SetClientFrame (edict_t *ent, float xyspeed)
{
	gclient_t	*client;
	bool	duck, run;

	if (ent->s.modelIndex != 255)
		return;		// not in the player model

	client = ent->client;

	if (client->ps.pMove.pmFlags & PMF_DUCKED)
		duck = true;
	else
		duck = false;
	if (xyspeed)
		run = true;
	else
		run = false;

	// check for stand/duck and stop/go transitions
	if (duck != client->anim_duck && client->anim_priority < ANIM_DEATH)
		goto newanim;
	if (run != client->anim_run && client->anim_priority == ANIM_BASIC)
		goto newanim;
	if (!ent->groundentity && client->anim_priority <= ANIM_WAVE)
		goto newanim;

	if(client->anim_priority == ANIM_REVERSE)
	{
		if(ent->s.frame > client->anim_end)
		{
			ent->s.frame--;
			return;
		}
	}
	else if (ent->s.frame < client->anim_end)
	{	// continue an animation
		ent->s.frame++;
		return;
	}

	if (client->anim_priority == ANIM_DEATH)
		return;		// stay there
	if (client->anim_priority == ANIM_JUMP)
	{
		if (!ent->groundentity)
			return;		// stay there
		ent->client->anim_priority = ANIM_WAVE;
		ent->s.frame = FRAME_jump3;
		ent->client->anim_end = FRAME_jump6;
		return;
	}

newanim:
	// return to either a running or standing frame
	client->anim_priority = ANIM_BASIC;
	client->anim_duck = duck;
	client->anim_run = run;

	if (!ent->groundentity)
	{
		client->anim_priority = ANIM_JUMP;
		if (ent->s.frame != FRAME_jump2)
			ent->s.frame = FRAME_jump1;
		client->anim_end = FRAME_jump2;
	}
	else if (run)
	{	// running
		if (duck)
		{
			ent->s.frame = FRAME_crwalk1;
			client->anim_end = FRAME_crwalk6;
		}
		else
		{
			ent->s.frame = FRAME_run1;
			client->anim_end = FRAME_run6;
		}
	}
	else
	{	// standing
		if (duck)
		{
			ent->s.frame = FRAME_crstnd01;
			client->anim_end = FRAME_crstnd19;
		}
		else
		{
			ent->s.frame = FRAME_stand01;
			client->anim_end = FRAME_stand40;
		}
	}
}

/*
=================
ClientEndServerFrame

Called for each player at the end of the server frame
and right after spawning
=================
*/
void ClientEndServerFrame (edict_t *ent)
{
	float	bobtime;
	int		i;

	//
	// If the origin or velocity have changed since ClientThink(),
	// update the pmove values.  This will happen when the client
	// is pushed by a bmodel or kicked by an explosion.
	// 
	// If it wasn't updated here, the view position would lag a frame
	// behind the body position when pushed -- "sinking into plats"
	//
	for (i=0 ; i<3 ; i++)
	{
		ent->client->ps.pMove.origin[i] = ent->s.origin[i]*8.0;
		ent->client->ps.pMove.velocity[i] = ent->velocity[i]*8.0;
	}

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if (level.intermissiontime)
	{
		// FIXME: add view drifting here?
		ent->client->ps.viewBlend[3] = 0;
		ent->client->ps.fov = 90;
		G_SetStats (ent);
		return;
	}

	vec3_t forward, right, up;
	Angles_Vectors (ent->client->v_angle, forward, right, up);

	// burn from lava, etc
	P_WorldEffects (ent);

	//
	// set model angles from view angles so other things in
	// the world can tell which direction you are looking
	//
	if (ent->client->v_angle[PITCH] > 180)
		ent->s.angles[PITCH] = (-360 + ent->client->v_angle[PITCH])/3;
	else
		ent->s.angles[PITCH] = ent->client->v_angle[PITCH]/3;
	ent->s.angles[YAW] = ent->client->v_angle[YAW];
	ent->s.angles[ROLL] = 0;
	ent->s.angles[ROLL] = SV_CalcRoll (ent->s.angles, ent->velocity, right)*4;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	float xyspeed = sqrtf(ent->velocity[0]*ent->velocity[0] + ent->velocity[1]*ent->velocity[1]);

	if (xyspeed < 5)
	{
		bobmove = 0;
		ent->client->bobtime = 0;	// start at beginning of cycle again
	}
	else if (ent->groundentity)
	{	// so bobbing only cycles when on ground
		if (xyspeed > 210)
			bobmove = 0.25;
		else if (xyspeed > 100)
			bobmove = 0.125;
		else
			bobmove = 0.0625;
	}
	
	bobtime = (ent->client->bobtime += bobmove);

	if (ent->client->ps.pMove.pmFlags & PMF_DUCKED)
		bobtime *= 4;

	bobcycle = (int)bobtime;
	bobfracsin = fabs(sinf(bobtime*M_PI));

	// detect hitting the floor
	P_FallingDamage (ent);

	// apply all the damage taken this frame
	P_DamageFeedback (ent, forward, right, up);

	// determine the view offsets
	SV_CalcViewOffset (ent, forward, right, up, xyspeed);

	// determine the gun offsets
	SV_CalcGunOffset (ent, forward, right, up, xyspeed);

	// determine the full screen color blend
	// must be after viewoffset, so eye contents can be
	// accurately determined
	// FIXME: with client prediction, the contents
	// should be determined by the client
	SV_CalcBlend (ent);

	// chase cam stuff
	if (ent->client->resp.spectator)
		G_SetSpectatorStats(ent);
	else
		G_SetStats (ent);
	G_CheckChaseStats(ent);

	G_SetClientEvent (ent, xyspeed);

	G_SetClientEffects (ent);

	G_SetClientSound (ent);

	G_SetClientFrame (ent, xyspeed);

	Vec3Copy (ent->velocity, ent->client->oldvelocity);
	Vec3Copy (ent->client->ps.viewAngles, ent->client->oldviewangles);

	// clear weapon kicks
	Vec3Clear (ent->client->kick_origin);
	Vec3Clear (ent->client->kick_angles);

	// if the scoreboard is up, update it
	if (ent->client->showscores && !(level.framenum & 31) )
		DeathmatchScoreboardMessage (ent, ent->enemy, false);

	RunPlayerNodes(ent);
}

