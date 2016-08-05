#ifndef TGL_OBJECT_FX_EXPLOSION1
#define TGL_OBJECT_FX_EXPLOSION1

class TGLobject_FX_explosion1 : public TGLobject {
public:
	TGLobject_FX_explosion1(float x,float y,float maxsize,int duration);
	virtual ~TGLobject_FX_explosion1();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

protected:
	float m_maxsize;
	int m_duration;
};

#endif