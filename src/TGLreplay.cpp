#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
#include <time.h>

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
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


#include "Symbol.h"
#include "BB2D.h"
#include "auxiliar.h"
#include "GLTile.h"
#include "BB2D.h"
#include "sound.h"
#include "keyboardstate.h"
#include "randomc.h"
#include "VirtualController.h"
#include "compression.h"

#include "GLTManager.h"
#include "SFXManager.h"

#include "TGLobject.h"
#include "TGLobject_ship.h"
#include "TGLobject_ball.h"
#include "TGLmap.h"
#include "TGL.h"
#include "TGLapp.h"
#include "XMLparser.h"

#include "TGLreplay.h"

#include "debug.h"

extern char *application_version;


const char *objects_to_ignore[]={"TGLobject_ballstand",
						   "TGLobject_button",
						   "TGLobject_fuelrecharge",
						   "TGLobject_pipescreeen",
						   "TGLobject_pipevscreeen",
						   "TGLobject_radar",
						   "TGLobject_redlight",
						   "TGLobject_techno_computer",
						   "TGLobject_laser_horizontal",
						   "TGLobject_laser_vertical",
						   0};

TGLreplay_object_position::~TGLreplay_object_position()
{
	if (m_name!=0) {
		delete []m_name;
		m_name=0;
	} // if 
} /* TGLreplay_object_position::~TGLreplay_object_position */ 


TGLreplay_node::TGLreplay_node()
{
	m_keyframe=false;
} /* TGLreplay_node::TGLreplay_node */  


TGLreplay_node::~TGLreplay_node()
{
    for(VirtualController *vc:m_input) delete vc;
    m_input.clear();
    for(TGLreplay_object_position *op:m_objects) delete op;
    m_objects.clear();
    
} /* TGLreplay_node::~TGLreplay_node */  


// Warning: This code assumes that 'fp' was open in binary mode!!!
TGLreplay::TGLreplay(FILE *fp, GLTManager *GLTM)
{
	// Load a replay:
	XMLNode *node;
	XMLNode *version=0,*date=0,*map=0,*players=0,*cycles=0,*fuel=0,*length=0;
    XMLNode *embedded_map = 0;

	m_buffer=0;
	m_buffer_position=0;

	// Decompress the file:
	{
		unsigned l;
		m_buffer=decompress(fp,&l);
	}

	node=XMLNode::from_string(m_buffer,&m_buffer_position);

	m_version=0;
	m_map=0;
    m_embedded_map = 0;
    m_embedded_map_buffer = 0;
	m_year=0;
	m_month=0;
	m_day=0;
	m_hour=0;
	m_minute=0;
	m_second=0;
	m_initial_fuel=100;
	m_length=0;
    m_is_over = false;

	if (node!=0) {
		version = node->get_child("version");
		date = node->get_child("date");
		map = node->get_child("map");
        embedded_map = node->get_child("embeddedmap");
		fuel = node->get_child("initial-fuel");
		players = node->get_child("players");
		cycles = node->get_child("cycles");
		length = node->get_child("length");
	} // if 

	if (version!=0) {
		m_version=new char[strlen(version->get_value())+1];
		strcpy(m_version,version->get_value());
	} // if 

	if (date!=0) {
		XMLNode *tmp;

		tmp=date->get_child("year");
		if (tmp!=0) m_year=atoi(tmp->get_value());
		tmp=date->get_child("month");
		if (tmp!=0) m_month=atoi(tmp->get_value());
		tmp=date->get_child("day");
		if (tmp!=0) m_day=atoi(tmp->get_value());
		tmp=date->get_child("hour");
		if (tmp!=0) m_hour=atoi(tmp->get_value());
		tmp=date->get_child("minute");
		if (tmp!=0) m_minute=atoi(tmp->get_value());
		tmp=date->get_child("second");
		if (tmp!=0) m_second=atoi(tmp->get_value());
	} // if 

	if (map!=0) {
		m_map=new char[strlen(map->get_value())+1];
		strcpy(m_map,map->get_value());
	} // if
    
    if (embedded_map!=0) {
        int length = atoi(embedded_map->get_attribute("size"));
        char *buffer = new char[length+1];
        char *content = embedded_map->get_value();
        int contentLength = (int)strlen(content);
        int value = 0;
        bool next_is_high = true;
        int j = 0;
        for(int i = 0;i<contentLength;i++) {
            int c = content[i];
            int v = -1;
            if (c>='0' && c<='9') v = c-'0';
            if (c>='a' && c<='f') v = (c-'a')+10;
            if (v>=0) {
                if (next_is_high) {
                    value = v*16;
                    next_is_high = false;
                } else {
                    value = value + v;
                    buffer[j] = value;
                    j++;
                    next_is_high = true;
                }
            }
        }
        buffer[j] = 0;
        
        m_embedded_map = 0;
        m_embedded_map_buffer = buffer;
    }

	if (fuel!=0) {
		m_initial_fuel=atoi(fuel->get_value());
	} // if 

	// players
	if (players!=0) {
		std::vector<XMLNode *> *l;
		XMLNode *tmp;

		l=players->get_children();
        for(XMLNode *player:*l) {
			tmp=player->get_child("name");
			if (tmp!=0) {
				char *name=new char[strlen(tmp->get_value())+1];
				strcpy(name,tmp->get_value());
				m_players.push_back(name);
			} // if 
			tmp=player->get_child("ship");
			if (tmp!=0) {
				m_ships.push_back((atoi(tmp->get_value())));
			} // if 
		} // while 
	} // if

	if (length!=0) {
		m_length=atoi(length->get_value());
	} // if 

	delete node;

	m_statistics_computed = false;
	m_max_speed = 0;
	m_average_speed = 0;
	m_average_speed_tmp = 0;

} /* TGLreplay::TGLreplay */ 


TGLreplay::TGLreplay(char *map_name, TGLmap *map)
{
	m_version=new char[strlen(application_version)+1];
	strcpy(m_version,application_version);

	m_buffer=0;
	m_buffer_position=-1;
	m_length=0;
    m_embedded_map = 0;
    m_embedded_map_buffer = 0;
    m_is_over = false;

#ifdef _WIN32
    struct tm today;

    _tzset();
    _getsystime(&today);

	m_year=today.tm_year;
	m_month=today.tm_mon;
	m_day=today.tm_mday;
	m_hour=today.tm_hour;
	m_minute=today.tm_min;
	m_second=today.tm_sec;
#else
    struct timeval ttime;
    struct tm *today;

    gettimeofday(&ttime, NULL);
    today = localtime(&(ttime.tv_sec));

	m_year=today->tm_year;
	m_month=today->tm_mon;
	m_day=today->tm_mday;
	m_hour=today->tm_hour;
	m_minute=today->tm_min;
	m_second=today->tm_sec;
#endif

	m_map=new char[strlen(map_name)+1];
	strcpy(m_map,map_name);
    m_embedded_map = map;

	m_statistics_computed = false;
	m_max_speed = 0;
	m_average_speed = 0;
	m_average_speed_tmp = 0;
} /* TGLreplay::TGLreplay */ 


TGLreplay::~TGLreplay()
{
	if (m_buffer!=0) delete []m_buffer;
	m_buffer=0;

	if (m_version!=0) delete []m_version;
	m_version=0;

	if (m_map!=0) delete []m_map;
	m_map=0;
    
    if (m_embedded_map!=0) delete m_embedded_map;
    m_embedded_map = 0;
    
    if (m_embedded_map_buffer!=0) delete m_embedded_map_buffer;
    m_embedded_map_buffer = 0;
    
    for(TGLreplay_node *node:m_replay) delete node;
    m_replay.clear();
    
    for(char *p:m_players) delete []p;
    m_players.clear();
        
} /* TGLreplay::~TGLreplay */ 


bool TGLreplay::read_one_cycle(void)
{
	XMLNode *cycle;

	cycle=XMLNode::from_string(m_buffer,&m_buffer_position);

	if (cycle!=0) {
		XMLNode *input,*state,*text;
		TGLreplay_node *node;
		VirtualController *vc;
		char stmp1[16],stmp2[16],stmp3[16],stmp4[16],stmp5[16],stmp6[16],stmp7[16];
	
		node=new TGLreplay_node();

		input=cycle->get_child("input");
		text=cycle->get_child("text");
		state=cycle->get_child("state");

		if (input!=0) {
			std::vector<XMLNode *> *l2;

			l2=input->get_children();
            for(XMLNode *vc_node:*l2) {
				vc=new VirtualController();

				sscanf(vc_node->get_value(),"%s %s %s %s %s %s %s",stmp1,stmp2,stmp3,stmp4,stmp5,stmp6,stmp7);
				vc->m_joystick[0]=(stmp1[0]=='t' ? true:false);
				vc->m_joystick[1]=(stmp2[0]=='t' ? true:false);
				vc->m_joystick[2]=(stmp3[0]=='t' ? true:false);
				vc->m_joystick[3]=(stmp4[0]=='t' ? true:false);
				vc->m_button[0]=(stmp5[0]=='t' ? true:false);
				vc->m_button[1]=(stmp6[0]=='t' ? true:false);
				vc->m_menu=(stmp7[0]=='t' ? true:false);

				node->m_input.push_back(vc);

			} // while 
		} // if 

		if (state!=0) {
			node->m_keyframe=true;
			std::vector<XMLNode *> *l2;
			XMLNode *tmp;
			TGLreplay_object_position *op;

			l2=state->get_children();
            for(XMLNode *object:*l2) {
				op=new TGLreplay_object_position();

				tmp=object->get_child("name");
				if (tmp!=0) {
					op->m_name=new char[strlen(tmp->get_value())+1];
					strcpy(op->m_name,tmp->get_value());
				} else {
					op->m_name=0;
				} // if 

				tmp=object->get_child("x");
				if (tmp!=0) {
					op->m_x=load_float(tmp->get_value());
				} // if 

				tmp=object->get_child("y");
				if (tmp!=0) {
					op->m_y=load_float(tmp->get_value());
				} // if 

				tmp=object->get_child("speedx");
				if (tmp!=0) {
					op->m_speed_x=load_float(tmp->get_value());
				} // if 

				tmp=object->get_child("speedy");
				if (tmp!=0) {
					op->m_speed_y=load_float(tmp->get_value());
				} // if 

				tmp=object->get_child("angle");
				if (tmp!=0) {
					op->m_a=atoi(tmp->get_value());
				} // if 

				if (!replay_ignored_object(op)) {
					node->m_objects.push_back(op);
				} else {
					delete op;
				} // if 
			} // while 

		} else {
			node->m_keyframe=false;
		} // if 
		
		m_replay.push_back(node);
        delete cycle;
		return true;
	} // if 

    m_is_over = true;
	return false;
} /* TGLreplay::read_one_cycle */ 


int TGLreplay::get_length(void)
{
	return m_length;
} /* TGLreplay::get_length */ 


void TGLreplay::add_player(char *player_name,int ship)
{
	char *tmp=new char[strlen(player_name)+1];
	strcpy(tmp,player_name);
	m_players.push_back(tmp);
	m_ships.push_back((ship));
} /* TGLreplay::add_player */ 


bool TGLreplay::save(FILE *fp_out, bool compressData, bool embedMap)
{
	char *tmp;
	char buff[256],*buff2;
	std::list<char *> elements;


	sprintf(buff,"<replay-info>\n");
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<version>%s</version>\n",m_version);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<date>\n");
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<day>%i</day>\n",m_day);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<month>%i</month>\n",m_month);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<year>%i</year>\n",m_year);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<hour>%i</hour>\n",m_hour);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<minute>%i</minute>\n",m_minute);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<second>%i</second>\n",m_second);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"</date>\n");
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
	sprintf(buff,"<map>%s</map>\n",m_map);
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);
    
    if (embedMap) {
        
        FILE *fp = fopen(m_map,"rb");
        if (fp!=0) {
            char *buffer = 0;
            fseek(fp, 0L, SEEK_END);
            int embeddedLength = (int)ftell(fp);
            rewind(fp);
            
            buffer = new char[embeddedLength+1];
            fread(buffer, embeddedLength, embeddedLength, fp);
            fclose(fp);
        
            sprintf(buff,"<embeddedmap size=\"%i\">\n", embeddedLength);
            buff2=new char[strlen(buff)+1];
            strcpy(buff2,buff);
            elements.push_back(buff2);
            
            for(int i = 0;i<embeddedLength;) {
                const char *hex = "0123456789abcdef";
                int j = 0;
                for(;j<32 && i<embeddedLength;j++, i++) {
                    int c = buffer[i];
                    int low_four = c&0x0f;
                    int high_four = (c&0xf0)>>4;
                    buff[j*2] = hex[high_four];
                    buff[j*2+1] = hex[low_four];
                }
                buff[j*2] = '\n';
                buff[j*2+1] = 0;
                buff2=new char[strlen(buff)+1];
                strcpy(buff2,buff);
                elements.push_back(buff2);
            }
            
            sprintf(buff,"</embeddedmap>\n");
            buff2=new char[strlen(buff)+1];
            strcpy(buff2,buff);
            elements.push_back(buff2);
            
            delete []buffer;
        }
    }
    
	sprintf(buff,"<players>\n");
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);

    for(int i = 0;i<m_players.size() && i<=m_ships.size();i++) {
        char *tmp = getAt<char *>(m_players, i);
        int itmp = getAt<int>(m_ships, i);
		sprintf(buff,"<player><name>%s</name><ship>%i</ship></player>\n",tmp,itmp);
		buff2=new char[strlen(buff)+1];
		strcpy(buff2,buff);
		elements.push_back(buff2);
	} // while 
	
	sprintf(buff,"</players>\n");
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);

	sprintf(buff,"<length>%i</length>\n",(int)m_replay.size());
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);

	sprintf(buff,"</replay-info>\n");
	buff2=new char[strlen(buff)+1];
	strcpy(buff2,buff);
	elements.push_back(buff2);	

	int i=0;
    for(TGLreplay_node *node:m_replay) {
		sprintf(buff,"<cycle num=\"%i\">\n<input>\n",i++);
		buff2=new char[strlen(buff)+1];
		strcpy(buff2,buff);
		elements.push_back(buff2);

        for(VirtualController *vc:node->m_input) {
			sprintf(buff,"<vc>%s %s %s %s %s %s %s</vc>",
				(vc->m_joystick[0] ? "t":"f"),
				(vc->m_joystick[1] ? "t":"f"),
				(vc->m_joystick[2] ? "t":"f"),
				(vc->m_joystick[3] ? "t":"f"),
				(vc->m_button[0] ? "t":"f"),
				(vc->m_button[1] ? "t":"f"),
                (vc->m_menu ? "t":"f"));
			buff2=new char[strlen(buff)+1];
			strcpy(buff2,buff);
			elements.push_back(buff2);
		} // while 
		sprintf(buff,"</input>\n");
		buff2=new char[strlen(buff)+1];
		strcpy(buff2,buff);
		elements.push_back(buff2);

		if (node->m_keyframe) {
			sprintf(buff,"<state>\n");
			buff2=new char[strlen(buff)+1];
			strcpy(buff2,buff);
			elements.push_back(buff2);

            for(TGLreplay_object_position *op:node->m_objects) {
				sprintf(buff,"<object>\n<name>%s</name>\n<x>",op->m_name);
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				

				save_float(op->m_x,buff);
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				

				sprintf(buff,"</x>\n<y>");
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				

				save_float(op->m_y,buff);
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				
				
				sprintf(buff,"</y>\n<speedx>");
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				

				save_float(op->m_speed_x,buff);
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				
				
				sprintf(buff,"</speedx>\n<speedy>");
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				

				save_float(op->m_speed_y,buff);
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				

				sprintf(buff,"</speedy>\n<angle>%i</angle>\n</object>\n",op->m_a);
				buff2=new char[strlen(buff)+1];
				strcpy(buff2,buff);
				elements.push_back(buff2);				
			} // while 
			sprintf(buff,"</state>\n");
			buff2=new char[strlen(buff)+1];
			strcpy(buff2,buff);
			elements.push_back(buff2);				
		} // if 
		sprintf(buff,"</cycle>\n");
		buff2=new char[strlen(buff)+1];
		strcpy(buff2,buff);
		elements.push_back(buff2);				
	} // while 
	
    if (compressData) {
		int len,pos;

		len=0;
        for(char *token:elements) {
			len+=strlen(token)+1;
		} /* while */ 
		len++;

		tmp=new char[len];

		pos=0;
        for(char *token:elements) {
			sprintf(tmp+pos,"%s",token);
			pos+=strlen(token);
		} /* while */ 
		tmp[pos]=0;


//		fprintf(fp_out,"%s",tmp);

        compress(tmp,len,fp_out);
		delete []tmp;
    } else {
        for(char *token:elements) {
            fprintf(fp_out,"%s",token);
        }
    }
    
    for(char *tmp:elements) delete []tmp;
    elements.clear();

	return true;
} /* TGLreplay::save */ 


void TGLreplay::store_cycle(std::list<VirtualController *> *m_input,std::list<TGLobject *> *m_objects)
{
	int cycle=(int)m_replay.size();
	TGLreplay_node *node=0;
	VirtualController *vc2;
	TGLreplay_object_position *ro;
	const char *name;

	m_statistics_computed = false;

    for(TGLobject *o:*m_objects) {
        if (o->is_a("TGLobject_ship")) {
            float sx = ((TGLobject_ship *)o)->get_speedx();
            float sy = ((TGLobject_ship *)o)->get_speedy();
            float speed = float(sqrt(sx*sx+sy*sy));
            m_average_speed_tmp += speed;
            m_average_speed = m_average_speed_tmp/(m_length+1);
            if (speed>m_max_speed) m_max_speed = speed;
        } // if
    } // for

	if ((cycle%KEYFRAME_PERIOD)==0) {
		node = new TGLreplay_node();
		node->m_keyframe=true;
		
        for(VirtualController *vc:*m_input) {
			vc2=new VirtualController(vc);
			node->m_input.push_back(vc2);
		} // for

        for(TGLobject *o:*m_objects) {
			
			if (!replay_ignored_object(o)) {
				ro=new TGLreplay_object_position();

				name=o->get_class();
				ro->m_name=new char[strlen(name)+1];
				strcpy(ro->m_name,name);
				ro->m_x=o->get_x();
				ro->m_y=o->get_y();
				ro->m_a=o->get_angle();

				if (o->is_a("TGLobject_ship")) {
					ro->m_speed_x=((TGLobject_ship *)o)->get_speedx();
					ro->m_speed_y=((TGLobject_ship *)o)->get_speedy();
				} else if (o->is_a("TGLobject_ball")) {
					ro->m_speed_x=((TGLobject_ball *)o)->get_speed_x();
					ro->m_speed_y=((TGLobject_ball *)o)->get_speed_y();
				} else {
					ro->m_speed_x=0;
					ro->m_speed_y=0;
				} // if

				node->m_objects.push_back(ro);
			} // if
		} // while

		m_replay.push_back(node);

	} else {
		node = new TGLreplay_node();
		node->m_keyframe=false;
		
        for(VirtualController *vc:*m_input) {
			vc2=new VirtualController(vc);
			node->m_input.push_back(vc2);
		} // for

		m_replay.push_back(node);
	} // if 

	m_length++;

} /* TGLreplay::store_cycle */ 


bool TGLreplay::execute_cycle(std::list<VirtualController *> *m_input,std::list<TGLobject *> *objects,std::list<TGLobject *> *todelete,std::list<TGLobject *> *toadd)
{
	TGLreplay_node *node;
	bool retval;

	retval=read_one_cycle();
    node = m_replay.back();

    for(std::list<VirtualController *>::iterator i1 = m_input->begin(),
        i2 = node->m_input.begin();
        i1 != m_input->end() && i2 != node->m_input.end();
        i1++, i2++) {
        VirtualController *vc1= *i1, *vc2 = *i2;
		vc1->new_cycle();
		vc1->copy_current(vc2);
	} // for

	if (node->m_keyframe) {
        for(TGLreplay_object_position *ro:node->m_objects) {
			float closest=0,d;
			TGLobject *go_found=0;
            for(TGLobject *go:*objects) {
                if (go_found!=0) break;
				if (strcmp(go->get_class(),ro->m_name)==0) {
					d=(go->get_x()-ro->m_x)*(go->get_x()-ro->m_x)+(go->get_y()-ro->m_y)*(go->get_y()-ro->m_y);
					if (go_found==0 || d<closest) {
						go_found = go;
						closest=d;
					} // if
				} // if 
			} // while 

			if (go_found!=0) {
				objects->remove(go_found);
#ifdef __DEBUG_MESSAGES
				if (go_found->get_x()!=ro->m_x ||
					go_found->get_y()!=ro->m_y) {
					output_debug_message("TGLreplay: [%i] Adjustment in object '%s' (%g,%g) -> (%g,%g)\n",indexOf<TGLreplay_node *>(m_replay, node),ro->m_name,go_found->get_x(),go_found->get_y(),ro->m_x,ro->m_y);
				} // if 
#endif
				go_found->set_x(ro->m_x);
				go_found->set_y(ro->m_y);
				if (go_found->is_a("TGLobject_ship")) {
					((TGLobject_ship *)go_found)->set_speedx(ro->m_speed_x);
					((TGLobject_ship *)go_found)->set_speedy(ro->m_speed_y);
				} else if (go_found->is_a("TGLobject_ball")) {
					((TGLobject_ball *)go_found)->set_speed_x(ro->m_speed_x);
					((TGLobject_ball *)go_found)->set_speed_y(ro->m_speed_y);
				} // if
			} else {
				// Create a new object:
                TGLobject *newObject = TGLobject::createObjectFromClassName(ro->m_name, ro->m_x, ro->m_y);
                if (newObject!=0) {
                    if (go_found->is_a("TGLobject_ship")) {
                        ((TGLobject_ship *)newObject)->set_speedx(ro->m_speed_x);
                        ((TGLobject_ship *)newObject)->set_speedy(ro->m_speed_y);
                    } else if (go_found->is_a("TGLobject_ball")) {
                        ((TGLobject_ball *)newObject)->set_speed_x(ro->m_speed_x);
                        ((TGLobject_ball *)newObject)->set_speed_y(ro->m_speed_y);
                    } // if
                    toadd->push_back(newObject);
                }
			} // if
		} // while 

        for(TGLobject *go:*objects) {
			if (!replay_ignored_object(go)) todelete->push_back(go);
		} // while
        objects->clear();
		
	} // if 

	return retval;
} /* TGLreplay::execute_cycle */ 


int TGLreplay::get_playership(char *player_name)
{
    int idx = 0;
    for(char *tmp:m_players) {
        if (strcmp(player_name, tmp)==0) {
            return getAt<int>(m_ships, idx);
        }
        idx++;
    }

	return -1;
} /* TGLreplay::get_playership */ 


char *TGLreplay::get_map_name(void)
{
	return m_map;
} /* TGLreplay::get_map */ 


TGLmap *TGLreplay::get_map(GLTManager *GLTM)
{
    if (m_embedded_map==0 && m_embedded_map_buffer!=0) {
        m_embedded_map = new TGLmap(m_embedded_map_buffer, GLTM);
    }
    return m_embedded_map;
} /* TGLreplay::get_map */


TGLmap *TGLreplay::get_map_copy(GLTManager *GLTM)
{
    TGLmap *map = new TGLmap(m_embedded_map_buffer, GLTM);
    return map;
}


void TGLreplay::set_map(TGLmap *map)
{
    m_embedded_map = map;
}


int TGLreplay::get_initial_fuel(void)
{
	return m_initial_fuel;
} /* TGLreplay::get_initial_fuel */ 


float TGLreplay::get_max_speed(void)
{
	return m_max_speed;
} /* TGLreplay::get_max_speed */ 


float TGLreplay::get_average_speed(void)
{
	return m_average_speed;
} /* TGLreplay::get_average_speed */ 


int TGLreplay::get_fuel_used(void)
{
	if (m_statistics_computed) {
		return m_used_fuel;
	} else {
		compute_statistics();
		return m_used_fuel;
	} // if 
} /* TGLreplay::get_fuel_used */ 

int TGLreplay::get_n_shots(void)
{
	if (m_statistics_computed) {
		return m_n_shots;
	} else {
		compute_statistics();
		return m_n_shots;
	} // if 
} /* TGLreplay::get_n_shots */ 




void TGLreplay::compute_statistics(void)
{
	int ship = m_ships.front();

	m_used_fuel = 0;
	m_n_shots = 0;

    for(TGLreplay_node *n:m_replay) {
		// Computing Fuel:
		if (n->m_input.front()->m_button[0]) {
			switch(ship) {
			case 0:	if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=64;
						m_n_shots++;
					} // if
					break;
			case 1: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=96;
						m_n_shots++;
					} // if 
					break;
			case 2: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=40;
						m_n_shots++;
					} // if
					break;
			case 3: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=40;
						m_n_shots++;
					} // if
					break;
			case 4: if (!n->m_input.front()->m_old_button[0]) m_n_shots++;
					m_used_fuel+=2;
					break;
			case 5: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=40;
						m_n_shots++;
					} // if
					break;
			case 6: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=40;
						m_n_shots++;
					} // if
					break;
			case 7: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=40;
						m_n_shots++;
					} // if
					break;
			case 8: if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=64;
						m_n_shots++;
					} // if
					break;
			case 9: m_used_fuel+=6;
					break;
			case 10:if (!n->m_input.front()->m_old_button[0]) {
						m_used_fuel+=32;
					} // if
					break;
			} // switch
		} // if 
		if (n->m_input.front()->m_joystick[VC_UP]) {
			if (ship==3) {
				if (n->m_input.front()->m_joystick[VC_DOWN]) {
					m_used_fuel+=2;
				} else {
					m_used_fuel++;
				} // if 
			} else {
				m_used_fuel++;
			} // if
		} // if 
		if (n->m_input.front()->m_joystick[VC_DOWN]) {
			if (!n->m_input.front()->m_joystick[VC_UP] && ship==2) m_used_fuel++;
			if (n->m_input.front()->m_joystick[VC_LEFT] && ship==5) m_used_fuel++;
			if (n->m_input.front()->m_joystick[VC_RIGHT] && ship==5) m_used_fuel++;
			if (ship==6) m_used_fuel++;
		} // if
	} // while 
} /* TGLreplay::compute_statistics */ 


char *TGLreplay::get_playername(int player)
{
	return getAt<char *>(m_players, player);
} /* TGLreplay::get_playername */ 


bool TGLreplay::replay_ignored_object(TGLobject *o)
{
	int i;

	for(i=0;objects_to_ignore[i]!=0;i++) {
		if (strcmp(o->get_class(),objects_to_ignore[i])==0) return true;
	} // if 

	return false;
} /* TGLreplay::replay_ignored_object */ 


bool TGLreplay::replay_ignored_object(TGLreplay_object_position *o)
{
	int i;

	for(i=0;objects_to_ignore[i]!=0;i++) {
		if (strcmp(o->m_name,objects_to_ignore[i])==0) return true;
	} // if 

	return false;
} /* TGLreplay::replay_ignored_object */ 



void TGLreplay::save_float(float v,FILE *fp)
{
	char tmp[9];
	char *ptr=(char *)&v;

#if SDL_BYTEORDER == SDL_LITTLE_ENDIAN
	tmp[0]='a'+(ptr[0]&0xf);
	tmp[1]='a'+((ptr[0]>>4)&0xf);
	tmp[2]='a'+(ptr[1]&0xf);
	tmp[3]='a'+((ptr[1]>>4)&0xf);
	tmp[4]='a'+(ptr[2]&0xf);
	tmp[5]='a'+((ptr[2]>>4)&0xf);
	tmp[6]='a'+(ptr[3]&0xf);
	tmp[7]='a'+((ptr[3]>>4)&0xf);
#else
	tmp[0]='a'+(ptr[3]&0xf);
	tmp[1]='a'+((ptr[3]>>4)&0xf);
	tmp[2]='a'+(ptr[2]&0xf);
	tmp[3]='a'+((ptr[2]>>4)&0xf);
	tmp[4]='a'+(ptr[1]&0xf);
	tmp[5]='a'+((ptr[1]>>4)&0xf);
	tmp[6]='a'+(ptr[0]&0xf);
	tmp[7]='a'+((ptr[0]>>4)&0xf);
#endif
	tmp[8]=0;

	fprintf(fp,"%s",tmp);
} /* TGLreplay::save_float */ 


void TGLreplay::save_float(float v,char *tmp)
{
	char *ptr=(char *)&v;

#if SDL_BYTEORDER == SDL_LITTLE_ENDIAN
	tmp[0]='a'+(ptr[0]&0xf);
	tmp[1]='a'+((ptr[0]>>4)&0xf);
	tmp[2]='a'+(ptr[1]&0xf);
	tmp[3]='a'+((ptr[1]>>4)&0xf);
	tmp[4]='a'+(ptr[2]&0xf);
	tmp[5]='a'+((ptr[2]>>4)&0xf);
	tmp[6]='a'+(ptr[3]&0xf);
	tmp[7]='a'+((ptr[3]>>4)&0xf);
#else
	tmp[0]='a'+(ptr[3]&0xf);
	tmp[1]='a'+((ptr[3]>>4)&0xf);
	tmp[2]='a'+(ptr[2]&0xf);
	tmp[3]='a'+((ptr[2]>>4)&0xf);
	tmp[4]='a'+(ptr[1]&0xf);
	tmp[5]='a'+((ptr[1]>>4)&0xf);
	tmp[6]='a'+(ptr[0]&0xf);
	tmp[7]='a'+((ptr[0]>>4)&0xf);
#endif
	tmp[8]=0;
} /* TGLreplay::save_float */ 


float TGLreplay::load_float(char *str)
{
	float tmp=0;
	char *ptr=(char *)&tmp;

	if ((str[0]>='0' && str[0]<='9') || str[0]=='-') {
		tmp=float(atof(str));
	} else {
#if SDL_BYTEORDER == SDL_LITTLE_ENDIAN
		ptr[0]|=(str[0]-'a');
		ptr[0]|=(str[1]-'a')<<4;
		ptr[1]|=(str[2]-'a');
		ptr[1]|=(str[3]-'a')<<4;
		ptr[2]|=(str[4]-'a');
		ptr[2]|=(str[5]-'a')<<4;
		ptr[3]|=(str[6]-'a');
		ptr[3]|=(str[7]-'a')<<4;
#else
		ptr[3]|=(str[0]-'a');
		ptr[3]|=(str[1]-'a')<<4;
		ptr[2]|=(str[2]-'a');
		ptr[2]|=(str[3]-'a')<<4;
		ptr[1]|=(str[4]-'a');
		ptr[1]|=(str[5]-'a')<<4;
		ptr[0]|=(str[6]-'a');
		ptr[0]|=(str[7]-'a')<<4;
#endif
	} // if 

	return tmp;
} /* TGLreplay::load_float */ 