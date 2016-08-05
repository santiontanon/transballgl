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
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"

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
#include "TGLobject.h"
#include "TGLobject_enemy.h"
#include "TGLmap.h"

#include "debug.h"




TGLobject_enemy::TGLobject_enemy(float x,float y,int ao) : TGLobject(x,y,ao)
{
	m_hitpoints=1;
	m_hit=false;
} /* TGLobject_enemy::TGLobject_enemy */ 


TGLobject_enemy::~TGLobject_enemy()
{
} /* TGLobject_enemy::~TGLobject_enemy */ 


bool TGLobject_enemy::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_enemy")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_enemy::is_a */ 


bool TGLobject_enemy::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_enemy::is_a */ 


const char *TGLobject_enemy::get_class(void)
{
	return "TGLobject_enemy";
} /* TGLobject_enemy::get_class */ 


void TGLobject_enemy::hit(int power)
{
	m_hitpoints-=power;
	m_hit=true;
} /* TGLobject_enemy::is_a */ 


int TGLobject_enemy::get_hitpoints(void)
{
	return m_hitpoints;
} /* TGLobject_enemy::get_hitpoints */ 


void TGLobject_enemy::set_hitpoints(int hp)
{
	m_hitpoints=hp;
} /* TGLobject_enemy::set_hitpoints */ 


