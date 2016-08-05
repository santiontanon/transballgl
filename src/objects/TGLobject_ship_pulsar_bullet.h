#ifndef TGL_OBJECT_PULSAR_BULLET
#define TGL_OBJECT_PULSAR_BULLET

class TGLobject_ship_pulsar_bullet : public TGLobject {
public:
	TGLobject_ship_pulsar_bullet(float x,float y, float angle, TGLobject *ship);
	virtual ~TGLobject_ship_pulsar_bullet();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

private:
	float m_speed;
	float m_radius;

	int n_points;
	int timmer;
	double *m_point_x,*m_point_y;
	double *m_point_alpha;
	double *m_point_width;
	
	GLTile *pixel_tile;
	TGLobject *pixel_object;
};

#endif