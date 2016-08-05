#ifndef TGL_OBJECT_GYRUS
#define TGL_OBJECT_GYRUS

class TGLobject_ship_gyrus : public TGLobject_ship {
public:
	TGLobject_ship_gyrus(float x,float y,int initial_fuel);
	virtual ~TGLobject_ship_gyrus();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	bool m_thrusting;
	int m_cannon_angle;
	int m_cannon_rotation_timmer;
	GLTile *m_cannon;
};

#endif