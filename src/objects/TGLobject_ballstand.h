#ifndef TGL_OBJECT_BALL_STAND
#define TGL_OBJECT_BALL_STAND

class TGLobject_ballstand : public TGLobject {
public:
	TGLobject_ballstand(float x,float y,int animation_offset);
	virtual ~TGLobject_ballstand();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif