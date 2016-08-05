#ifndef TGL_OBJECT_DFLECTER
#define TGL_OBJECT_DFLECTER

class TGLobject_ship_dflecter : public TGLobject_ship {
public:
	TGLobject_ship_dflecter(float x,float y,int initial_fuel);
	virtual ~TGLobject_ship_dflecter();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	void generate_shield_textures(void);

private:
	bool m_thrusting;

	int m_collision_counter;	// Counts how many cycles in a row the ship has collided
								// Since some objects might destroy themselves in the shield
								// this value has to be at least 2 for the ship to be distroyed

	TGLobject_ship_shield *m_shield;
};

#endif