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
	int		l;
	static	const colorb	PowerColor = colorb(0, 255, 0, 0);
	static	const colorb	ArmorColor = colorb(255, 255, 255, 0);
	static	const colorb	BloodColor = colorb(255, 0, 0, 0);

	client = player->client;

	// flash the backgrounds behind the status numbers
	client->playerState.stats[STAT_FLASHES] = 0;
	if (client->damage_blood)
		client->playerState.stats[STAT_FLASHES] |= 1;
	if (client->damage_armor && !(player->flags & FL_GODMODE) && (client->invincible_framenum <= level.framenum))
		client->playerState.stats[STAT_FLASHES] |= 2;

	// total points of damage shot at the player this frame
	count = (client->damage_blood + client->damage_armor + client->damage_parmor);
	if (count == 0)
		return;		// didn't take any damage

	// start a pain animation if still in the player model
	if (client->anim_priority < ANIM_PAIN && player->state.modelIndex == 255)
	{
		static int		i;

		client->anim_priority = ANIM_PAIN;
		if (client->playerState.pMove.pmFlags & PMF_DUCKED)
		{
			player->state.frame = FRAME_crpain1-1;
			client->anim_end = FRAME_crpain4;
		}
		else
		{
			i = (i+1)%3;
			switch (i)
			{
			case 0:
				player->state.frame = FRAME_pain101-1;
				client->anim_end = FRAME_pain104;
				break;
			case 1:
				player->state.frame = FRAME_pain201-1;
				client->anim_end = FRAME_pain204;
				break;
			case 2:
				player->state.frame = FRAME_pain301-1;
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
		player->pain_debounce_time = level.time + 0.7;

		l = (int)clamp(((floorf((max(0, player->health-1)) / 25))), 0, 3);
		PlaySoundFrom (player, CHAN_VOICE, gMedia.Player.Pain[l][(rand()&1)]);
	}

	// the total alpha of the blend is always proportional to count
	byte Alpha = client->damage_blend.A + count*3;
	if (Alpha < 51)
		Alpha = 51;
	if (Alpha > 153)
		Alpha = 153;		// don't go too saturated

	// the color of the blend will vary based on how much was absorbed
	// by different armors
	client->damage_blend.Set (
							(BloodColor.R * (client->damage_blood/realcount)) + (ArmorColor.R * (client->damage_armor/realcount)) + (PowerColor.R * (client->damage_parmor/realcount)),
							(BloodColor.G * (client->damage_blood/realcount)) + (ArmorColor.G * (client->damage_armor/realcount)) + (PowerColor.G * (client->damage_parmor/realcount)),
							(BloodColor.B * (client->damage_blood/realcount)) + (ArmorColor.B * (client->damage_armor/realcount)) + (PowerColor.B * (client->damage_parmor/realcount)),
							Alpha);

	//
	// calculate view angle kicks
	//
	kick = abs(client->damage_knockback);
	if (kick && player->health > 0)	// kick of 0 means no view adjust at all
	{
		vec3_t v;
		kick = kick * 100 / player->health;

		if (kick < count*0.5)
			kick = count*0.5;
		if (kick > 50)
			kick = 50;

		Vec3Subtract (client->damage_from, player->state.origin, v);
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
	angles = ent->client->playerState.kickAngles;

	// if dead, fix the angle and don't add any kick
	if (ent->deadflag)
	{
		Vec3Clear (angles);

		ent->client->playerState.viewAngles[ROLL] = 40;
		ent->client->playerState.viewAngles[PITCH] = -15;
		ent->client->playerState.viewAngles[YAW] = ent->client->killer_yaw;
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
		if (ent->client->playerState.pMove.pmFlags & PMF_DUCKED)
			delta *= 6;		// crouching
		angles[PITCH] += delta;
		delta = bobfracsin * bob_roll->Float() * xyspeed;
		if (ent->client->playerState.pMove.pmFlags & PMF_DUCKED)
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

	Vec3Copy (v, ent->client->playerState.viewOffset);
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
	ent->client->playerState.gunAngles[ROLL] = xyspeed * bobfracsin * 0.005;
	ent->client->playerState.gunAngles[YAW] = xyspeed * bobfracsin * 0.01;
	if (bobcycle & 1)
	{
		ent->client->playerState.gunAngles[ROLL] = -ent->client->playerState.gunAngles[ROLL];
		ent->client->playerState.gunAngles[YAW] = -ent->client->playerState.gunAngles[YAW];
	}

	ent->client->playerState.gunAngles[PITCH] = xyspeed * bobfracsin * 0.005;

	// gun angles from delta movement
	for (i=0 ; i<3 ; i++)
	{
		delta = ent->client->oldviewangles[i] - ent->client->playerState.viewAngles[i];
		if (delta > 180)
			delta -= 360;
		if (delta < -180)
			delta += 360;
		if (delta > 45)
			delta = 45;
		if (delta < -45)
			delta = -45;
		if (i == YAW)
			ent->client->playerState.gunAngles[ROLL] += 0.1*delta;
		ent->client->playerState.gunAngles[i] += 0.2 * delta;
	}

	// gun height
	Vec3Clear (ent->client->playerState.gunOffset);
//	ent->ps->gunorigin[2] += bob;

	// gun_x / gun_y / gun_z are development tools
	for (i=0 ; i<3 ; i++)
	{
		ent->client->playerState.gunOffset[i] += forward[i]*(gun_y->Float());
		ent->client->playerState.gunOffset[i] += right[i]*gun_x->Float();
		ent->client->playerState.gunOffset[i] += up[i]* (-gun_z->Float());
	}
}


/*
=============
SV_AddBlend
=============
*/
static inline void SV_AddBlend (colorb color, colorb &v_blend)
{
	byte	a2, a3;

	if (color.A <= 0)
		return;

	a2 = v_blend.A + color.A;	// new total alpha
	a3 = v_blend.A/a2;		// fraction of color from old

	v_blend.R = v_blend.R*a3 + color.R;
	v_blend.G = v_blend.G*a3 + color.G;
	v_blend.B = v_blend.B*a3 + color.B;
	v_blend.A = a2;
}


/*
=============
SV_CalcBlend
=============
*/
static inline void SV_CalcBlend (edict_t *ent)
{
	static const colorb LavaColor = colorb(255, 76, 0, 153);
	static const colorb SlimeColor = colorb(0, 25, 13, 153);
	static const colorb WaterColor = colorb(127, 76, 51, 102);
	static const colorb QuadColor = colorb(0, 0, 255, 20);
	static const colorb InvulColor = colorb(255, 255, 0, 20);
	static const colorb EnviroColor = colorb(0, 255, 0, 20);
	static const colorb BreatherColor = colorb(102, 255, 102, 10);
	static const colorb ClearColor = colorb(0,0,0,0);
	static colorb BonusColor = colorb(217, 178, 76, 0);
	
	ent->client->pers.viewBlend = ClearColor;

	// add for contents
	vec3_t	vieworg;
	Vec3Add (ent->state.origin, ent->client->playerState.viewOffset, vieworg);
	int contents = gi.pointcontents (vieworg);

	if (contents & (CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER) )
		ent->client->playerState.rdFlags |= RDF_UNDERWATER;
	else
		ent->client->playerState.rdFlags &= ~RDF_UNDERWATER;

	// add for powerups
	if (ent->client->quad_framenum > level.framenum)
	{
		int remaining = ent->client->quad_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/damage2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (QuadColor, ent->client->pers.viewBlend);
	}
	else if (ent->client->invincible_framenum > level.framenum)
	{
		int remaining = ent->client->invincible_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/protect2.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (InvulColor, ent->client->pers.viewBlend);
	}
	else if (ent->client->enviro_framenum > level.framenum)
	{
		int remaining = ent->client->enviro_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (EnviroColor, ent->client->pers.viewBlend);
	}
	else if (ent->client->breather_framenum > level.framenum)
	{
		int remaining = ent->client->breather_framenum - level.framenum;

		if (remaining == 30)	// beginning to fade
			PlaySoundFrom(ent, CHAN_ITEM, SoundIndex("items/airout.wav"));

		if (remaining > 30 || (remaining & 4) )
			SV_AddBlend (BreatherColor, ent->client->pers.viewBlend);
	}

	// add for damage
	if (ent->client->damage_blend.A)
		SV_AddBlend (ent->client->damage_blend, ent->client->pers.viewBlend);

	// drop the damage value
	if (ent->client->damage_blend.A)
	{
		if (ent->client->damage_blend.A < 15)
			ent->client->damage_blend.A = 0;
		else
			ent->client->damage_blend.A -= 15;
	}

	// add bonus and drop the value
	if (ent->client->bonus_alpha)
	{
		BonusColor.A = ent->client->bonus_alpha;
		SV_AddBlend (BonusColor, ent->client->pers.viewBlend);

		if (ent->client->bonus_alpha < 15)
			ent->client->bonus_alpha = 0;
		else
			ent->client->bonus_alpha -= 15;
	}

	if (contents & (CONTENTS_SOLID|CONTENTS_LAVA))
		SV_AddBlend (LavaColor, ent->client->pers.viewBlend);
	else if (contents & CONTENTS_SLIME)
		SV_AddBlend (SlimeColor, ent->client->pers.viewBlend);
	else if (contents & CONTENTS_WATER)
		SV_AddBlend (WaterColor, ent->client->pers.viewBlend);

	ent->client->playerState.viewBlend[0] = ((float)(ent->client->pers.viewBlend.R) / 255);
	ent->client->playerState.viewBlend[1] = ((float)(ent->client->pers.viewBlend.G) / 255);
	ent->client->playerState.viewBlend[2] = ((float)(ent->client->pers.viewBlend.B) / 255);
	ent->client->playerState.viewBlend[3] = ((float)(ent->client->pers.viewBlend.A) / 255);
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

	if (ent->state.modelIndex != 255)
		return;		// not in the player model

	if (ent->movetype == MOVETYPE_NOCLIP)
		return;

#ifdef CLEANCTF_ENABLED
//ZOID
	// never take damage if just release grapple or on grapple
	if (level.time - ent->client->ctf_grapplereleasetime <= FRAMETIME * 2 ||
		(ent->client->ctf_grapple && 
		ent->client->ctf_grapplestate > CTF_GRAPPLE_STATE_FLY))
		return;
//ZOID
#endif

	if ((ent->client->oldvelocity[2] < 0) && (ent->velocity[2] > ent->client->oldvelocity[2]) && (!ent->groundentity))
		delta = ent->client->oldvelocity[2];
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
		ent->state.event = EV_FOOTSTEP;
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
				ent->state.event = EV_FALLFAR;
			else
				ent->state.event = EV_FALL;
		}
		ent->pain_debounce_time = level.time;	// no normal pain sound
		damage = (delta-30)/2;
		if (damage < 1)
			damage = 1;
		Vec3Set (dir, 0, 0, 1);

		if (!dmFlags.dfNoFallingDamage )
			T_Damage (ent, world, world, dir, ent->state.origin, vec3Origin, damage, 0, 0, MOD_FALLING);
	}
	else
	{
		ent->state.event = EV_FALLSHORT;
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
		PlayerNoise(ent, ent->state.origin, PNOISE_SELF);
		if (ent->watertype & CONTENTS_LAVA)
			PlaySoundFrom (ent, CHAN_BODY, SoundIndex("player/lava_in.wav"));
		else if (ent->watertype & CONTENTS_SLIME)
			PlaySoundFrom (ent, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		else if (ent->watertype & CONTENTS_WATER)
			PlaySoundFrom (ent, CHAN_BODY, SoundIndex("player/watr_in.wav"));
		ent->flags |= FL_INWATER;

		// clear damage_debounce, so the pain sound will play immediately
		ent->pain_debounce_time = level.time - 1;
	}

	//
	// if just completely exited a water volume, play a sound
	//
	if (old_waterlevel && ! waterlevel)
	{
		PlayerNoise(ent, ent->state.origin, PNOISE_SELF);
		PlaySoundFrom (ent, CHAN_BODY, SoundIndex("player/watr_out.wav"));
		ent->flags &= ~FL_INWATER;
	}

	//
	// check for head just going under water
	//
	if (old_waterlevel != 3 && waterlevel == 3)
		PlaySoundFrom (ent, CHAN_BODY, SoundIndex("player/watr_un.wav"));

	//
	// check for head just coming out of water
	//
	if (old_waterlevel == 3 && waterlevel != 3)
	{
		if (ent->air_finished < level.time)
		{	// gasp for air
			PlaySoundFrom (ent, CHAN_VOICE, SoundIndex("player/gasp1.wav"));
			PlayerNoise(ent, ent->state.origin, PNOISE_SELF);
		}
		else  if (ent->air_finished < level.time + 11) // just break surface
			PlaySoundFrom (ent, CHAN_VOICE, SoundIndex("player/gasp2.wav"));
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
				PlaySoundFrom (ent, CHAN_AUTO, SoundIndex((!ent->client->breather_sound) ? "player/u_breath1.wav" : "player/u_breath2.wav"));
				ent->client->breather_sound = !ent->client->breather_sound;
				PlayerNoise(ent, ent->state.origin, PNOISE_SELF);
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
					PlaySoundFrom (ent, CHAN_VOICE, SoundIndex("player/drown1.wav"));
				else
					PlaySoundFrom (ent, CHAN_VOICE, gMedia.Player.Gurp[(rand()&1)]);

				ent->pain_debounce_time = level.time;

				T_Damage (ent, world, world, vec3Origin, ent->state.origin, vec3Origin, ent->dmg, 0, DAMAGE_NO_ARMOR, MOD_WATER);
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
				PlaySoundFrom (ent, CHAN_VOICE, SoundIndex((rand()&1) ? "player/burn1.wav" : "player/burn2.wav"));
				ent->pain_debounce_time = level.time + 1;
			}

			// take 1/3 damage with envirosuit
			T_Damage (ent, world, world, vec3Origin, ent->state.origin, vec3Origin, (envirosuit) ? 1*waterlevel : 3*waterlevel, 0, 0, MOD_LAVA);
		}

		if (ent->watertype & CONTENTS_SLIME)
		{
			if (!envirosuit) // no damage from slime with envirosuit
				T_Damage (ent, world, world, vec3Origin, ent->state.origin, vec3Origin, 1*waterlevel, 0, 0, MOD_SLIME);
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

	ent->state.effects = 0;
	ent->state.renderFx = 0;

	if (ent->health <= 0 || level.intermissiontime)
		return;

	if (ent->powerarmor_time > level.time)
	{
		pa_type = PowerArmorType (ent);
		if (pa_type == POWER_ARMOR_SCREEN)
		{
			ent->state.effects |= EF_POWERSCREEN;
		}
		else if (pa_type == POWER_ARMOR_SHIELD)
		{
			ent->state.effects |= EF_COLOR_SHELL;
			ent->state.renderFx |= RF_SHELL_GREEN;
		}
	}

#ifdef CLEANCTF_ENABLED
//ZOID
	CTFEffects(ent);
//ZOID
#endif

	if (ent->client->quad_framenum > level.framenum)
	{
		remaining = ent->client->quad_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			ent->state.effects |= EF_QUAD;
	}

	if (ent->client->invincible_framenum > level.framenum)
	{
		remaining = ent->client->invincible_framenum - level.framenum;
		if (remaining > 30 || (remaining & 4) )
			ent->state.effects |= EF_PENT;
	}

	// show cheaters!!!
	if (ent->flags & FL_GODMODE)
	{
		ent->state.effects |= EF_COLOR_SHELL;
		ent->state.renderFx |= (RF_SHELL_RED|RF_SHELL_GREEN|RF_SHELL_BLUE);
	}
}


/*
===============
G_SetClientEvent
===============
*/
static inline void G_SetClientEvent (edict_t *ent, float xyspeed)
{
	if (ent->state.event)
		return;

	if ( ent->groundentity && xyspeed > 225)
	{
		if ( (int)(ent->client->bobtime+bobmove) != bobcycle )
			ent->state.event = EV_FOOTSTEP;
	}
}

/*
===============
G_SetClientSound
===============
*/
static inline void G_SetClientSound (edict_t *ent)
{
	if (ent->client->pers.game_helpchanged != game.helpchanged)
	{
		ent->client->pers.game_helpchanged = game.helpchanged;
		ent->client->pers.helpchanged = 1;
	}

	// help beep (no more than three times)
	if (ent->client->pers.helpchanged && ent->client->pers.helpchanged <= 3 && !(level.framenum&63) )
	{
		ent->client->pers.helpchanged++;
		PlaySoundFrom (ent, CHAN_VOICE, SoundIndex ("misc/pc_up.wav"), 1, ATTN_STATIC);
	}

	if (ent->waterlevel && (ent->watertype&(CONTENTS_LAVA|CONTENTS_SLIME)) )
		ent->state.sound = gMedia.FrySound;
	else if (ent->client->pers.Weapon && ent->client->pers.Weapon->WeaponSoundIndex)
		ent->state.sound = ent->client->pers.Weapon->WeaponSoundIndex;
	else if (ent->client->weapon_sound)
		ent->state.sound = ent->client->weapon_sound;
	else
		ent->state.sound = 0;
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

	if (ent->state.modelIndex != 255)
		return;		// not in the player model

	client = ent->client;

	if (client->playerState.pMove.pmFlags & PMF_DUCKED)
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
		if(ent->state.frame > client->anim_end)
		{
			ent->state.frame--;
			return;
		}
	}
	else if (ent->state.frame < client->anim_end)
	{	// continue an animation
		ent->state.frame++;
		return;
	}

	if (client->anim_priority == ANIM_DEATH)
		return;		// stay there
	if (client->anim_priority == ANIM_JUMP)
	{
		if (!ent->groundentity)
			return;		// stay there
		ent->client->anim_priority = ANIM_WAVE;
		ent->state.frame = FRAME_jump3;
		ent->client->anim_end = FRAME_jump6;
		return;
	}

newanim:
	// return to either a running or standing frame
	client->anim_priority = ANIM_BASIC;
	client->anim_duck = duck;
	client->anim_run = run;

	if (!ent->groundentity && !duck)
	{
#ifdef CLEANCTF_ENABLED
//ZOID: if on grapple, don't go into jump frame, go into standing
//frame
		if (client->ctf_grapple)
		{
			ent->state.frame = FRAME_stand01;
			client->anim_end = FRAME_stand40;
		}
		else
		{
//ZOID
#endif
			client->anim_priority = ANIM_JUMP;
			if (ent->state.frame != FRAME_jump2)
				ent->state.frame = FRAME_jump1;
			client->anim_end = FRAME_jump2;
#ifdef CLEANCTF_ENABLED
		}
#endif
	}
	else if (run)
	{	// running
		if (duck)
		{
			ent->state.frame = FRAME_crwalk1;
			client->anim_end = FRAME_crwalk6;
		}
		else
		{
			ent->state.frame = FRAME_run1;
			client->anim_end = FRAME_run6;
		}
	}
	else
	{	// standing
		if (duck)
		{
			ent->state.frame = FRAME_crstnd01;
			client->anim_end = FRAME_crstnd19;
		}
		else
		{
			ent->state.frame = FRAME_stand01;
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
		ent->client->playerState.pMove.origin[i] = ent->state.origin[i]*8.0;
		ent->client->playerState.pMove.velocity[i] = ent->velocity[i]*8.0;
	}

	//
	// If the end of unit layout is displayed, don't give
	// the player any normal movement attributes
	//
	if (level.intermissiontime)
	{
		// FIXME: add view drifting here?
		ent->client->playerState.viewBlend[3] = 0;
		ent->client->playerState.fov = 90;
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
		ent->state.angles[PITCH] = (-360 + ent->client->v_angle[PITCH])/3;
	else
		ent->state.angles[PITCH] = ent->client->v_angle[PITCH]/3;
	ent->state.angles[YAW] = ent->client->v_angle[YAW];
	ent->state.angles[ROLL] = 0;
	ent->state.angles[ROLL] = SV_CalcRoll (ent->state.angles, ent->velocity, right)*4;

	//
	// calculate speed and cycle to be used for
	// all cyclic walking effects
	//
	float xyspeed = sqrtf(ent->velocity[0]*ent->velocity[0] + ent->velocity[1]*ent->velocity[1]);

	if (xyspeed < 5 || ent->client->playerState.pMove.pmFlags & PMF_DUCKED)
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

	if (ent->client->playerState.pMove.pmFlags & PMF_DUCKED)
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
#ifndef CLEANCTF_ENABLED
	if (ent->client->resp.spectator)
		G_SetSpectatorStats(ent);
	else
#else
//ZOID
	if (!ent->client->chase_target)
//ZOID
#endif
		G_SetStats (ent);

#ifdef CLEANCTF_ENABLED
//ZOID
//update chasecam follower stats
	for (i = 1; i <= game.maxclients; i++) {
		edict_t *e = g_edicts + i;
		if (!e->inUse || e->client->chase_target != ent)
			continue;
		memcpy(e->client->playerState.stats, 
			ent->client->playerState.stats, 
			sizeof(ent->client->playerState.stats));
		e->client->playerState.stats[STAT_LAYOUTS] = 1;
		break;
	}
//ZOID
#else
	G_CheckChaseStats(ent);
#endif

	G_SetClientEvent (ent, xyspeed);

	G_SetClientEffects (ent);

	G_SetClientSound (ent);

	G_SetClientFrame (ent, xyspeed);

	Vec3Copy (ent->velocity, ent->client->oldvelocity);
	Vec3Copy (ent->client->playerState.viewAngles, ent->client->oldviewangles);

	// clear weapon kicks
	Vec3Clear (ent->client->kick_origin);
	Vec3Clear (ent->client->kick_angles);

	// if the scoreboard is up, update it
	if (ent->client->showscores && !(level.framenum & 31) )
		DeathmatchScoreboardMessage (ent, ent->enemy, false);
	else if (ent->client->resp.MenuState.InMenu && !(level.framenum & 4))
		ent->client->resp.MenuState.CurrentMenu->Draw (false);
}

