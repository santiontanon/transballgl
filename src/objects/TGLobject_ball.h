#ifndef TGL_OBJECT_BALL
#define TGL_OBJECT_BALL

class TGLobject_ball : public TGLobject {
public:
	TGLobject_ball(float x,float y,int animation_offset);
	virtual ~TGLobject_ball();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	bool attractor(void);
	void capture(int m_type);	// m_type = 1 -> blue, 2 -> red, 3 -> purple, 4 -> green, 5 -> yellow, 6 -> darkblue

	float get_speed_x(void);
	float get_speed_y(void);
	void set_speed_x(float speed_x);
	void set_speed_y(float speed_y);

private:
	int m_attractor_timmer;
	float m_speed_x,m_speed_y;
};

#endif