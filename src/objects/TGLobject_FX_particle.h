#ifndef TGL_OBJECT_FX_PARTICLE
#define TGL_OBJECT_FX_PARTICLE

class TGLobject_FX_particle : public TGLobject {
public:
	TGLobject_FX_particle(float x,float y,int a,float speedx,float speedy,int speeda,bool gravity,float alpha1,float alpha2,float scale1,float scale2,int duration,GLTile *tile);
	TGLobject_FX_particle(float x,float y,int a,float speedx,float speedy,int speeda,bool gravity,
						  float r1,float r2,
						  float g1,float g2,
						  float b1,float b2,
						  float alpha1,float alpha2,float scale1,float scale2,int duration,GLTile *tile);
	virtual ~TGLobject_FX_particle();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

protected:
	bool m_affected_by_gavity;
	float m_speedx,m_speedy;
	int m_speeda;
	float m_alpha1,m_alpha2;
	float m_scale1,m_scale2;
	float m_r1,m_r2;
	float m_g1,m_g2;
	float m_b1,m_b2;
	int m_duration;

};

#endif