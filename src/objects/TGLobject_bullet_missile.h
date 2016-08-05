#ifndef TGL_OBJECT_BULLET_MISSILE
#define TGL_OBJECT_BULLET_MISSILE

class TGLobject_bullet_missile : public TGLobject_bullet {
public:
	TGLobject_bullet_missile(float x,float y,int animation_offset,int angle,float speed,int power,GLTile *tile,GLTile *tile2,TGLobject *ship);

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	GLTile *m_tile2;
};

#endif