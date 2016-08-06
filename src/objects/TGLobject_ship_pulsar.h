#ifndef TGL_OBJECT_PULSAR
#define TGL_OBJECT_PULSAR

class TGLobject_ship_pulsar : public TGLobject_ship {
public:
	TGLobject_ship_pulsar(float x,float y,int initial_fuel);

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	bool m_thrusting;
};

#endif