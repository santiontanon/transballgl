#ifndef TGL_OBJECT_BUTTON
#define TGL_OBJECT_BUTTON

class TGLobject_button : public TGLobject {
public:
	TGLobject_button(float x,float y,int animation_offset,int event,int type);
	virtual ~TGLobject_button();

	void ball_hit(void);

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	int get_type(void) {return m_type;};
	int get_event(void) {return m_event;};
	void set_event(int event) {m_event=event;};

private:
	int m_type;
	int m_event;
};

#endif