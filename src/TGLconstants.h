//
//  TGLconstants.h
//  Transball GL
//
//  Created by Santiago Ontanon on 7/9/16.
//  Copyright © 2016 Brain Games. All rights reserved.
//

#ifndef TGLconstants_h
#define TGLconstants_h


#define MAXLOCAL_PLAYERS	4

#define TGL_STATE_NONE	-1
#define TGL_STATE_PLAYERPROFILE	1
#define TGL_STATE_INTRO			2
#define TGL_STATE_LEVELPACKSCREEN	3
#define TGL_STATE_LEVELPACKBROWSER	4
#define TGL_STATE_MAINMENU		5
#define TGL_STATE_CONFIGURE		6
#define TGL_STATE_PREGAME		7
#define TGL_STATE_GAME			8
#define TGL_STATE_POSTGAME		9
#define TGL_STATE_SAVEREPLAY	10
#define TGL_STATE_REPLAYBROWSER	11
#define TGL_STATE_HIGHSCORES	12
#define TGL_STATE_HIGHSCORES_TIMES	13
#define TGL_STATE_EDITOR			14
#define TGL_STATE_MAPEDITOR			15
#define TGL_STATE_LOADLEVELPACK		16
#define TGL_STATE_LOADMAP		17
#define TGL_STATE_SAVEMAP		18

#define KEY_THRUST		0
#define KEY_SPECIAL		1
#define KEY_LEFT		2
#define KEY_RIGHT		3
#define KEY_FIRE		4
#define KEY_ATTRACTOR	5
#define KEY_MENU		6
#define NUMBER_OF_REDEFINABLE_KEYS  7

#define SAVEREPLAY_REPLAYSPERPAGE	12
#define LEVELPACKBROWSER_LPPERPAGE	10
#define MAPBROWSER_MPERPAGE			16
#define MAPBROWSER_MPERPAGE_SAVE	12
#define PROFILESPERPAGE				12
#define HIGHSCORE_GLOBAL_PERPAGE	14

#define MIN_ZOOM    0.5f
#define MAX_ZOOM    2.0f

#define DEFAULT_SCREEN_X    640
#define DEFAULT_SCREEN_Y    480


extern int SCREEN_X;
extern int SCREEN_Y;

#endif /* TGLconstants_h */
