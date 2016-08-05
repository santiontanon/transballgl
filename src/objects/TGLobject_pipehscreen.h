#ifndef TGL_OBJECT_PIPEHSCREEN
#define TGL_OBJECT_PIPEHSCREEN

class TGLobject_pipehscreen : public TGLobject {
public:
	TGLobject_pipehscreen(float x,float y,int animation_offset);
	virtual ~TGLobject_pipehscreen();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif