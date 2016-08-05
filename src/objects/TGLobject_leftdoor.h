#ifndef TGL_OBJECT_LEFTDOOR
#define TGL_OBJECT_LEFTDOOR

class TGLobject_leftdoor : public TGLobject {
public:
	TGLobject_leftdoor(float x,float y,int animation_offset,int state,int action);
	virtual ~TGLobject_leftdoor();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual bool editor_cycle(TGLmap *map,GLTManager *GLTM);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	void action(int action);
	int get_action(void) {return m_action;};
	void set_action(int a) {m_action=a;};

	float get_start_x(void) {return m_start_x;};

private:
	float m_start_x;
	int m_action;
	int m_local_cycle;
};

#endif