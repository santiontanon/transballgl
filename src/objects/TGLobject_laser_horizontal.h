#ifndef TGL_OBJECT_LASER_HORIZONTAL
#define TGL_OBJECT_LASER_HORIZONTAL

class TGLobject_laser_horizontal : public TGLobject {
public:
	TGLobject_laser_horizontal(float x,float y,int animation_offset,TGLobject *l1,TGLobject *l2);
	virtual ~TGLobject_laser_horizontal();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);	

private:
	TGLobject *m_l1,*m_l2;
};

#endif