#ifndef TGL_INTERFACE
#define TGL_INTERFACE

#include<list>

class TGLInterfaceElement {
public:
	TGLInterfaceElement();
	virtual ~TGLInterfaceElement();
    virtual void createOpenGLBuffers(void);
    
	virtual bool mouse_over(int mousex,int mousey);
	virtual bool check_status(int mousex,int mousey,int button,int button_status,class KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);
    virtual std::list<TGLInterfaceElement *> getChildren(void);
    
	int m_ID;
	bool m_modal;	/* If any element is modal, only him has the control until it is destroyed (the rest of the interface is faded) */ 
	bool m_enabled;
	bool m_active;	/* This indictes wether the component is active or passive, */ 
					/* e.g.: TGLText and TGLframe are passive					*/ 
	bool m_to_be_deleted;

	float m_x,m_y,m_dx,m_dy;
    
    int vPositionLocation, UVLocation;
};


class TGLText : public TGLInterfaceElement {
public:

	TGLText(const char *text,TTF_Font *font,float x,float y,bool centered);
	TGLText(const char *text,TTF_Font *font,float x,float y,bool centered,int ID);
	virtual ~TGLText();

	virtual void draw(float alpha);
	virtual void draw(void);

	void set_text(char *text);

	bool m_centered;
	char *m_text;
	TTF_Font *m_font;
};


class TGLbutton : public TGLInterfaceElement {
public:

	TGLbutton(const char *text,TTF_Font *font,float x,float y,float dx,float dy,int ID);
	TGLbutton(GLTile *icon,float x,float y,float dx,float dy,int ID);
	virtual ~TGLbutton();
    virtual void createOpenGLBuffers(void);
    
	virtual bool mouse_over(int mousex,int mousey);
	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	char *m_text;
	TTF_Font *m_font;
	int m_status;
	GLTile *m_tile;
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};


class TGLbuttonTransparent : public TGLbutton {
public:

	TGLbuttonTransparent(const char *text,TTF_Font *font,float x,float y,float dx,float dy,int ID);
    TGLbuttonTransparent(const char *text,TTF_Font *font,float x,float y,float dx,float dy,int ID, float r, float g, float b, float a);
    
	virtual void draw(float alpha);
 
    float r, g, b, a;
};


class TGLquad : public TGLInterfaceElement {
public:
    
    TGLquad(float x,float y,float dx,float dy, float r, float g, float b, float a);
    virtual ~TGLquad();
    virtual void createOpenGLBuffers(void);
    
    virtual void draw(float alpha);
    virtual void draw(void);
    
    float r, g, b, a;
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};



class TGLframe : public TGLInterfaceElement {
public:

	TGLframe(float x,float y,float dx,float dy);
	virtual ~TGLframe();
    virtual void createOpenGLBuffers(void);

	virtual void draw(float alpha);
	virtual void draw(void);

    GLfloat cursor_dx, cursor_dy;
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};


class TGLTextInputFrame : public TGLframe {
public:

	TGLTextInputFrame(const char *initial_text,int max_characters,TTF_Font *font,float x,float y,float dx,float dy,int ID);
	virtual ~TGLTextInputFrame();
    virtual void createOpenGLBuffers(void);
    
	virtual bool mouse_over(int mousex,int mousey);
	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	TTF_Font *m_font;
	char *m_editing;
	int m_max_characters;
	unsigned int m_editing_position;
	bool m_focus;
	int m_cycle;
	bool m_change_in_last_cycle;
    
    GLuint VertexArrayID2;
    GLuint vertexbuffer2;
};


class TGLslider : public TGLInterfaceElement {
public:

	TGLslider(float x,float y,float dx,float dy,float slider_dx,float slider_dy,int ID);
	virtual ~TGLslider();
    virtual void createOpenGLBuffers(void);
    
	virtual bool mouse_over(int mousex,int mousey);
	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	float m_slider_dx,m_slider_dy;
	float m_value;
    
    GLuint VertexArrayID, VertexArrayID2;
    GLuint vertexbuffer, vertexbuffer2;
};


class TGLBrowser : public TGLInterfaceElement {
public:

	TGLBrowser(TTF_Font *font,float x,float y,float dx,float dy,int ID);
	virtual ~TGLBrowser();

	void clear(void);
	void addEntry(char *);
	char *getEntry(int i);
	void deleteEntry(int i);
	void setSelected(int i);
	int getSelected(void);
	int getNEntries();

	virtual bool mouse_over(int mousex,int mousey);
	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	TTF_Font *m_font;
	std::list<char *> m_entries;
	float m_slider_pos,m_slider_height;
	int m_selected,m_mouse_over;
	int m_old_mouse_x,m_old_mouse_y;

};


class TGLTileBrowser : public TGLInterfaceElement {
public:

	TGLTileBrowser(float x,float y,float dx,float dy,int ID);
	virtual ~TGLTileBrowser();

	void clear(void);
	void addEntry(GLTile *tile);
	GLTile *getEntry(int i);
	void deleteEntry(int i);
	void setSelected(int i);
	int getSelected(void);
	int getNEntries();
	
	float scale(GLTile *t);

	virtual bool mouse_over(int mousex,int mousey);
	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	std::list<GLTile *> m_entries;
	float m_slider_pos,m_slider_height;
	int m_selected,m_mouse_over;

};



class TGLConfirmation : public TGLInterfaceElement {
public:

	// Separate the different lines of the message with '\n' characters:
	TGLConfirmation(const char *message,TTF_Font *font,float x,float y,int ID,bool cancelOption);
	virtual ~TGLConfirmation();
    virtual void createOpenGLBuffers(void);
    
	virtual bool check_status(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	virtual void draw(float alpha);
	virtual void draw(void);

	TTF_Font *m_font;
	std::list<char *> m_message_lines;
	int m_state,m_cycle;
	TGLbutton *m_ok_button,*m_cancel_button;
    
    GLuint VertexArrayID;
    GLuint vertexbuffer;
};



class TGLinterface {
public:
    static std::list<TGLInterfaceElement *> m_elements;

	static void add_element(TGLInterfaceElement *b);
	static void remove_element(TGLInterfaceElement *b);
	static void remove_element(int ID);
	static void substitute_element(TGLInterfaceElement *old,TGLInterfaceElement *n);
	static void reset(void);
	static bool mouse_over_element(int mousex, int mousey);
	static int update_state(int mousex,int mousey,int button,int button_status,KEYBOARDSTATE *k);
	static void draw(float alpha);
	static void draw(void);
	static TGLInterfaceElement *get(int ID);
	static void enable(int ID);
	static void disable(int ID);
	static bool is_enabled(int ID);

	static void clear_print_cache(void);
    static GLTile *get_text_tile(const char *text,TTF_Font *font);
    static void print_left(const char *text,TTF_Font *font,float x,float y);
	static void print_center(const char *text,TTF_Font *font,float x,float y);
	static void print_left(const char *text,TTF_Font *font,float x,float y,float r,float g,float b,float a);
	static void print_center(const char *text,TTF_Font *font,float x,float y,float r,float g,float b,float a);

	// Centered in both X and Y:
	static void print_centered(const char *text,TTF_Font *font,float x,float y,float r,float g,float b,float a,float angle,float scale);
};

#endif