#ifndef TGL_OBJECT_ACCURA
#define TGL_OBJECT_ACCURA

class TGLobject_ship_accura : public TGLobject_ship {
public:
	TGLobject_ship_accura(float x,float y,int initial_fuel);
	virtual ~TGLobject_ship_accura();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	bool m_thrusting;

	int m_missile_side;
};

#endif