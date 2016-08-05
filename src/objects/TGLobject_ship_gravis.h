#ifndef TGL_OBJECT_GRAVIS
#define TGL_OBJECT_GRAVIS

class TGLobject_ship_gravis : public TGLobject_ship {
public:
	TGLobject_ship_gravis(float x,float y,int initial_fuel);
	virtual ~TGLobject_ship_gravis();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	bool m_thrusting;

	bool m_attracting;
	int m_attracting_cycle;
	int m_attracting_channel;
};

#endif