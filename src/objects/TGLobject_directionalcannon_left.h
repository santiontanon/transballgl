#ifndef TGL_OBJECT_DIRECTIONALCANNON_LEFT
#define TGL_OBJECT_DIRECTIONALCANNON_LEFT

class TGLobject_directionalcannon_left : public TGLobject_enemy {
public:
	TGLobject_directionalcannon_left(float x,float y,int animation_offset);
	virtual ~TGLobject_directionalcannon_left();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif