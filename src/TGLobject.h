#ifndef TGL_OBJECT
#define TGL_OBJECT

#include<list>

// #define PRECISION_FACTOR	256


class TGLobject {
public:
    static void addObjectToMap(char *className, int x, int y, int animation_offset, int p1, int p2, class TGLmap *map);
    static TGLobject *createObjectFromClassName(char *className, int x, int y);
    
	TGLobject(GLTile *t,float x,float y,int animation_offset);
	TGLobject(float x,float y,int animation_offset);
	virtual ~TGLobject();

    virtual void draw(glm::mat4 transform, class GLTManager *GLTM);
    virtual bool cycle(class VirtualController *k,class TGLmap *map,class GLTManager *GLTM,class SFXManager *SFXM,int sfx_volume);
	virtual bool editor_cycle(TGLmap *map,GLTManager *GLTM);

	void exclude_for_collision(TGLobject *o);
	void reconsider_for_collision(TGLobject *o);
	bool check_collision(TGLobject *o);

	bool collision(TGLobject *o);

	void set_controller(int c);
	int get_controller(void);

	GLTile *get_last_tile(void);
	GLTile *get_last_mask(void);

	float get_x(void);
	float get_y(void);
	int get_angle(void);
	float get_scale(void);

	void set_x(float x);
	void set_y(float y);

	int get_state(void);
	void set_state(int s);

	int get_cycle(void);
	void set_cycle(int s);

	int get_animation_offset(void);
	void set_animation_offset(int o);

	virtual bool is_a(const char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);
	
	SDL_Surface *get_collision_cache(float angle,float scale,SDL_Surface *source);
	void set_collision_cache(SDL_Surface *s, float angle, float scale, SDL_Surface *source);

protected:
	int m_controller;	// identifies which controller from the list is it taking input from
	int m_state;
	int m_cycle;
	float m_x,m_y;
	float m_angle;
	float m_scale;
	int m_animation_offset;
	GLTile *m_last_tile;
	GLTile *m_last_mask;
	
	SDL_Surface *m_collision_cache;	// this stores the result of rotozooming the object to the current angle and scale, 
									// to prevent computing it again and again in the same cycle when multiple objects need to test for collision
	float m_collision_cache_angle, m_collision_cache_scale;
	SDL_Surface *m_collision_cache_source;

    std::list<TGLobject *> m_excluded_from_collision;
//	GLTileCache *m_transformation_cache[360];		// Note: this assumes that the "m_scale" parameter wont change!!
};

#endif