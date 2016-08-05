#ifndef TGL_OBJECT_HA_DIRECTIONALCANNON_RIGHT
#define TGL_OBJECT_HA_DIRECTIONALCANNON_RIGHT

class TGLobject_ha_directionalcannon_right : public TGLobject_enemy {
public:
	TGLobject_ha_directionalcannon_right(float x,float y,int animation_offset);
	virtual ~TGLobject_ha_directionalcannon_right();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif