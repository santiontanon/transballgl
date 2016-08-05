#include "assert.h"

#include "string.h"
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include <vector>

#include "SDL.h"

#include "debug.h"
#include "keyboardstate.h"

#define JOYSTICK_SIZE	12


KEYBOARDSTATE::KEYBOARDSTATE(int repeat_period)
{
    int i;
    
    n_joysticks=SDL_NumJoysticks();
    if (n_joysticks>0) {
        joysticks=new SDL_Joystick *[n_joysticks];
        for(i=0;i<n_joysticks;i++) joysticks[i]=SDL_JoystickOpen(i);
    } else {
        joysticks=0;
    } // if
    
    k_size=SDL_NUM_SCANCODES+n_joysticks*JOYSTICK_SIZE;
    joystick_0_pos=SDL_NUM_SCANCODES;
    joystick_size=JOYSTICK_SIZE;
    output_debug_message("k_size = %i\n",k_size);
    
    keyboard=new unsigned char[k_size];
    old_keyboard=new unsigned char[k_size];
    time_pressed=new int[k_size];
    
    for(i=0;i<k_size;i++) {
        keyboard[i]=0;
        old_keyboard[i]=0;
        time_pressed[i]=0;
    } // for
    
    m_repeat_period = repeat_period;
} /* KEYBOARDSTATE::KEYBOARDSTATE */


KEYBOARDSTATE::~KEYBOARDSTATE(void)
{
    int i;
    
    delete []keyboard;
    keyboard=0;
    delete []old_keyboard;
    old_keyboard=0;
    
    for(i=0;i<n_joysticks;i++) {
        SDL_JoystickClose(joysticks[i]);
        joysticks[i]=0;
    } // for
    delete []joysticks;
    delete []time_pressed;
    
    
} /* KEYBOARDSTATE::~KEYBOARDSTATE */


void KEYBOARDSTATE::cycle(void)
{
    int i,j,v;
    int numkeys = SDL_NUM_SCANCODES;
    SDL_PumpEvents();
    unsigned char *k = (unsigned char *)SDL_GetKeyboardState(&numkeys);
    
    /* Update keyboard: */
    for(i=0;i<numkeys && i<SDL_NUM_SCANCODES;i++) {
        old_keyboard[i]=keyboard[i];
        keyboard[i]=k[i];
        //        if (keyboard[i]!=old_keyboard[i]) output_debug_message("state changed %i -> %i\n",i,keyboard[i]);
        if (k[i]==0) time_pressed[i]=0;
        else time_pressed[i]++;
    } // for
    
    if (n_joysticks>0) {
        SDL_JoystickUpdate();
        
        /* Update joysticks: */
        for(i=0;i<n_joysticks;i++) {
            for(j=0;j<joystick_size;j++) old_keyboard[joystick_0_pos+i*joystick_size+j]=keyboard[joystick_0_pos+i*joystick_size+j];
            v=SDL_JoystickGetAxis(joysticks[i],0);
            if (v<-16000) keyboard[joystick_0_pos+i*joystick_size]=1;
            else keyboard[joystick_0_pos+i*joystick_size]=0;
            if (v>16000) keyboard[joystick_0_pos+i*joystick_size+1]=1;
            else keyboard[joystick_0_pos+i*joystick_size+1]=0;
            v=SDL_JoystickGetAxis(joysticks[i],1);
            if (v<-16000) keyboard[joystick_0_pos+i*joystick_size+2]=1;
            else keyboard[joystick_0_pos+i*joystick_size+2]=0;
            if (v>16000) keyboard[joystick_0_pos+i*joystick_size+3]=1;
            else keyboard[joystick_0_pos+i*joystick_size+3]=0;
            
            for(j=0;j<8 && j<SDL_JoystickNumButtons(joysticks[i]);j++) {
                keyboard[joystick_0_pos+i*joystick_size+4+j]=SDL_JoystickGetButton(joysticks[i],j);
            } // for
        } // for
    } // if
    
} /* KEYBOARDSTATE::cycle */


void KEYBOARDSTATE::copy(KEYBOARDSTATE *lvk)
{
    int i;
    LVK_ID=lvk->LVK_ID;
    for(i=0;i<k_size && i<lvk->k_size;i++) {
        keyboard[i]=lvk->keyboard[i];
        old_keyboard[i]=lvk->old_keyboard[i];
    } // for
    
    for(SDL_Keysym ks:lvk->keyevents) {
        keyevents.push_back(ks);
    }
    for(SDL_TextInputEvent ks:lvk->textevents) {
        textevents.push_back(ks);
    }
} /* KEYBOARDSTATE::copy */


bool KEYBOARDSTATE::key_press(int key)
{
    if (old_keyboard[key]==0 && keyboard[key]==1) return true;
    if (keyboard[key]==1 && time_pressed[key]>20 && (time_pressed[key]%2)==0) return true;
    return false;
} /* KEYBOARDSTATE::key_press */


void KEYBOARDSTATE::consume_key_press(int key)
{
    old_keyboard[key] = keyboard[key];
}


void KEYBOARDSTATE::clearEvents()
{
    keyevents.clear();
    textevents.clear();
}

