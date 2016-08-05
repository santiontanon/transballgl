#ifndef TGL_OBJECT_TANK
#define TGL_OBJECT_TANK

class TGLobject_tank : public TGLobject_enemy {
public:
	TGLobject_tank(float x,float y,int animation_offset,int type);
	virtual ~TGLobject_tank();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void hit(int power);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	int get_type(void) {return m_type;};

private:
	int m_type;
};

#endif