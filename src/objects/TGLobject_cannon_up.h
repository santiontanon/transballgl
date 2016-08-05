#ifndef TGL_OBJECT_CANNON_UP
#define TGL_OBJECT_CANNON_UP

class TGLobject_cannon_up : public TGLobject_enemy {
public:
	TGLobject_cannon_up(float x,float y,int animation_offset,int type);	// 0 : rock, 1: techno
	virtual ~TGLobject_cannon_up();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	int get_type(void) {return m_type;};

private:
	int m_type;
};

#endif