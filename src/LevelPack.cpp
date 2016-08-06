#ifdef _WIN32
#include <windows.h>
#include "direct.h"
#else
#include "sys/stat.h"
#include "sys/types.h"
#endif

#include "stdlib.h"
#include "string.h"
#include "stdio.h"

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif
#include "SDL.h"

#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#include "SDL/SDL_ttf.h"
#else
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif

#include "SDL_rotozoom.h"


#include "Symbol.h"
#include "XMLparser.h"
#include "LevelPack.h"

#include "GLTManager.h"
#include "SFXManager.h"
#include "TGLmap.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
extern const char *persistent_data_folder;
#endif


LevelPack_Level::LevelPack_Level(LevelPack *lp,GLTManager *GLTM)
{
	m_map=0;
	m_name=0;
	m_description=0;
	m_initial_fuel=100;
	m_points=1;
	m_levelpack=lp;

	m_map_data = new TGLmap(GLTM);
} /* LevelPack_Level::LevelPack_Level */ 


LevelPack_Level::LevelPack_Level(LevelPack *lp,FILE *fp, const char *folder, GLTManager *GLTM)
{

	m_map=0;
	m_name=0;
	m_description=0;
	m_initial_fuel=100;
	m_points=1;
	m_levelpack=lp;

	// Load it from a file:
	XMLNode *node;
	node=XMLNode::from_file(fp);
	load(node, folder, GLTM);
	delete node;

} /* LevelPack_Level::LevelPack_Level */ 


LevelPack_Level::LevelPack_Level(LevelPack *lp,XMLNode *node, const char *folder, GLTManager *GLTM)
{

	m_map=0;
	m_name=0;
	m_description=0;
	m_initial_fuel=100;
	m_points=1;
	m_levelpack=lp;

	load(node,folder, GLTM);

} /* LevelPack_Level::LevelPack_Level */ 


void LevelPack_Level::load(XMLNode *node, const char *folder, GLTManager *GLTM)
{
	std::vector<XMLNode *> *children;

	if (!node->get_type()->cmp("level")) return;
	children=node->get_children();
    for(XMLNode *n:*children) {
		if (n->get_type()->cmp("map")) {
			m_map=new char[strlen(n->get_value())+1];
			strcpy(m_map,n->get_value());
		} // if 
		if (n->get_type()->cmp("name")) {
			m_name=new char[strlen(n->get_value())+1];
			strcpy(m_name,n->get_value());
		} // if 
		if (n->get_type()->cmp("description")) {
			if (n->get_value()!=0) {
				m_description=new char[strlen(n->get_value())+1];
				strcpy(m_description,n->get_value());
			} else {
				m_description=new char[1];
				strcpy(m_description,"");
			} // if
		} // if 
		if (n->get_type()->cmp("initial-fuel")) {
			m_initial_fuel=atoi(n->get_value());
		} // if 
		if (n->get_type()->cmp("points")) {
			m_points=atoi(n->get_value());
		} // if 
	} // while 

	{
		char map_file[512];
		FILE *fp;

		sprintf(map_file,"%s/%s/%s",folder,m_levelpack->m_id,m_map);
		fp=fopen(map_file,"r+");
		if (fp!=0) {
			m_map_data = new TGLmap(fp,GLTM);
			fclose(fp);
		} else {
			m_map_data = new TGLmap(GLTM);
		} // if
	}

} /* LevelPack_Level::load */ 


LevelPack_Level::~LevelPack_Level()
{
	if (m_map!=0) delete []m_map;
	m_map=0;
	if (m_name!=0) delete []m_name;
	m_name=0;
	if (m_description!=0) delete []m_description;
	m_description=0;

	if (m_map_data!=0) delete m_map_data;
	m_map_data=0;
    
} /* LevelPack_Level::~LevelPack_Level */ 


void LevelPack_Level::setMapName(char* name)
{
	if (m_map!=0) delete m_map;
	m_map = new char[strlen(name)+1];
	strcpy(m_map,name);
} /* LevelPack_Level::setMapName */ 


void LevelPack_Level::setName(char* name)
{
	if (m_name!=0) delete m_name;
	m_name = new char[strlen(name)+1];
	strcpy(m_name,name);
} /* LevelPack_Level::setName */ 




LevelPack::LevelPack()
{
	m_id=0;
	m_name=0;
	m_description=0;
	m_creator_name=0;
	m_creator_nickname=0;

} /* LevelPack::LevelPack */ 


LevelPack::LevelPack(FILE *fp, const char *folder, GLTManager *GLTM)
{
	m_id=0;
	m_name=0;
	m_description=0;
	m_creator_name=0;
	m_creator_nickname=0;

	// Load it from a file:
	XMLNode *node;
	node=XMLNode::from_file(fp);
	load(node, folder, GLTM);
	delete node;

} /* LevelPack::LevelPack */ 


void LevelPack::load(XMLNode *node, const char *folder, GLTManager *GLTM)
{
	std::vector<XMLNode *> *children;

	if (!node->get_type()->cmp("levelpack")) return;
	children=node->get_children();
    for(XMLNode *n:*children) {
		if (n->get_type()->cmp("id")) {
			m_id=new char[strlen(n->get_value())+1];
			strcpy(m_id,n->get_value());
		} // if 
		if (n->get_type()->cmp("name")) {
			m_name=new char[strlen(n->get_value())+1];
			strcpy(m_name,n->get_value());
		} // if 
		if (n->get_type()->cmp("description")) {
			m_description=new char[strlen(n->get_value())+1];
			strcpy(m_description,n->get_value());
		} // if 
		if (n->get_type()->cmp("creator")) {
			std::vector<XMLNode *> *children2;
			
			children2=n->get_children();
            for(XMLNode *n2:*children2) {
				if (n2->get_type()->cmp("name")) {
					m_creator_name=new char[strlen(n2->get_value())+1];
					strcpy(m_creator_name,n2->get_value());
				} // if 
				if (n2->get_type()->cmp("nickname")) {
					m_creator_nickname=new char[strlen(n2->get_value())+1];
					strcpy(m_creator_nickname,n2->get_value());
				} // if 
			} // while
		} // if
		if (n->get_type()->cmp("levels")) {
			std::vector<XMLNode *> *children2;

			children2=n->get_children();
            for(XMLNode *n2:*children2) {
				if (n2->get_type()->cmp("level")) {
					m_levels.push_back(new LevelPack_Level(this,n2,folder,GLTM));
				} // if 
			} // while
		} // if
	} // while 
} /* LevelPack::load */


LevelPack::~LevelPack()
{
	if (m_id!=0) delete []m_id;
	m_id=0;
	if (m_name!=0) delete []m_name;
	m_name=0;
	if (m_description!=0) delete []m_description;
	m_description=0;
	if (m_creator_name!=0) delete []m_creator_name;
	m_creator_name=0;
	if (m_creator_nickname!=0) delete []m_creator_nickname;
	m_creator_nickname=0;
    
    for(LevelPack_Level *level:m_levels) delete level;
    m_levels.clear();    
} /* LevelPack::~LevelPack */ 


int LevelPack::get_points(int level)
{
	if (level>=0 && level<m_levels.size()) return m_levels[level]->m_points;

	return 0;
} /* LevelPack::get_points */ 


int LevelPack::getLevelPosition(LevelPack_Level *level)
{
    int idx = 0;
    for(LevelPack_Level *l:m_levels) {
        if (l==level) return idx;
        idx++;
    }
    return -1;
} /* LevelPack::getLevelPosition */ 


int LevelPack::getNLevels()
{
	return (int)m_levels.size();
} /* LevelPack::getNLevels */ 


LevelPack_Level *LevelPack::getLevel(int n)
{
	return m_levels[n];
} /* LevelPack::getLevel */ 


LevelPack_Level *LevelPack::getLevel(char *name)
{
    for(LevelPack_Level *l:m_levels) {
		if (strcmp(l->m_name,name)==0) return l;
	} // while

	return 0;
} /* LevelPack::getLevel */ 


LevelPack_Level *LevelPack::getLevelByMap(char *map)
{
    for(LevelPack_Level *l:m_levels) {
		if (strcmp(l->m_map,map)==0) return l;
	} // while
	
	return 0;
} /* LevelPack::getLevelByMap */ 


void LevelPack::save(FILE *fp, const char *folder, GLTManager *GLTM)
{
	fprintf(fp,"<levelpack>\n");
	fprintf(fp,"  <id>%s</id>\n",m_id);
	fprintf(fp,"  <name>%s</name>\n",m_name);
	fprintf(fp,"  <description>%s</description>\n",m_description);
	fprintf(fp,"  <creator>\n");
    if (m_creator_name!=0) fprintf(fp,"    <name>%s</name>\n",m_creator_name);
    if (m_creator_nickname!=0) fprintf(fp,"    <nickname>%s</nickname>\n",m_creator_nickname);
	fprintf(fp,"  </creator>\n");
	fprintf(fp,"  <levels>\n");
	
    for(LevelPack_Level *l:m_levels) l->save(fp, folder, GLTM);

	fprintf(fp,"  </levels>\n");
	fprintf(fp,"</levelpack>\n");
} /* LevelPack::save */ 


/*
        <level>
            <map>sa-level1.tgl</map>
            <name>Wellcome back!</name>
            <description>This level is just to warm up again...</description>
            <points>2</points>
        </level>
*/

void LevelPack_Level::save(FILE *fp, const char *folder, GLTManager *GLTM)
{
	fprintf(fp,"    <level>\n");
	if (m_map!=0) fprintf(fp,"      <map>%s</map>\n",m_map);
	if (m_name!=0) fprintf(fp,"      <name>%s</name>\n",m_name);
	if (m_description!=0) fprintf(fp,"      <description>%s</description>\n",m_description);
	fprintf(fp,"      <points>%i</points>\n",m_points);
    if (m_initial_fuel!=100) fprintf(fp,"      <initial-fuel>%i</initial-fuel>\n",m_initial_fuel);
	fprintf(fp,"    </level>\n");

	{
		FILE *fp;
		char map_file[256];
		sprintf(map_file,"%s/%s/%s",folder,m_levelpack->m_id,m_map);
		fp=fopen(map_file,"w+");
		if (fp==0) {
			// assume the folder does not exist:
			char folder_file[256];
			sprintf(folder_file,"%s/%s",folder,m_levelpack->m_id);
#ifdef _WIN32
			_mkdir(folder_file);
#else
#ifdef __EMSCRIPTEN__
            char buffer[256];
            sprintf(buffer,"try{FS.mkdir(\"%s\");}catch(err){}",folder_file);
            emscripten_run_script(buffer);
#else
			{
				char tmp[512];
				sprintf(tmp,"mkdir %s",folder_file);
				system(tmp);
			}
#endif
#endif
            
			fp=fopen(map_file,"w+");
		} // if

		if (fp!=0) {
			m_map_data->save(fp,GLTM);
			fclose(fp);
		} // if
	}

} /* LevelPack_Level::save */ 

