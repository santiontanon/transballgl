#ifndef _GLTILE_MANAGER
#define _GLTILE_MANAGER

#include <vector>

class GLTManagerImageNode {
public:
	GLTManagerImageNode(SDL_Surface *a_surface, class Symbol *m_fileName);
	~GLTManagerImageNode();

	struct SDL_Surface *m_surface;
	Symbol *m_fileName;
    GLuint m_tex;
    float m_tx, m_ty;
};


class GLTManagerNode {
public:
	GLTManagerNode(class GLTile *m_tile, Symbol *a_name, GLTManagerImageNode *a_source, int a_x, int a_y, int a_dx, int a_dy);
	~GLTManagerNode();

	class GLTile *m_tile;
	Symbol *m_name;
	GLTManagerImageNode *m_sourceImage;
	int m_x, m_y, m_dx, m_dy;
};


class GLTManager {
public:
	GLTManager();
	~GLTManager();

	void clear(void);
	void clearOpenGLState();

	// note: these functions are not supposed to be fast, so minimizing the number of times
	// 		 they are called is recommended.
	GLTile *get(const char *name);
	GLTile *get(Symbol *name);
	GLTile *get(const char *name, int x, int y, int dx, int dy);
	GLTile *get(Symbol *name, int x, int y, int dx, int dy);

	SDL_Surface *getImage(const char *name);
	SDL_Surface *getImage(Symbol *name);

	char *get_name(GLTile *tile);

protected:

	std::vector<GLTManagerNode *> *m_hash;
	std::vector<GLTManagerImageNode *> *m_image_hash;
};

#endif



