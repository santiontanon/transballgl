#ifndef TGL_OBJECT_RADAR
#define TGL_OBJECT_RADAR

class TGLobject_radar : public TGLobject {
public:
	TGLobject_radar(float x,float y,int animation_offset);
	virtual ~TGLobject_radar();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
};

#endif