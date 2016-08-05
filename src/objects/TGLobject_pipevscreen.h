#ifndef TGL_OBJECT_PIPEVSCREEN
#define TGL_OBJECT_PIPEVSCREEN

class TGLobject_pipevscreen : public TGLobject {
public:
	TGLobject_pipevscreen(float x,float y,int animation_offset);
	virtual ~TGLobject_pipevscreen();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif