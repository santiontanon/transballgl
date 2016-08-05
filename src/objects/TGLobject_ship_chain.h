#ifndef TGL_OBJECT_SHIP_CHAIN
#define TGL_OBJECT_SHIP_CHAIN

class TGLobject_ship_chain : public TGLobject {
public:
	TGLobject_ship_chain(float x,float y);
	virtual ~TGLobject_ship_chain();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	void chain_cycle(void);

	void set_previous(TGLobject *o);
	void set_next(TGLobject *o);

	void set_previous_join(float x,float y);
	void set_next_join(float x,float y);

private:
	TGLobject *m_previous,*m_next;

	float m_previous_join_x,m_previous_join_y;
	float m_next_join_x,m_next_join_y;

};

#endif