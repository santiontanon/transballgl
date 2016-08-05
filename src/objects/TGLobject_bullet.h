#ifndef TGL_OBJECT_BULLET
#define TGL_OBJECT_BULLET

class TGLobject_bullet : public TGLobject {
public:
	TGLobject_bullet(float x,float y,int animation_offset,int angle,float speed,int power,GLTile *tile,TGLobject *ship);
	virtual ~TGLobject_bullet();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);
	
	void hit(void);

protected:
	float m_speed;
	int m_power;
	GLTile *m_tile;
};

class TGLobject_soft_bullet : public TGLobject_bullet {
public:
	TGLobject_soft_bullet(float x,float y,int animation_offset,int angle,float speed,int power,GLTile *tile,TGLobject *ship);

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);
};

#endif