#ifndef TGL_OBJECT_LASER_RIGHT
#define TGL_OBJECT_LASER_RIGHT

class TGLobject_laser_right : public TGLobject_enemy {
public:
	TGLobject_laser_right(float x,float y,int animation_offset);
	virtual ~TGLobject_laser_right();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif