#ifndef TGL_OBJECT_TANK_CANNON
#define TGL_OBJECT_TANK_CANNON

class TGLobject_tank_cannon : public TGLobject_enemy {
public:
	TGLobject_tank_cannon(float x,float y,int animation_offset,TGLobject_enemy *enemy);
	virtual ~TGLobject_tank_cannon();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual bool editor_cycle(TGLmap *map,GLTManager *GLTM);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);


	TGLobject_enemy *get_turret(void) {return m_turret;};

private:
	TGLobject_enemy *m_turret;
};

#endif