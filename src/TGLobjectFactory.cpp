//
//  TGLobjectFactory.cpp
//  Transball GL
//
//  Created by Santiago Ontanon on 7/19/16.
//  Copyright © 2016 Brain Games. All rights reserved.
//

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif


#include "Symbol.h"
#include "BB2D.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "BB2D.h"
#include "sound.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"

#include "GLTManager.h"
#include "SFXManager.h"
#include "TGLmap.h"
#include "TGLobject.h"
#include "TGLobject_ballstand.h"
#include "TGLobject_redlight.h"
#include "TGLobject_radar.h"
#include "TGLobject_pipevscreen.h"
#include "TGLobject_pipehscreen.h"
#include "TGLobject_enemy.h"
#include "TGLobject_laser_left.h"
#include "TGLobject_laser_right.h"
#include "TGLobject_laser_up.h"
#include "TGLobject_laser_down.h"
#include "TGLobject_laser_horizontal.h"
#include "TGLobject_laser_vertical.h"
#include "TGLobject_cannon_left.h"
#include "TGLobject_cannon_right.h"
#include "TGLobject_cannon_down.h"
#include "TGLobject_cannon_up.h"
#include "TGLobject_fastcannon_left.h"
#include "TGLobject_fastcannon_right.h"
#include "TGLobject_fastcannon_down.h"
#include "TGLobject_fastcannon_up.h"
#include "TGLobject_ball.h"
#include "TGLobject_spike_left.h"
#include "TGLobject_spike_right.h"
#include "TGLobject_fuelrecharge.h"
#include "TGLobject_techno_computer.h"
#include "TGLobject_directionalcannon_left.h"
#include "TGLobject_directionalcannon_right.h"
#include "TGLobject_directionalcannon_up.h"
#include "TGLobject_directionalcannon_down.h"
#include "TGLobject_ha_directionalcannon_left.h"
#include "TGLobject_ha_directionalcannon_right.h"
#include "TGLobject_ha_directionalcannon_up.h"
#include "TGLobject_ha_directionalcannon_down.h"
#include "TGLobject_fast_directionalcannon_left.h"
#include "TGLobject_fast_directionalcannon_right.h"
#include "TGLobject_fast_directionalcannon_up.h"
#include "TGLobject_fast_directionalcannon_down.h"
#include "TGLobject_tank.h"
#include "TGLobject_tank_turret.h"
#include "TGLobject_tank_cannon.h"
#include "TGLobject_big_tank.h"
#include "TGLobject_big_tank_cannon.h"
#include "TGLobject_leftdoor.h"
#include "TGLobject_rightdoor.h"
#include "TGLobject_button.h"
#include "TGLobject_ship.h"

#include "debug.h"


void TGLobject::addObjectToMap(char *className, int x, int y, int animation_offset, int p1, int p2, TGLmap *map)
{
    if (strcmp(className,"ball-stand")==0) map->add_object(new TGLobject_ballstand(x,y,animation_offset));
    if (strcmp(className,"red-light")==0) map->add_object(new TGLobject_redlight(x,y,animation_offset,0));
    if (strcmp(className,"red-light-snow")==0) map->add_object(new TGLobject_redlight(x,y,animation_offset,1));
    if (strcmp(className,"radar")==0) map->add_object(new TGLobject_radar(x,y,animation_offset));
    if (strcmp(className,"pipe-vertical-screen")==0) map->add_object(new TGLobject_pipevscreen(x,y,animation_offset));
    if (strcmp(className,"pipe-horizontal-screen")==0) map->add_object(new TGLobject_pipehscreen(x,y,animation_offset));
    if (strcmp(className,"laser-left")==0) map->add_object(new TGLobject_laser_left(x,y,animation_offset));
    if (strcmp(className,"laser-right")==0) map->add_object(new TGLobject_laser_right(x,y,animation_offset));
    if (strcmp(className,"laser-up")==0) map->add_object(new TGLobject_laser_up(x,y,animation_offset));
    if (strcmp(className,"laser-down")==0) map->add_object(new TGLobject_laser_down(x,y,animation_offset));
    if (strcmp(className,"cannon-rock-left")==0) map->add_object(new TGLobject_cannon_left(x,y,animation_offset,0));
    if (strcmp(className,"cannon-rock-right")==0) map->add_object(new TGLobject_cannon_right(x,y,animation_offset,0));
    if (strcmp(className,"cannon-rock-down")==0) map->add_object(new TGLobject_cannon_down(x,y,animation_offset,0));
    if (strcmp(className,"cannon-rock-up")==0) map->add_object(new TGLobject_cannon_up(x,y,animation_offset,0));
    if (strcmp(className,"cannon-techno-left")==0) map->add_object(new TGLobject_cannon_left(x,y,animation_offset,1));
    if (strcmp(className,"cannon-techno-right")==0) map->add_object(new TGLobject_cannon_right(x,y,animation_offset,1));
    if (strcmp(className,"cannon-techno-down")==0) map->add_object(new TGLobject_cannon_down(x,y,animation_offset,1));
    if (strcmp(className,"cannon-techno-up")==0) map->add_object(new TGLobject_cannon_up(x,y,animation_offset,1));
    if (strcmp(className,"fastcannon-techno-left")==0) map->add_object(new TGLobject_fastcannon_left(x,y,animation_offset));
    if (strcmp(className,"fastcannon-techno-right")==0) map->add_object(new TGLobject_fastcannon_right(x,y,animation_offset));
    if (strcmp(className,"fastcannon-techno-down")==0) map->add_object(new TGLobject_fastcannon_down(x,y,animation_offset));
    if (strcmp(className,"fastcannon-techno-up")==0) map->add_object(new TGLobject_fastcannon_up(x,y,animation_offset));
    if (strcmp(className,"spike-left")==0) map->add_object(new TGLobject_spike_left(x,y,animation_offset));
    if (strcmp(className,"spike-right")==0) map->add_object(new TGLobject_spike_right(x,y,animation_offset));
    if (strcmp(className,"fuel-recharge")==0) map->add_object(new TGLobject_fuelrecharge(x,y,animation_offset));
    if (strcmp(className,"wall-techno-computer")==0) map->add_object(new TGLobject_techno_computer(x,y,animation_offset));
    if (strcmp(className,"directional-cannon-left")==0) map->add_object(new TGLobject_directionalcannon_left(x,y,animation_offset));
    if (strcmp(className,"directional-cannon-right")==0) map->add_object(new TGLobject_directionalcannon_right(x,y,animation_offset));
    if (strcmp(className,"directional-cannon-up")==0) map->add_object(new TGLobject_directionalcannon_up(x,y,animation_offset));
    if (strcmp(className,"directional-cannon-down")==0) map->add_object(new TGLobject_directionalcannon_down(x,y,animation_offset));
    if (strcmp(className,"ha-directional-cannon-left")==0) map->add_object(new TGLobject_ha_directionalcannon_left(x,y,animation_offset));
    if (strcmp(className,"ha-directional-cannon-right")==0) map->add_object(new TGLobject_ha_directionalcannon_right(x,y,animation_offset));
    if (strcmp(className,"ha-directional-cannon-up")==0) map->add_object(new TGLobject_ha_directionalcannon_up(x,y,animation_offset));
    if (strcmp(className,"ha-directional-cannon-down")==0) map->add_object(new TGLobject_ha_directionalcannon_down(x,y,animation_offset));
    if (strcmp(className,"fast-directional-cannon-left")==0) map->add_object(new TGLobject_fast_directionalcannon_left(x,y,animation_offset));
    if (strcmp(className,"fast-directional-cannon-right")==0) map->add_object(new TGLobject_fast_directionalcannon_right(x,y,animation_offset));
    if (strcmp(className,"fast-directional-cannon-up")==0) map->add_object(new TGLobject_fast_directionalcannon_up(x,y,animation_offset));
    if (strcmp(className,"fast-directional-cannon-down")==0) map->add_object(new TGLobject_fast_directionalcannon_down(x,y,animation_offset));
    if (strcmp(className,"grey-tank")==0) {
        TGLobject_enemy *tank,*turret,*cannon;
        tank=new TGLobject_tank(x,y,animation_offset,0);
        turret=new TGLobject_tank_turret(x,y,animation_offset,tank,0);
        cannon=new TGLobject_tank_cannon(x,y,animation_offset,turret);
        map->add_object(cannon);
        map->add_object(turret);
        map->add_object(tank);
        tank->exclude_for_collision(turret);
        tank->exclude_for_collision(cannon);
        turret->exclude_for_collision(tank);
        turret->exclude_for_collision(cannon);
        cannon->exclude_for_collision(tank);
        cannon->exclude_for_collision(turret);
    } // if
    if (strcmp(className,"red-tank")==0) {
        TGLobject_enemy *tank,*turret,*cannon;
        tank=new TGLobject_tank(x,y,animation_offset,1);
        turret=new TGLobject_tank_turret(x,y,animation_offset,tank,1);
        cannon=new TGLobject_tank_cannon(x,y,animation_offset,turret);
        map->add_object(cannon);
        map->add_object(turret);
        map->add_object(tank);
        tank->exclude_for_collision(turret);
        tank->exclude_for_collision(cannon);
        turret->exclude_for_collision(tank);
        turret->exclude_for_collision(cannon);
        cannon->exclude_for_collision(tank);
        cannon->exclude_for_collision(turret);
    } // if
    if (strcmp(className,"green-tank")==0) {
        TGLobject_enemy *tank,*turret,*cannon;
        tank=new TGLobject_tank(x,y,animation_offset,2);
        turret=new TGLobject_tank_turret(x,y,animation_offset,tank,2);
        cannon=new TGLobject_tank_cannon(x,y,animation_offset,turret);
        map->add_object(cannon);
        map->add_object(turret);
        map->add_object(tank);
        tank->exclude_for_collision(turret);
        tank->exclude_for_collision(cannon);
        turret->exclude_for_collision(tank);
        turret->exclude_for_collision(cannon);
        cannon->exclude_for_collision(tank);
        cannon->exclude_for_collision(turret);
    } // if
    if (strcmp(className,"big-tank")==0) {
        TGLobject_enemy *tank,*turret,*cannon;
        tank=new TGLobject_big_tank(x,y,animation_offset);
        turret=new TGLobject_tank_turret(x,y,animation_offset,tank,3);
        cannon=new TGLobject_big_tank_cannon(x,y,animation_offset,turret);
        map->add_object(cannon);
        map->add_object(turret);
        map->add_object(tank);
        tank->exclude_for_collision(turret);
        tank->exclude_for_collision(cannon);
        turret->exclude_for_collision(tank);
        turret->exclude_for_collision(cannon);
        cannon->exclude_for_collision(tank);
        cannon->exclude_for_collision(turret);
    } // if
    if (strcmp(className,"door-left")==0) {
        map->add_object(new TGLobject_leftdoor(x,y,animation_offset,p1,p2));
    } // if
    if (strcmp(className,"door-right")==0) {
        map->add_object(new TGLobject_rightdoor(x,y,animation_offset,p1,p2));
    } // if
    if (strcmp(className,"button-red-left")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,0));
    } // if
    if (strcmp(className,"button-red-right")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,1));
    } // if
    if (strcmp(className,"button-red-up")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,2));
    } // if
    if (strcmp(className,"button-red-down")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,3));
    } // if
    if (strcmp(className,"button-purple-left")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,4));
    } // if
    if (strcmp(className,"button-purple-right")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,5));
    } // if
    if (strcmp(className,"button-purple-up")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,6));
    } // if
    if (strcmp(className,"button-purple-down")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,7));
    } // if
    if (strcmp(className,"button-blue-left")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,8));
    } // if
    if (strcmp(className,"button-blue-right")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,9));
    } // if
    if (strcmp(className,"button-blue-up")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,10));
    } // if
    if (strcmp(className,"button-blue-down")==0) {
        map->add_object(new TGLobject_button(x,y,animation_offset,p1,11));
    } // if
}



TGLobject *TGLobject::createObjectFromClassName(char *className, int x, int y)
{
    if (strcmp(className,"ball-stand")==0) return new TGLobject_ballstand(x,y,0);
    if (strcmp(className,"red-light")==0) return new TGLobject_redlight(x,y,0,0);
    if (strcmp(className,"red-light-snow")==0) return new TGLobject_redlight(x,y,0,1);
    if (strcmp(className,"radar")==0) return new TGLobject_radar(x,y,0);
    if (strcmp(className,"pipe-vertical-screen")==0) return new TGLobject_pipevscreen(x,y,0);
    if (strcmp(className,"pipe-horizontal-screen")==0) return new TGLobject_pipehscreen(x,y,0);
    if (strcmp(className,"laser-left")==0) return new TGLobject_laser_left(x,y,0);
    if (strcmp(className,"laser-right")==0) return new TGLobject_laser_right(x,y,0);
    if (strcmp(className,"laser-up")==0) return new TGLobject_laser_up(x,y,0);
    if (strcmp(className,"laser-down")==0) return new TGLobject_laser_down(x,y,0);
    if (strcmp(className,"cannon-rock-left")==0) return new TGLobject_cannon_left(x,y,0,0);
    if (strcmp(className,"cannon-rock-right")==0) return new TGLobject_cannon_right(x,y,0,0);
    if (strcmp(className,"cannon-rock-down")==0) return new TGLobject_cannon_down(x,y,0,0);
    if (strcmp(className,"cannon-rock-up")==0) return new TGLobject_cannon_up(x,y,0,0);
    if (strcmp(className,"cannon-techno-left")==0) return new TGLobject_cannon_left(x,y,0,1);
    if (strcmp(className,"cannon-techno-right")==0) return new TGLobject_cannon_right(x,y,0,1);
    if (strcmp(className,"cannon-techno-down")==0) return new TGLobject_cannon_down(x,y,0,1);
    if (strcmp(className,"cannon-techno-up")==0) return new TGLobject_cannon_up(x,y,0,1);
    if (strcmp(className,"fastcannon-techno-left")==0) return new TGLobject_fastcannon_left(x,y,0);
    if (strcmp(className,"fastcannon-techno-right")==0) return new TGLobject_fastcannon_right(x,y,0);
    if (strcmp(className,"fastcannon-techno-down")==0) return new TGLobject_fastcannon_down(x,y,0);
    if (strcmp(className,"fastcannon-techno-up")==0) return new TGLobject_fastcannon_up(x,y,0);
    if (strcmp(className,"spike-left")==0) return new TGLobject_spike_left(x,y,0);
    if (strcmp(className,"spike-right")==0) return new TGLobject_spike_right(x,y,0);
    if (strcmp(className,"fuel-recharge")==0) return new TGLobject_fuelrecharge(x,y,0);
    if (strcmp(className,"wall-techno-computer")==0) return new TGLobject_techno_computer(x,y,0);
    if (strcmp(className,"directional-cannon-left")==0) return new TGLobject_directionalcannon_left(x,y,0);
    if (strcmp(className,"directional-cannon-right")==0) return new TGLobject_directionalcannon_right(x,y,0);
    if (strcmp(className,"directional-cannon-up")==0) return new TGLobject_directionalcannon_up(x,y,0);
    if (strcmp(className,"directional-cannon-down")==0) return new TGLobject_directionalcannon_down(x,y,0);
    if (strcmp(className,"ha-directional-cannon-left")==0) return new TGLobject_ha_directionalcannon_left(x,y,0);
    if (strcmp(className,"ha-directional-cannon-right")==0) return new TGLobject_ha_directionalcannon_right(x,y,0);
    if (strcmp(className,"ha-directional-cannon-up")==0) return new TGLobject_ha_directionalcannon_up(x,y,0);
    if (strcmp(className,"ha-directional-cannon-down")==0) return new TGLobject_ha_directionalcannon_down(x,y,0);
    if (strcmp(className,"fast-directional-cannon-left")==0) return new TGLobject_fast_directionalcannon_left(x,y,0);
    if (strcmp(className,"fast-directional-cannon-right")==0) return new TGLobject_fast_directionalcannon_right(x,y,0);
    if (strcmp(className,"fast-directional-cannon-up")==0) return new TGLobject_fast_directionalcannon_up(x,y,0);
    if (strcmp(className,"fast-directional-cannon-down")==0) return new TGLobject_fast_directionalcannon_down(x,y,0);
    output_debug_message("TGLobject::createObjectFromClassName: could not create an object with class name \"%s\"\n", className);
    
    return 0;
}

