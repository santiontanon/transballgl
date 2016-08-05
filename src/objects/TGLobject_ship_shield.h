#ifndef TGL_OBJECT_SHIELD
#define TGL_OBJECT_SHIELD

class TGLobject_ship_shield : public TGLobject {
public:
	TGLobject_ship_shield(float x,float y);
	virtual ~TGLobject_ship_shield();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	void generate_shield_textures(void);

	void set_x(float x);
	void set_y(float y);
	void set_status(int status);
	int get_status(void);

private:
	int m_shield_status;
	int m_shield_frame;
	int m_n_shield_frames;	
	GLTile *m_shield_frames;
};

#endif