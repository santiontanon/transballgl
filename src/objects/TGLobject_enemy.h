#ifndef TGL_OBJECT_ENEMY
#define TGL_OBJECT_ENEMY

class TGLobject_enemy : public TGLobject {
public:
	TGLobject_enemy(float x,float y,int animation_offset);
	virtual ~TGLobject_enemy();

	virtual bool is_a(char *n);
	virtual bool is_a(Symbol *n);
	virtual const char *get_class(void);

	virtual void hit(int power);

	int get_hitpoints(void);
	void set_hitpoints(int hp);

protected:
	int m_hitpoints;
	bool m_hit;
};

#endif