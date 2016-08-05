#ifndef TGL_OBJECT_GRABBER
#define TGL_OBJECT_GRABBER

class TGLobject_ship_grabber : public TGLobject {
public:
	TGLobject_ship_grabber(float x,float y);
	virtual ~TGLobject_ship_grabber();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	void chain_cycle(void);
	void chain_cycle_ball(void);

	void set_chain(TGLobject_ship_chain *chain);
	void set_ball(TGLobject_ball *ball);
	void set_ship(class TGLobject_ship_c_harpoon *ship);

	void set_previous_join(float x,float y);

	TGLobject_ball *grab_ball(TGLmap *map);
	bool release_ball(void);

private:
	TGLobject_ship_chain *m_chain;
	TGLobject_ball *m_ball;
	TGLobject_ship_c_harpoon *m_ship;

	float m_previous_join_x,m_previous_join_y;
	float m_precission_angle;

	float m_speed_x,m_speed_y,m_speed_a;
};

#endif