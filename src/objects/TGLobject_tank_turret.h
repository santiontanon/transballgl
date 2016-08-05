#ifndef TGL_OBJECT_TANK_TURRET
#define TGL_OBJECT_TANK_TURRET

class TGLobject_tank_turret : public TGLobject_enemy {
public:
	TGLobject_tank_turret(float x,float y,int animation_offset,TGLobject_enemy *tank,int type);
	virtual ~TGLobject_tank_turret();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual bool editor_cycle(TGLmap *map,GLTManager *GLTM);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);


	TGLobject_enemy *get_tank(void) {return m_tank;};

private:
	TGLobject_enemy *m_tank;
	int m_type;		// 0 : grey, 1: red, 2: green
};

#endif