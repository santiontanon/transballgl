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
#include "TGLobject_bullet.h"
#include "TGLobject_enemy.h"
#include "TGLobject_FX_explosion1.h"
#include "TGLmap.h"

#include "debug.h"



TGLobject_soft_bullet::TGLobject_soft_bullet(float x,float y,int animation_offset,int angle,float speed,int power,GLTile *tile,TGLobject *ship) : TGLobject_bullet(x,y,animation_offset,angle,speed,power,tile,ship)
{
}


bool TGLobject_soft_bullet::is_a(Symbol *c)
{
	if (c->cmp("TGLobject_soft_bullet")) return true;
	return TGLobject::is_a(c);
} /* TGLobject_soft_bullet::is_a */ 


bool TGLobject_soft_bullet::is_a(char *c)
{
	bool retval;
	Symbol *s=new Symbol(c);

	retval=is_a(s);

	delete s;

	return retval;
} /* TGLobject_soft_bullet::is_a */ 


const char *TGLobject_soft_bullet::get_class(void)
{
	return "TGLobject_soft_bullet";
} /* TGLobject_soft_bullet::get_class */ 

