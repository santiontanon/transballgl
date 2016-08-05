#ifndef __VIRTUAL_CONTROLLER
#define __VIRTUAL_CONTROLLER

#define VC_UP		0
#define VC_RIGHT	1
#define VC_DOWN		2
#define VC_LEFT		3

class VirtualController {
public:

	VirtualController(void);
	VirtualController(VirtualController *vc);

	void copy(VirtualController *vc);
	void copy_current(VirtualController *vc);

	bool m_joystick[4];
	bool m_button[2];
	bool m_menu;

	bool m_old_joystick[4];
	bool m_old_button[2];
	bool m_old_menu;

	void reset(void) {
		m_old_joystick[0]=m_joystick[0]=false;
		m_old_joystick[1]=m_joystick[1]=false;
		m_old_joystick[2]=m_joystick[2]=false;
		m_old_joystick[3]=m_joystick[3]=false;

		m_old_button[0]=m_button[0]=false;
		m_old_button[1]=m_button[1]=false;

		m_old_menu=m_menu=false;
	} // reet

	void new_cycle(void) {
		m_old_joystick[0]=m_joystick[0];
		m_old_joystick[1]=m_joystick[1];
		m_old_joystick[2]=m_joystick[2];
		m_old_joystick[3]=m_joystick[3];

		m_old_button[0]=m_button[0];
		m_old_button[1]=m_button[1];

		m_old_menu=m_menu;
	} // new_cycle
};

#endif

