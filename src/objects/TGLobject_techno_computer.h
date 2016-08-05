#ifndef TGL_OBJECT_TECHNO_COMPUTER
#define TGL_OBJECT_TECHNO_COMPUTER

class TGLobject_techno_computer : public TGLobject {
public:
	TGLobject_techno_computer(float x,float y,int animation_offset);
	virtual ~TGLobject_techno_computer();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif