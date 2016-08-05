#ifndef TGL_OBJECT_C_HARPOON
#define TGL_OBJECT_C_HARPOON

#include<list>

#define CHAIN_TENSION		1.0f
#define CHAIN_LINK_LENGTH	4
#define GRABBER_WEIGHT_WO_BALL	0.2f
#define GRABBER_WEIGHT_W_BALL	0.8f

#define INITIAL_CHAIN_SIZE	4

#define MIN_CHAIN_SIZE	2
#define MAX_CHAIN_SIZE	16


class TGLobject_ship_c_harpoon : public TGLobject_ship {
public:
	TGLobject_ship_c_harpoon(float x,float y,int initial_fuel);
	virtual ~TGLobject_ship_c_harpoon();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual bool cycle(VirtualController *k,class TGLmap *map,GLTManager *GLTM,SFXManager *SFXM,int sfx_volume);
	virtual void draw(glm::mat4 transform, class GLTManager *GLTM);

	void chain_cycle(void);

	void set_next_join(float x,float y);

	float get_chain_length(void);

private:
	bool m_thrusting;

    std::list<TGLobject_ship_chain *> m_chain;
	TGLobject_ship_grabber *m_grabber;

	float m_next_join_x,m_next_join_y;

	float m_chain_length;
};

#endif