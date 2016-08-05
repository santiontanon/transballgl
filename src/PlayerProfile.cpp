#ifdef _WIN32
#include <windows.h>
#include "direct.h"
#else
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
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

#include "PlayerProfile.h"
#include "LevelPack.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
extern const char *persistent_data_folder;
#endif


/*
// This is the default profile file:

<player-profile>
  <name>player</name>
  <lastlevelpack>maps/st2.lp</lastlevelpack>
  <video>
    <mode>window</mode>
    <resolutionx>640</resolutionx>
    <resolutiony>480</resolutiony>
    <zoom>1.0</zoom>
  </video>
  <audio>
    <sfx-volume>127</sfx-volume>
    <music-volume>96</music-volume>
  </audio>
  <input>
      <keyboard player="0">
        <up>113</up>
        <down>97</down>
        <left>111</left>
        <right>112</right>
        <button1>32</button1>
        <button2>13</button2>
        <pause>282</pause>
        <quit>27</quit>
    </keyboard>
  </input>
  <ships>
    <ship>0</ship>
    <ship>1</ship>
    <ship>2</ship>
  </ships>
  <progress>
  </progress>
</player-profile>
*/


PlayerProfileLPProgress::PlayerProfileLPProgress()
{
	m_levelpack_id=0;
	m_levels_completed=0;
	m_points=0;
} /* PlayerProfileLPProgress::PlayerProfileLPProgress */ 


PlayerProfileLPProgress::~PlayerProfileLPProgress()
{
	if (m_levelpack_id!=0) delete m_levelpack_id;
	m_levelpack_id=0;
    for(PlayerProfileLevelResult *tmp:m_results) delete tmp;
    m_results.clear();
} /* PlayerProfileLPProgress::~PlayerProfileLPProgress */ 


PlayerProfile::PlayerProfile(char *name)
{
	if (name!=0) {
		m_name = new char[strlen(name)+1];
		strcpy(m_name,name);
	} else {
		m_name=0;
	} // if 
	m_fullscreen=false;
	m_resolution_x=DEFAULT_SCREEN_X;
	m_resolution_y=DEFAULT_SCREEN_Y;
	m_sfx_volume=MIX_MAX_VOLUME;
	m_music_volume=96;
    m_zoom = 1.0f;
	m_n_players=1;
    m_keys_configuration[0][KEY_THRUST]=SDL_SCANCODE_Q;
	m_keys_configuration[0][KEY_SPECIAL]=SDL_SCANCODE_A;
	m_keys_configuration[0][KEY_LEFT]=SDL_SCANCODE_O;
	m_keys_configuration[0][KEY_RIGHT]=SDL_SCANCODE_P;
	m_keys_configuration[0][KEY_FIRE]=SDL_SCANCODE_SPACE;
	m_keys_configuration[0][KEY_ATTRACTOR]=SDL_SCANCODE_RETURN;
	m_keys_configuration[0][KEY_MENU]=SDL_SCANCODE_ESCAPE;

	m_ships.push_back(0);
	m_ships.push_back(1);
	m_ships.push_back(2);
    if (strcmp(m_name,"mazeofgalious")==0) {
        for(int i = 3;i<N_SHIPS;i++) {
            m_ships.push_back(i);
        }
    }

	
	m_last_levelpack = new char[12];
	strcpy(m_last_levelpack,TGL_PLAY_MAPS_FOLDER "/tgl.lp");
} /* PlayerProfile::PlayerProfile */ 



PlayerProfile::PlayerProfile(FILE *fp)
{
	m_name=0;
	m_last_levelpack=0;
	m_fullscreen=false;
	m_resolution_x = DEFAULT_SCREEN_X;
	m_resolution_y = DEFAULT_SCREEN_Y;
	m_sfx_volume=MIX_MAX_VOLUME;
	m_music_volume=96;
    m_zoom = 1.0f;
	m_n_players=1;
	m_keys_configuration[0][KEY_THRUST]=SDL_SCANCODE_Q;
	m_keys_configuration[0][KEY_SPECIAL]=SDL_SCANCODE_A;
	m_keys_configuration[0][KEY_LEFT]=SDL_SCANCODE_O;
	m_keys_configuration[0][KEY_RIGHT]=SDL_SCANCODE_P;
	m_keys_configuration[0][KEY_FIRE]=SDL_SCANCODE_SPACE;
	m_keys_configuration[0][KEY_ATTRACTOR]=SDL_SCANCODE_RETURN;
	m_keys_configuration[0][KEY_MENU]=SDL_SCANCODE_ESCAPE;

	XMLNode *node=XMLNode::from_file(fp);
	XMLNode *name,*video,*audio,*input,*ships,*progress,*lastlevelpack;
	XMLNode *tmp;

	name = node->get_child("name");
	lastlevelpack = node->get_child("lastlevelpack");
	video = node->get_child("video");
	audio = node->get_child("audio");
	input = node->get_child("input");
	ships = node->get_child("ships");
	progress = node->get_child("progress");

	if (name!=0) {
		m_name = new char[strlen(name->get_value())+1];
		strcpy(m_name,name->get_value());
	} // if 
	
	if (lastlevelpack!=0) {
		m_last_levelpack = new char[strlen(lastlevelpack->get_value())+1];
		strcpy(m_last_levelpack,lastlevelpack->get_value());
	} else {
		m_last_levelpack = new char[12];
		strcpy(m_last_levelpack,TGL_PLAY_MAPS_FOLDER "/tgl.lp");
	}

	if (video!=0) {
		tmp = video->get_child("mode");
		if (tmp!=0) {
			if (strcmp(tmp->get_value(),"window")==0) m_fullscreen=false;
                                                 else m_fullscreen=true;
		} // if 
		tmp = video->get_child("resolutionx");
		if (tmp!=0) {
			m_resolution_x=atoi(tmp->get_value());
		} // if 
		tmp = video->get_child("resolutiony");
		if (tmp!=0) {
			m_resolution_y=atoi(tmp->get_value());
		} // if 
        tmp = video->get_child("zoom");
        if (tmp!=0) {
            m_zoom=atof(tmp->get_value());
        } // if 
	} // if

	if (audio!=0) {
		tmp = video->get_child("mode");
		tmp = video->get_child("sfx-volume");
		if (tmp!=0) m_sfx_volume=atoi(tmp->get_value());
		tmp = video->get_child("music-volume");
		if (tmp!=0) m_music_volume=atoi(tmp->get_value());
	} // if 

	if (input!=0) {
		std::vector<XMLNode *> *l=input->get_children();
		XMLNode *tmp;

		m_n_players=0;

        for(XMLNode *keyboard:*l) {
			tmp=keyboard->get_child("up");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_THRUST]=atoi(tmp->get_value());
			tmp=keyboard->get_child("down");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_SPECIAL]=atoi(tmp->get_value());
			tmp=keyboard->get_child("left");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_LEFT]=atoi(tmp->get_value());
			tmp=keyboard->get_child("right");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_RIGHT]=atoi(tmp->get_value());
			tmp=keyboard->get_child("button1");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_FIRE]=atoi(tmp->get_value());
			tmp=keyboard->get_child("button2");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_ATTRACTOR]=atoi(tmp->get_value());
			tmp=keyboard->get_child("menu");
			if (tmp!=0) m_keys_configuration[m_n_players][KEY_MENU]=atoi(tmp->get_value());

			m_n_players++;
		} // while 
	} // if

	if (ships!=0) {
		std::vector<XMLNode *> *l=ships->get_children();
        for(XMLNode *ship:*l) {
			m_ships.push_back((atoi(ship->get_value())));
		} // while
	} // if

	if (progress!=0) {
		std::vector<XMLNode *> *l=progress->get_children();
		XMLNode *tmp,*results;
		PlayerProfileLPProgress *lpp;
		PlayerProfileLevelResult *lr;

        for(XMLNode *levelpack:*l) {
			lpp=new PlayerProfileLPProgress();

			tmp=levelpack->get_child("id");
			if (tmp!=0) {
				lpp->m_levelpack_id=new char[strlen(tmp->get_value())+1];
				strcpy(lpp->m_levelpack_id,tmp->get_value());
			} // if 
			tmp=levelpack->get_child("levels-completed");
			if (tmp!=0) lpp->m_levels_completed=atoi(tmp->get_value());
			tmp=levelpack->get_child("points");
			if (tmp!=0) lpp->m_points=atoi(tmp->get_value());

			results=levelpack->get_child("results");
			if (results!=0) {
				std::vector<XMLNode *> *l2=results->get_children();

                for(XMLNode *result:*l2) {
					lr=new PlayerProfileLevelResult();

					tmp=result->get_child("level");
					if (tmp!=0) lr->m_level=atoi(tmp->get_value());
					tmp=result->get_child("ship");
					if (tmp!=0) lr->m_ship=atoi(tmp->get_value());
					tmp=result->get_child("time");
					if (tmp!=0) lr->m_best_time=atoi(tmp->get_value());
					tmp=result->get_child("fuelused");
					if (tmp!=0) lr->m_fuel_used=atoi(tmp->get_value());
					tmp=result->get_child("shots");
					if (tmp!=0) lr->m_shots=atoi(tmp->get_value());
					tmp=result->get_child("impacts");
					if (tmp!=0) lr->m_impacts=atoi(tmp->get_value());
					tmp=result->get_child("kills");
					if (tmp!=0) lr->m_kills=atoi(tmp->get_value());

					lpp->m_results.push_back(lr);
				} // while 
			} // if 

			m_progress.push_back(lpp);
			
		} // while 
	} // if

	delete node;
} /* PlayerProfile::PlayerProfile */ 


PlayerProfile::~PlayerProfile()
{
	if (m_name!=0) delete []m_name;
	m_name=0;
	if (m_last_levelpack!=0) delete []m_last_levelpack;
	m_last_levelpack=0;
    
    for(PlayerProfileLPProgress *tmp:m_progress) delete tmp;
    m_progress.clear();
} /* PlayerProfile::~PlayerProfile */ 


bool PlayerProfile::save(FILE *fp)
{
	fprintf(fp,"<player-profile>\n");
	if (m_name!=0) fprintf(fp,"  <name>%s</name>\n",m_name);
	if (m_last_levelpack!=0) fprintf(fp,"  <lastlevelpack>%s</lastlevelpack>\n",m_last_levelpack);
	fprintf(fp,"  <video>\n");
	fprintf(fp,"    <mode>%s</mode>\n",(m_fullscreen ? "fullscreen":"window"));
	fprintf(fp,"    <resolutionx>%i</resolutionx>\n",m_resolution_x);
	fprintf(fp,"    <resolutiony>%i</resolutiony>\n",m_resolution_y);
    fprintf(fp,"    <zoom>%g</zoom>\n",m_zoom);
	fprintf(fp,"  </video>\n");
	fprintf(fp,"  <audio>\n");
	fprintf(fp,"    <sfx-volume>%i</sfx-volume>\n",m_sfx_volume);
	fprintf(fp,"    <music-volume>%i</music-volume>\n",m_music_volume);
	fprintf(fp,"  </audio>\n");
	fprintf(fp,"  <input>\n");
	for(int i=0;i<m_n_players;i++) {
		fprintf(fp,"      <keyboard player=\"%i\">\n",i);
		fprintf(fp,"        <up>%i</up>\n",m_keys_configuration[i][KEY_THRUST]);
		fprintf(fp,"        <down>%i</down>\n",m_keys_configuration[i][KEY_SPECIAL]);
		fprintf(fp,"        <left>%i</left>\n",m_keys_configuration[i][KEY_LEFT]);
		fprintf(fp,"        <right>%i</right>\n",m_keys_configuration[i][KEY_RIGHT]);
		fprintf(fp,"        <button1>%i</button1>\n",m_keys_configuration[i][KEY_FIRE]);
		fprintf(fp,"        <button2>%i</button2>\n",m_keys_configuration[i][KEY_ATTRACTOR]);
		fprintf(fp,"        <menu>%i</menu>\n",m_keys_configuration[i][KEY_MENU]);
		fprintf(fp,"    </keyboard>\n");
	} // for
	fprintf(fp,"  </input>\n");
	fprintf(fp,"  <ships>\n");
    for(int ip:m_ships) {
		fprintf(fp,"    <ship>%i</ship>\n",ip);
	} // while
	fprintf(fp,"  </ships>\n");
	fprintf(fp,"  <progress>\n");
    for(PlayerProfileLPProgress *lpp:m_progress) {
		fprintf(fp,"    <levelpack>\n");
		fprintf(fp,"      <id>%s</id>\n",lpp->m_levelpack_id);
		fprintf(fp,"      <levels-completed>%i</levels-completed>\n",lpp->m_levels_completed);
		fprintf(fp,"      <points>%i</points>\n",lpp->m_points);
		fprintf(fp,"      <results>\n");
        for(PlayerProfileLevelResult *lr:lpp->m_results) {
			fprintf(fp,"        <result>\n");
			fprintf(fp,"          <level>%i</level>\n",lr->m_level);
			fprintf(fp,"          <ship>%i</ship>\n",lr->m_ship);
			fprintf(fp,"          <time>%i</time>\n",lr->m_best_time);
			fprintf(fp,"          <fuelused>%i</fuelused>\n",lr->m_fuel_used);
			fprintf(fp,"          <shots>%i</shots>\n",lr->m_shots);
			fprintf(fp,"          <impacts>%i</impacts>\n",lr->m_impacts);
			fprintf(fp,"          <kills>%i</kills>\n",lr->m_kills);
			fprintf(fp,"        </result>\n");
		} // while
		fprintf(fp,"      </results>\n");
		fprintf(fp,"    </levelpack>\n");
	} // while 
	fprintf(fp,"  </progress>\n");
	fprintf(fp,"</player-profile>\n");

	return true;
} /* PlayerProfile::save */ 


int PlayerProfile::progress_in_levelpack(char *id)
{
    if (strcmp(m_name,"mazeofgalious")==0) return __INT_MAX__;  // cheat code
    
    for(PlayerProfileLPProgress *lpp:m_progress) {
		if (strcmp(id,lpp->m_levelpack_id)==0) return lpp->m_levels_completed;
	} // while 
	return 0;
} /* PlayerProfile::progress_in_levelpack */ 



void PlayerProfile::level_completed(char *player_data_path,char *levelpack_id,int level,TGLreplay *replay,LevelPack *lp)
{
	PlayerProfileLPProgress *selected=0;
	PlayerProfileLevelResult *selected_l=0;

    for(PlayerProfileLPProgress *lpp:m_progress) {
		if (strcmp(levelpack_id,lpp->m_levelpack_id)==0) selected=lpp;
	} // while 

	if (selected==0) {
		selected=new PlayerProfileLPProgress();
		m_progress.push_back(selected);
		selected->m_levelpack_id=new char[strlen(levelpack_id)+1];
		strcpy(selected->m_levelpack_id,levelpack_id);
	} // if 

	if (level+1>selected->m_levels_completed) selected->m_levels_completed=level+1;

    for(PlayerProfileLevelResult *lr:selected->m_results) {
		if (lr->m_level==level && lr->m_ship==replay->get_playership(m_name)) selected_l=lr;
	} // while 

	if (selected_l==0) {
		selected_l=new PlayerProfileLevelResult();
		selected_l->m_level=level;
		selected_l->m_ship=replay->get_playership(m_name);
		selected_l->m_best_time=-1;
		selected_l->m_fuel_used=0;
		selected_l->m_shots=0;
		selected_l->m_impacts=0;
		selected_l->m_kills=0;
		selected->m_results.push_back(selected_l);
	
		selected->m_points+=lp->get_points(level);
	} // if 

	if (selected_l->m_best_time==-1 || selected_l->m_best_time>replay->get_length()*18) {
		selected_l->m_best_time=replay->get_length()*18;
		selected_l->m_fuel_used=0;
		selected_l->m_shots=0;
		selected_l->m_impacts=0;
		selected_l->m_kills=0;

		// Save replay:
		{
			FILE *fp;
			char tmp[512];

//            FILE *fp_uncompressed;
//            char tmp_uncompessed[512];

            sprintf(tmp,"%splayers/%s/%s-level-%i-%i.rpl", player_data_path ,m_name,levelpack_id,level,replay->get_playership(m_name));
//			sprintf(tmp_uncompessed,"%splayers/%s/%s-level-%i-%i.rpl.xml", player_data_path ,m_name,levelpack_id,level,replay->get_playership(m_name));

			fp=fopen(tmp,"wb");
			if (fp==0) {
				// assume the folder does not exist:
				char tmp2[256];
				sprintf(tmp2,"%splayers/%s", player_data_path ,m_name);
#ifdef _WIN32
				_mkdir(tmp2);
#else
#ifdef __EMSCRIPTEN__
                char buffer[256];
                sprintf(buffer,"try{FS.mkdir(\"%s\");}catch(err){}",tmp2);
                emscripten_run_script(buffer);
#else
				{	
					char tmp3[512];
					sprintf(tmp3,"mkdir %s",tmp2);
					system(tmp3);
				}
#endif				
#endif

				fp=fopen(tmp,"wb");
			} // if 
//            fp_uncompressed=fopen(tmp_uncompessed,"w");

			if (fp!=0) {
				replay->save(fp, true, true);
				fclose(fp);
			} // if 
//            if (fp_uncompressed!=0) {
//                replay->save(fp_uncompressed, false, true);
//                fclose(fp_uncompressed);
//            } // if
		}
	} // if 

} /* PlayerProfile::level_completed */ 


int PlayerProfile::get_besttime(char *levelpack_id,int level,int ship)
{
	PlayerProfileLPProgress *selected=0;
	PlayerProfileLevelResult *selected_l=0;

    for(PlayerProfileLPProgress *lpp:m_progress) {
		if (strcmp(levelpack_id,lpp->m_levelpack_id)==0) selected=lpp;
	} // while 

	if (selected==0) return -1;

    for(PlayerProfileLevelResult *lr:selected->m_results) {
		if (lr->m_level==level && lr->m_ship==ship) selected_l=lr;
	} // while 

	if (selected_l==0) return -1;

	return selected_l->m_best_time;
} /* PlayerProfile::get_besttime */ 


int PlayerProfile::get_points(void)
{
	int points=0;
    for(PlayerProfileLPProgress *lpp:m_progress) {
		points+=lpp->m_points;
	} // while 

	return points;
} /* PlayerProfile::get_points */ 


int PlayerProfile::get_points(char *lp_id)
{
    for(PlayerProfileLPProgress *lpp:m_progress) {
		if (strcmp(lpp->m_levelpack_id,lp_id)==0) return lpp->m_points;
	} // while 

	return 0;
} /* PlayerProfile::get_points */ 



bool PlayerProfile::has_ship(int ship)
{
    if (strcmp(m_name,"mazeofgalious")==0) return true;  // cheat code
    
    for(int s:m_ships) {
		if (s==ship) return true;
	} // while 
	return false;
} /* PlayerProfile::has_ship */ 


int PlayerProfile::number_of_times_completed(char *lpid,int level)
{
	int times=0;
    for(PlayerProfileLPProgress *lpp:m_progress) {
		if (strcmp(lpp->m_levelpack_id,lpid)==0) {
            for(PlayerProfileLevelResult *r:lpp->m_results) {
				if (r->m_level==level) times++;
			} // while 
		} // if 
	} // times 

	return times;
} /* PlayerProfile::number_of_times_completed */ 


int PlayerProfile::get_accumulated_time(void)
{
	int time=0;

    for(PlayerProfileLPProgress *lpp:m_progress) {
        for(PlayerProfileLevelResult *r:lpp->m_results) {
			time+=r->m_best_time;
		} // while 
	} // while 

	return time;
} /* PlayerProfile::get_accumulated_time */ 


int PlayerProfile::get_accumulated_time(char *lp_id)
{
	int time=0;
    for(PlayerProfileLPProgress *lpp:m_progress) {
        if (strcmp(lpp->m_levelpack_id,lp_id)==0) {
            for(PlayerProfileLevelResult *r:lpp->m_results) {
				time+=r->m_best_time;
			} // while 
		} // if 
	} // while 

	return time;
} /* PlayerProfile::get_accumulated_time */ 


void PlayerProfile::set_last_levelpack(char *id)
{
	if (m_last_levelpack!=0) delete []m_last_levelpack;
	m_last_levelpack = new char[strlen(id)+1];
	strcpy(m_last_levelpack,id);
}


char *PlayerProfile::get_last_levelpack()
{
	return m_last_levelpack;
}

