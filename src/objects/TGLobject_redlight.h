#ifndef TGL_OBJECT_REDLIGHT
#define TGL_OBJECT_REDLIGHT

class TGLobject_redlight : public TGLobject {
public:
	TGLobject_redlight(float x,float y,int animation_offset,int type);	// type = 0 is regular, type = 1 is snow
	virtual ~TGLobject_redlight();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:

	int m_type;
};

#endif