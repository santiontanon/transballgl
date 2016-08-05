#ifndef TGL_LEVEL_PACK
#define TGL_LEVEL_PACK

#include<vector>

class LevelPack_Level {
public:
	LevelPack_Level(class LevelPack *lp,class GLTManager *GLTM);
	LevelPack_Level(LevelPack *lp,FILE *fp, const char *folder, GLTManager *GLTM);
	LevelPack_Level(LevelPack *lp,class XMLNode *node, const char *folder, GLTManager *GLTM);
	~LevelPack_Level();

	void load(XMLNode *node, const char *folder, GLTManager *GLTM);
	void save(FILE *fp, const char *folder, GLTManager *GLTM);
	void setName(char* name);
	void setMapName(char* name);

	char *m_map;
	char *m_name;
	char *m_description;
	int m_initial_fuel;
	int m_points;
	class TGLmap *m_map_data;
	LevelPack *m_levelpack;
};


class LevelPack {
public:
	LevelPack();
	LevelPack(FILE *fp, const char *folder, GLTManager *GLTM);
	~LevelPack();

	void load(XMLNode *node, const char *folder, GLTManager *GLTM);
	void save(FILE *fp, const char *folder, GLTManager *GLTM);
	int get_points(int level);

	int getLevelPosition(LevelPack_Level *level);
	int getNLevels();
	LevelPack_Level *getLevel(int n);
	LevelPack_Level *getLevel(char *name);
	LevelPack_Level *getLevelByMap(char *map);

	char *m_id;
	char *m_name;
	char *m_description;
	char *m_creator_name;
	char *m_creator_nickname;
    std::vector<LevelPack_Level *> m_levels;
};

#endif