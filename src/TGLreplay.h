#ifndef TGL_REPLAY
#define TGL_REPLAY

#include<list>

// one keyframe every 50 standard frames
#define KEYFRAME_PERIOD		50	

class TGLreplay_object_position {
public:
	~TGLreplay_object_position();

	char *m_name;
	float m_x,m_y,m_speed_x,m_speed_y;
	int m_a;
};


class TGLreplay_node {
public:
	TGLreplay_node(void);
	~TGLreplay_node();

	bool m_keyframe;				// If "true", then the "m_objects" array contains a dump of all the objects in the game
    std::list<VirtualController *> m_input;
    std::list<TGLreplay_object_position *> m_objects;
};


class TGLreplay {
public:
	TGLreplay(FILE *fp, GLTManager *GLTM);	// load a replay
	TGLreplay(char *map, TGLmap *embedded_map);	// create a blank replay
	~TGLreplay();

	void add_player(char *player_name,int ship);

	void store_cycle(std::list<VirtualController *> *m_input,std::list<TGLobject *> *m_objects);
	bool execute_cycle(std::list<VirtualController *> *m_input,std::list<TGLobject *> *m_objects,std::list<TGLobject *> *m_todelete,std::list<TGLobject *> *m_toadd);

	bool save(FILE *fp, bool compress, bool embedMap);

	int get_length(void);
	int get_playership(char *player_name);
	char *get_playername(int player);
	char *get_map_name(void);
    TGLmap *get_map(GLTManager *GLTM);
    TGLmap *get_map_copy(GLTManager *GLTM); // returns a copy of the embedded map
    void set_map(TGLmap *map);
    int get_initial_fuel(void);

	void compute_statistics(void);
	int get_fuel_used(void);
	int get_n_shots(void);
	float get_average_speed(void);
	float get_max_speed(void);

	bool read_one_cycle(void);
    bool is_over() {
        return m_is_over;
    }

private:

	static bool replay_ignored_object(TGLobject *o);
	static bool replay_ignored_object(TGLreplay_object_position *o);
	static void save_float(float v,FILE *fp);
	static void save_float(float v,char *str);
	static float load_float(char *str);


	// Version of TRANSBALL with which it was saved:
	char *m_version;

	// Date of the beginning of the game:
	int m_day,m_month,m_year;
	int m_hour,m_minute,m_second;

	// Map:
	char *m_map;
    TGLmap *m_embedded_map;
    char *m_embedded_map_buffer;    // buffer containing the description of the embedded map
	int m_initial_fuel;

	// Who played the game:
	std::list<char *> m_players;

	// ships:
	std::list<int> m_ships;

	// One node per game cycle (thus, the time is m_replay.size()*18 miliseconds)
    std::list<TGLreplay_node *> m_replay;
	int m_length;

	// buffer:
	char *m_buffer;		// replay loaded from a file that is being decoded
	int m_buffer_position;

	// statistics:
	bool m_statistics_computed;
	int m_used_fuel;
	int m_n_shots;
	float m_max_speed,m_average_speed,m_average_speed_tmp;
    
    bool m_is_over;
};

#endif