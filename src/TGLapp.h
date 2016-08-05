#ifndef _TGL_APP
#define _TGL_APP

#include<list>
#include<vector>

#include "TGL.h"

#include "TGLconstants.h"

#define TGL_SCROLLWHEEL_BUTTON  10

#define TGL_PLAY_MAPS_FOLDER    "maps"
#define TGL_EDIT_MAPS_FOLDER    "mymaps"

class TextNode {
public:
	char *m_text;
	int m_time;

	TextNode() {m_text = 0; m_time = 0;};
	~TextNode() {if (m_text!=0) delete m_text;};
};


class TGLapp {
public:    
	TGLapp();
	~TGLapp();

	bool cycle(class KEYBOARDSTATE *k);
	void draw(int SCREEN_X,int SCREEN_Y);

	void save_playerprofile(void);
	void load_playerprofile(char *name);

	void MouseClick(int mx, int my, int button);
    void MouseWheel(int mx, int my);

    void clearOpenGLState();
    void resetOpenGLState();    
    
	static void string_editor_cycle(char *editing_text,unsigned int *editing_position,unsigned int max_length,KEYBOARDSTATE *k);

	class PlayerProfile *get_player_profile(void) {return m_player_profile;};


protected:
	int playerprofile_cycle(KEYBOARDSTATE *k);
	int intro_cycle(KEYBOARDSTATE *k);
	int levelpackscreen_cycle(KEYBOARDSTATE *k);
	int levelpackbrowser_cycle(KEYBOARDSTATE *k);
	int mainmenu_cycle(KEYBOARDSTATE *k);
	int configure_cycle(KEYBOARDSTATE *k);
	int game_cycle(KEYBOARDSTATE *k);
	int pregame_cycle(KEYBOARDSTATE *k);
	int postgame_cycle(KEYBOARDSTATE *k);
	int savereplay_cycle(KEYBOARDSTATE *k);
	int replaybrowser_cycle(KEYBOARDSTATE *k);
	int highscores_cycle(KEYBOARDSTATE *k);
	int highscores_times_cycle(KEYBOARDSTATE *k);
	int editor_cycle(KEYBOARDSTATE *k);
	int mapeditor_cycle(KEYBOARDSTATE *k);
	int loadlevelpack_cycle(KEYBOARDSTATE *k);
	int loadmap_cycle(KEYBOARDSTATE *k);
	int savemap_cycle(KEYBOARDSTATE *k);

	void playerprofile_draw(void);
	void intro_draw(void);
	void levelpackscreen_draw(void);
	void levelpackbrowser_draw(void);
	void mainmenu_draw(void);
	void configure_draw(void);
	void game_draw(void);
	void pregame_draw(void);
	void postgame_draw(void);
	void savereplay_draw(void);
	void replaybrowser_draw(void);
	void highscores_draw(void);
	void highscores_times_draw(void);
	void editor_draw(void);
	void mapeditor_draw(void);
	void loadlevelpack_draw(void);
	void loadmap_draw(void);
	void savemap_draw(void);

	// Fade in/out effects:
	void fade_in_alpha(float f);
	void fade_in_squares(float f,float size);
	void fade_in_triangles(float f,float size);

	class SFXManager *m_SFXM;
	class GLTManager *m_GLTM;

	TTF_Font *m_font32,*m_font16,*m_ifont32;

	int m_mouse_x,m_mouse_y,m_mouse_click;
    std::list<int> m_mouse_click_x;
	std::list<int> m_mouse_click_y;
	std::list<int> m_mouse_click_button;

	int m_state,m_previous_state;
	int m_state_cycle;
	int m_state_fading;
	int m_state_fading_cycle;
	int m_state_selection;
	class TGL *m_game;

    std::list<class VirtualController *> m_lvc;

	// configuration:
	PlayerProfile *m_player_profile;

	// Game state:
	int m_game_state;
	int m_game_state_cycle;
	int m_game_fade_effect;
	int m_game_replay_mode;	//  1 : saving replay, 2 : replaying replay
	class TGLreplay *m_game_replay;
	class LevelPack *m_current_levelpack;
	int m_selected_level;
	int m_selected_ship;
	int m_game_previous_state;
	bool m_game_reinit_previous_state;

	// Post game:
	int m_ship_unlocked;
    int m_next_ship_to_unlock;

	// Main menu:
	int m_mm_demo_state;
	int m_mm_demo_timmer;
	TGL *m_mm_game;
	TGLreplay *m_mm_replay;

	// Save Replay
	class TGLTextInputFrame *m_replay_name_inputframe;
	class TGLInterfaceElement *m_replay_save_button;
	TGLInterfaceElement *m_sr_replay_uparrow;
	TGLInterfaceElement *m_sr_replay_downarrow;
    std::vector<char *> m_sr_replay_fullnames;
    std::vector<char *> m_sr_replay_names;
    std::vector<char *> m_sr_replay_info;
	int m_sr_first_replay;

	// Replay Browser:
	TGLInterfaceElement *m_replay_rename_button;
	TGLInterfaceElement *m_replay_play_button;
	TGLInterfaceElement *m_replay_delete_button;
	int m_rb_mouse_over_replay;
	int m_rb_replay_selected;

	// LevelPack screen:
	int m_lp_music_channel;
	int m_lp_first_level;
	TGLInterfaceElement *m_lp_level_uparrow;
	TGLInterfaceElement *m_lp_level_downarrow;
	TGLInterfaceElement *m_lp_level_name[3];
	TGLInterfaceElement *m_lp_level_time[3];
	TGLInterfaceElement *m_lp_level_points[3];
	TGLInterfaceElement *m_lp_play_buttons[3];
	TGLInterfaceElement *m_lp_viewreplay_buttons[3];
	TGLInterfaceElement *m_lp_ship_leftarrow;
	TGLInterfaceElement *m_lp_ship_rightarrow;
	TGL *m_lp_tutorial_game;
	TGLreplay *m_lp_tutorial_replay;
    std::list<TextNode *> m_lp_tutorial_replay_text;
	int m_lp_replay_mode;
	int m_lp_replay_timmer;

	// Levelpack Browser
	TGLInterfaceElement *m_lpb_select_button;
	TGLInterfaceElement *m_lpb_lp_uparrow;
	TGLInterfaceElement *m_lpb_lp_downarrow;
	std::list<char *> m_lpb_lp_fullnames;
	std::list<char *> m_lpb_lp_names;
	std::list<char *> m_lpb_lp_info;
	std::list<char *> m_lpb_lp_longinfo;
	int m_lpb_first_lp;
	int m_lpb_mouse_over_lp;
	int m_lpb_lp_selected;

	// configure:
	char m_player_data_path[256];	// This will be empty for windows (data will be saved directly int he aplication folder), 
									// but set to "~/.transballGL/" in Linux/Mac
	int m_configure_key_to_change;
	TGLInterfaceElement *m_configure_fullscreen;
	TGLInterfaceElement *m_configure_window;
    TGLInterfaceElement *m_configure_zoom;
	TGLInterfaceElement *m_configure_music_volume;
	TGLInterfaceElement *m_configure_sfx_volume;
    int m_configure_previous_state;
    bool m_configure_refresh_gui;

	// Player profile selection:
	TGLInterfaceElement *m_profile_select_button;
	TGLInterfaceElement *m_profile_create_button;
	TGLTextInputFrame *m_profile_name_inputframe;
	int m_profile_mouse_over_profile;
	int m_profile_profile_selected;
	TGLInterfaceElement *m_profile_profile_uparrow;
	TGLInterfaceElement *m_profile_profile_downarrow;
	std::list<char *> m_profile_profile_fullnames;
	std::list<char *> m_profile_profile_names;
	std::list<char *> m_profile_profile_info;
	int m_profile_first_profile;

	// Highscores:
	std::vector<char *> m_highscores_names;
	std::vector<int> m_highscores_points;
	std::vector<int> m_highscores_time;
	int m_highscores_first_name;
	char *m_highscores_level_pack;
	std::vector<char *> m_highscores_lp_ids;
	std::vector<char *> m_highscores_lp_names;
    std::vector< std::vector<char *> *> m_highscores_times_names[N_SHIPS+1];	// One list per ship type, plus a common one
    std::vector< std::vector<int> *> m_highscores_times_time[N_SHIPS+1];	// One list per ship type, plus a common one
	int m_highscores_first_ship;
	int m_highscores_first_level;

	// Editor:
	LevelPack *m_editor_levelpack;
	class LevelPack_Level *m_editor_level;
	class TGLmap *m_editor_level_editing;
	int m_editor_mode;
	int m_editor_focus_x,m_editor_focus_y;
	int m_editor_zoom;
	float m_editor_real_zoom,m_editor_current_zoom;	
	int m_editor_selected_tile,m_editor_selected_object;
	std::vector<GLTile *> m_editor_tiles;
	std::vector<GLTile *> m_editor_object_tiles;
	std::vector<GLTile *> m_editor_smart_tiles;
	std::vector<GLTile *> m_editor_smart_tile_palette;
	int m_editor_insert_x,m_editor_insert_y;
    int m_editor_last_smart_tile_x,m_editor_last_smart_tile_y;

	// Load/Save maps:
	TGLInterfaceElement *m_mb_select_button;
	TGLInterfaceElement *m_mb_m_uparrow;
	TGLInterfaceElement *m_mb_m_downarrow;
	std::vector<char *> m_mb_folders_names;
	std::vector<char *> m_mb_m_names;
	char m_mb_current_path[256];
	bool m_mb_recheckfiles;
	int m_mb_first_m;
	int m_mb_mouse_over_m;
	int m_mb_m_selected;
	class TGLobject *m_mb_object_under_pointer;
    int m_save_map_type;    // 0: save map, 1: save PNG


};

#endif

