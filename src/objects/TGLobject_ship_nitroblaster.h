#ifndef TGL_OBJECT_NITROBLASTER
#define TGL_OBJECT_NITROBLASTER

class TGLobject_ship_nitroblaster : public TGLobject_ship {
public:
	TGLobject_ship_nitroblaster(float x,float y,int initial_fuel);
	virtual ~TGLobject_ship_nitroblaster();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	bool m_thrusting,m_nitrothrusting;
};

#endif