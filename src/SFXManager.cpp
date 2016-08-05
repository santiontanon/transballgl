#ifdef _WIN32
#include <windows.h>
#else
#include "unistd.h"
#include "sys/stat.h"
#include "sys/types.h"
#include <dirent.h>
#endif

#include "debug.h"

#include <stdio.h>
#include <stdlib.h>
#include "math.h"

#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_mixer.h"
#else
#include "SDL_mixer.h"
#endif
#include "auxiliar.h"

#include <algorithm>
#include <vector>
#include "sound.h"
#include "keyboardstate.h"
#include "Symbol.h"

#include "SFXManager.h"

extern bool sound;

SFXManagerNode::~SFXManagerNode()
{
    if (m_name!=0) delete m_name;
    m_name=0;
    if (m_sfx!=0) Mix_FreeChunk(m_sfx);
    m_sfx=0;
} /* SFXManagerNode::~SFXManagerNode */


SFXManager::SFXManager()
{
    m_hash=new std::vector<SFXManagerNode *>[SYMBOL_HASH_SIZE];
    
#ifdef __DEBUG_MESSAGES
    output_debug_message("SFXManager created.\n");
#endif
    
} /* SFXManager::SFXManager */


SFXManager::~SFXManager()
{
    m_already_played.clear();
    for(int i = 0;i<SYMBOL_HASH_SIZE;i++) {
        for(SFXManagerNode *s:m_hash[i]) delete s;
        m_hash[i].clear();
    }
    delete []m_hash;
    m_hash=0;
} /* SFXManager::~SFXManager */


Mix_Chunk *SFXManager::get(const char *name)
{
    if (!sound) return 0;
    Symbol *s=new Symbol(name);
    Mix_Chunk *t=get(s);
    delete s;
    return t;
} /* SFXManager::get */


Mix_Chunk *SFXManager::get(Symbol *name)
{
    if (!sound) return 0;
    int h=name->hash_function();
    
    for(SFXManagerNode *n:m_hash[h]) {
        if (n->m_name->cmp(name)) {
            if (std::find(m_already_played.begin(), m_already_played.end(), n) == m_already_played.end()) {
                m_already_played.push_back(n);
                return n->m_sfx;
            } else {
                return 0;
            } // if
        } // if
    } /* while */
    
    // load the sfx:
    {
        char filename[256];
        char *extensionLess = 0;
        Mix_Chunk *t;
        
        sprintf(filename,"%s",name->get());
        extensionLess = remove_extension_copy(filename);

        t = Mix_LoadWAV(filename);
        if (t==0) output_debug_message("SFXManager::get: failed to load file \"%s\"\n",name->get());
        SFXManagerNode *n=new SFXManagerNode();
        n->m_sfx=t;
        n->m_name=new Symbol(extensionLess);
        m_hash[h].push_back(n);
        delete []extensionLess;
        
        m_already_played.push_back(n);
        return t;
    }
} /* SFXManager::get */


// helper function for the cache
// filter the dir specified for only ogg/wav/mp3
bool is_sfx(const char *filename) {
    if (strcmp(".ogg", filename+strlen(filename)-4) == 0 ||
        strcmp(".wav", filename+strlen(filename)-4) == 0 ||
        strcmp(".mp3", filename+strlen(filename)-4) == 0) {
        return true;
    } else {
        return false;
    }
}

// helper function for the cache;
// remove file extensions
char *SFXManager::remove_extension_copy(const char *a_filename)
{
    char *filename = new char[strlen(a_filename)+1];
    strcpy(filename, a_filename);
#ifdef _WIN32
    char *p = strrchr(filename, '\\');
#else
    char *p = strrchr(filename, '/');
#endif
    if (p != NULL) {
        p = strrchr(p+1, '.');
    } else {
        p = strrchr(filename, '.');
    }
    if (p != NULL) {
        *p = '\0';
    }
    return filename;
}

// read specified dir for sfx and cache them
void SFXManager::cache(const char *path)
{
#ifdef _WIN32
    WIN32_FIND_DATA finfo;
    HANDLE h;
    char pattern[256], tmp[256];
    sprintf(pattern, "%s/*.*", path);
    
    h = FindFirstFile(pattern, &finfo);
    if (h != INVALID_HANDLE_VALUE) {
        strcpy(tmp, finfo.cFileName);
        while(FindNextFile(h, &finfo) == TRUE) {
            if (is_sfx(finfo.cFileName))
            {
                sprintf(tmp, "%s/%s", path, finfo.cFileName);
                get(tmp);
            }
        }
    }
#else
    char tmp[512];
    DIR *dp;
    struct dirent *ep;
    dp = opendir ((const char *)path);
    if (dp != NULL) {
        while ((ep = readdir (dp))) {
            if (is_sfx(ep->d_name)) 
            {					 						
                sprintf(tmp, "%s/%s", path, ep->d_name);
                get(tmp);
            }
        }
        (void) closedir (dp);
    }
#endif
}

void SFXManager::next_cycle(void)
{
    m_already_played.clear();
} /* SFXManager::next_cycle */ 

