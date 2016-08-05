#ifndef _TGL_GAME
#define _TGL_GAME

#include <list>

#include "GLTManager.h"
#include "SFXManager.h"
#include "TGLobject.h"
#include "TGLmap.h"

#define N_SHIPS		12

class TGL {
public:
    static int s_points_for_ship[N_SHIPS];
    
    
    TGL(class TGLreplay *replay,int sfx_volume, int music_volume, GLTManager *GLTM);
    TGL(char *map, int ship,int initial_fuel,int sfx_volume, int music_volume, GLTManager *GLTM);
	~TGL();

    bool cycle(std::list<class VirtualController *> *lk,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	bool editor_cycle(GLTManager *GLTM);
	void draw(float zoom, GLTManager *GLTM);

	void reset(void);

	void set_music_volume(int v);
	void set_sfx_volume(int v);

	char *get_map_name(void);
	TGLmap *get_map(void);

	int get_game_result();

	int get_cycle();

	static const char *ship_tiles[N_SHIPS];
	static const char *ship_names[N_SHIPS];
	
protected:
	TTF_Font *m_font;

	int m_cycle;

	int m_game_result;	// 0 : nothing, 1 : level compelted, 2 : failed

	TGLobject *m_ball;
	class TGLobject_ship *m_ship;
	int m_focus_x,m_focus_y;
	char *m_map_file;
	TGLmap *m_map;

	// sound:
	int m_sfx_volume,m_music_volume;
	int m_music_channel;
	
};

#endif

