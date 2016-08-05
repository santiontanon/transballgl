#ifndef TGL_OBJECT_FAST_DIRECTIONALCANNON_UP
#define TGL_OBJECT_FAST_DIRECTIONALCANNON_UP

class TGLobject_fast_directionalcannon_up : public TGLobject_enemy {
public:
	TGLobject_fast_directionalcannon_up(float x,float y,int animation_offset);
	virtual ~TGLobject_fast_directionalcannon_up();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif