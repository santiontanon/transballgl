#ifndef TGL_OBJECT_SPIKE_LEFT
#define TGL_OBJECT_SPIKE_LEFT

class TGLobject_spike_left : public TGLobject {
public:
	TGLobject_spike_left(float x,float y,int animation_offset);
	virtual ~TGLobject_spike_left();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual bool editor_cycle(TGLmap *map,GLTManager *GLTM);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	float get_start_x(void) {return m_start_x;};

private:
	float m_start_x;
};

#endif