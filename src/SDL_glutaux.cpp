#ifdef _WIN32
#include "windows.h"
#endif

#include <string>
#include <vector>
#include <algorithm>


//#include <glm/glm.hpp>
//#include <glm/ext.hpp>
#include "SDL.h"
#ifdef __EMSCRIPTEN__
#include "SDL/SDL_opengl.h"
#endif

#ifdef __APPLE__
#include "OpenGL/gl.h"
#include "OpenGL/glu.h"
#else
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#endif


#include "stdio.h"
#include "math.h"

#include "auxiliar.h"
#include "debug.h"

#include "GLTile.h"
#include "SDL_glutaux.h"


// these global variables can be used to avoid having to pass programIDs all over the code
GLuint programID = 0;

GLuint PVMatrixID = 0;
GLuint MMatrixID = 0;
GLuint inColorID = 0;
GLuint useTextureID = 0;

GLuint lastTexture = 0;

GLuint LoadShaders()
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    std::string VertexShaderCode =
    "attribute vec3 vPosition;\n"
    "attribute vec2 UV;\n"
    "varying vec2 texture_coordinate;\n"
    "uniform mat4 PV;\n"
    "uniform mat4 M;\n"
    "void main()\n"
    "{\n"
    "  gl_Position = PV * M * vec4(vPosition, 1.0);\n"
    "  texture_coordinate = UV;\n"
    "}";
    
    std::string FragmentShaderCode =
#ifdef __EMSCRIPTEN__
    "precision mediump float;\n"
#endif
    "varying vec2 texture_coordinate;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec4 inColor;\n"
    "uniform int useTexture;\n"
    "void main()\n"
    "{\n"
    "  if (useTexture==2) {\n"  // useTexture = 2 color rgb*tx and color a*ty
    "    vec4 c = texture2D(textureSampler, texture_coordinate);\n"
    "    gl_FragColor = vec4(inColor.r*texture_coordinate.x, inColor.g*texture_coordinate.x, inColor.b*texture_coordinate.x, inColor.a*texture_coordinate.y);\n"
    "  } else if (useTexture==1) {\n"   // useTexture = 1 is normal texture
    "    vec4 c = texture2D(textureSampler, texture_coordinate);\n"
    "    gl_FragColor = vec4(inColor.r*c.r, inColor.g*c.g, inColor.b*c.b, inColor.a*c.a);\n"
    "  } else {\n"  // useTexture = 0 is uniform color
    "    gl_FragColor = inColor;\n"
    "  }\n"
    "}                                         \n";
    
    GLint Result = GL_FALSE;
    int InfoLogLength;
    
    // Compile Vertex Shader
    output_debug_message("Compiling vertex shader\n");
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);
    
    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> VertexShaderErrorMessage(std::max(InfoLogLength, int(1)));
    glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
    output_debug_message("%s\n", &VertexShaderErrorMessage[0]);
    
    // Compile Fragment Shader
    output_debug_message("Compiling fragment shader\n");
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);
    
    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> FragmentShaderErrorMessage(std::max(InfoLogLength, int(1)));
    glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
    output_debug_message("%s\n", &FragmentShaderErrorMessage[0]);
    
    // Link the program
    output_debug_message("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);
    
    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    std::vector<char> ProgramErrorMessage( std::max(InfoLogLength, int(1)) );
    glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
    output_debug_message("%s\n", &ProgramErrorMessage[0]);
    
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);
    
    return ProgramID;
}



void setTexture(GLuint tex)
{
    if (tex!=lastTexture) {
        // if lastTexture = 0 it means it's the first time we use a texture:
        glBindTexture(GL_TEXTURE_2D, tex);
        lastTexture = tex;
    }
}


int nearest_2pow(int n)
{
	int res=2;

	for(res=2;res<2048;res*=2) {
		if (res>=n) return res;
	} // for 

	return res;
} /* nearest_2pow */ 


GLuint  createTexture(SDL_Surface *sfc,float *tx,float *ty)
{
    unsigned int tname=0;
    int szx,szy;
    
    if (sfc!=0) {
        
        SDL_Surface *sfc2=0;
        
        szx=nearest_2pow(sfc->w);
        szy=nearest_2pow(sfc->h);
        *tx=(sfc->w)/float(szx);
        *ty=(sfc->h)/float(szy);
        sfc2=SDL_CreateRGBSurface(SDL_SWSURFACE,szx,szy,32,RMASK,GMASK,BMASK,AMASK);
        SDL_BlitSurface(sfc,0,sfc2,0);
        
        setTexture(0);
        glGenTextures(1,&tname);
        //glPixelStorei(GL_UNPACK_ALIGNMENT,4);
        setTexture(tname);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
        //      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
        //      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
        //      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        //      glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,szx,szy,0,GL_RGBA,GL_UNSIGNED_BYTE,sfc2->pixels);
        SDL_FreeSurface(sfc2);
        setTexture(0);
        
        if (!glIsTexture(tname)) {
            output_debug_message("createTexture: %i is not a texture!! Error %i\n", tname, glGetError());
            exit(1);
        }
        
        //        output_debug_message("createTexture: %i [%i,%i] -> [%i,%i]\n",tname,sfc->w,sfc->h,szx,szy);
    } else {
        return 0;
    } // if
    
    return tname;
} /* createTexture */ 


GLuint  createTextureFromScreen(int x,int y,int dx,int dy,float *tx,float *ty)
{
	unsigned int tname=0;
	int szx,szy;

	szx=nearest_2pow(dx);
	szy=nearest_2pow(dy);
	*tx=(dx)/float(szx);
	*ty=(dy)/float(szy);

    setTexture(0);
    glGenTextures(1,&tname);
    setTexture(tname);
    glPixelStorei(GL_UNPACK_ALIGNMENT,tname);
	glBindTexture(GL_TEXTURE_2D,tname);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);		 

	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,x,y,dx,dy);
    setTexture(0);
    
	return tname;
} /* createTextureFromScreen */ 


bool drawQuad_called = false;
GLuint drawQuad_VertexArrayID = 0;
GLuint drawQuad_Vertexbuffer = 0;
GLfloat drawQuad_buffer_data[] = {
    0,0,0, 0,0,
    0,0,0, 1,0,
    0,0,0, 1,1,
    0,0,0, 0,1,
};


void drawQuad(float x, float y, float w, float h, float r, float g, float b, float a)
{
    drawQuad_buffer_data[0] = x;
    drawQuad_buffer_data[1] = y;
    
    drawQuad_buffer_data[5] = x+w;
    drawQuad_buffer_data[6] = y;
    
    drawQuad_buffer_data[10] = x+w;
    drawQuad_buffer_data[11] = y+h;
    
    drawQuad_buffer_data[15] = x;
    drawQuad_buffer_data[16] = y+h;
    if (!drawQuad_called) {
        glGenVertexArrays(1, &drawQuad_VertexArrayID);
        glBindVertexArray(drawQuad_VertexArrayID);
        glGenBuffers(1, &drawQuad_Vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, drawQuad_Vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*4, drawQuad_buffer_data, GL_DYNAMIC_DRAW);
        drawQuad_called = true;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, drawQuad_Vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*4, drawQuad_buffer_data);
    }
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    int loc1 = glGetAttribLocation(programID, "vPosition");
    int loc2 = glGetAttribLocation(programID, "UV");
    glUniform4f(inColorID, r, g, b, a);
    glUniform1i(useTextureID, 0);
    glBindVertexArray(drawQuad_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, drawQuad_Vertexbuffer);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}




bool drawCircle_called = false;
GLuint drawCircle_VertexArrayID = 0;
GLuint drawCircle_Vertexbuffer = 0;
GLfloat drawCircle_buffer_data[5*12*3];

void drawCircle(float x, float y, float z, float radius, float r, float g, float b, float a)
{
    drawCircle(glm::mat4(1.0f), x,y,z,radius,r,g,b,a);
}

void drawCircle(glm::mat4 transform, float x, float y, float z, float radius, float r, float g, float b, float a)
{
    for (int i = 0;i<12;i++) {
        int angle = i*30;
        drawCircle_buffer_data[i*3*5] = x;
        drawCircle_buffer_data[i*3*5+1] = y;
        drawCircle_buffer_data[i*3*5+2] = z;
        drawCircle_buffer_data[i*3*5+3] = 0;
        drawCircle_buffer_data[i*3*5+4] = 0;

        drawCircle_buffer_data[i*3*5+5] = x+(cos_degree(angle)*radius);
        drawCircle_buffer_data[i*3*5+6] = y+(sin_degree(angle)*radius);
        drawCircle_buffer_data[i*3*5+7] = z;
        drawCircle_buffer_data[i*3*5+8] = 0;
        drawCircle_buffer_data[i*3*5+9] = 0;

        drawCircle_buffer_data[i*3*5+10] = x+(cos_degree((angle+30)%360)*radius);
        drawCircle_buffer_data[i*3*5+11] = y+(sin_degree((angle+30)%360)*radius);
        drawCircle_buffer_data[i*3*5+12] = z;
        drawCircle_buffer_data[i*3*5+13] = 0;
        drawCircle_buffer_data[i*3*5+14] = 0;
    } // for
    
    if (!drawCircle_called) {
        glGenVertexArrays(1, &drawCircle_VertexArrayID);
        glBindVertexArray(drawCircle_VertexArrayID);
        glGenBuffers(1, &drawCircle_Vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, drawCircle_Vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*12*3, drawCircle_buffer_data, GL_DYNAMIC_DRAW);
        drawCircle_called = true;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, drawCircle_Vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*12*3, drawCircle_buffer_data);
    }
    
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &transform[0][0]);
    int loc1 = glGetAttribLocation(programID, "vPosition");
    int loc2 = glGetAttribLocation(programID, "UV");
    glUniform4f(inColorID, r, g, b, a);
    glUniform1i(useTextureID, 0);
    glBindVertexArray(drawCircle_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, drawCircle_Vertexbuffer);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 12*3);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
 
}


bool drawPixel_called = false;
GLuint drawPixel_VertexArrayID = 0;
GLuint drawPixel_Vertexbuffer = 0;
GLfloat drawPixel_buffer_data[] = {
    0,0,0, 0,0,
};


void drawPixel(float x, float y, float z, float r, float g, float b, float a)
{
    drawPixel_buffer_data[0] = x;
    drawPixel_buffer_data[1] = y;
    drawPixel_buffer_data[2] = z;

    if (!drawPixel_called) {
        glGenVertexArrays(1, &drawPixel_VertexArrayID);
        glBindVertexArray(drawPixel_VertexArrayID);
        glGenBuffers(1, &drawPixel_Vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, drawPixel_Vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5, drawPixel_buffer_data, GL_DYNAMIC_DRAW);
        drawPixel_called = true;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, drawPixel_Vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5, drawPixel_buffer_data);
    }
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    int loc1 = glGetAttribLocation(programID, "vPosition");
    int loc2 = glGetAttribLocation(programID, "UV");
    glUniform4f(inColorID, r, g, b, a);
    glUniform1i(useTextureID, 0);
    glBindVertexArray(drawPixel_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, drawPixel_Vertexbuffer);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_POINTS, 0, 1);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


bool drawLine_called = false;
GLuint drawLine_VertexArrayID = 0;
GLuint drawLine_Vertexbuffer = 0;
GLfloat drawLine_buffer_data[] = {
    0,0,0, 0,0,
    0,0,0, 0,0,
};


void drawLine(float x0, float y0, float x1, float y1, float r, float g, float b, float a)
{
    drawLine_buffer_data[0] = x0;
    drawLine_buffer_data[1] = y0;
    drawLine_buffer_data[2] = 0;
    drawLine_buffer_data[5] = x1;
    drawLine_buffer_data[6] = y1;
    drawLine_buffer_data[7] = 0;
    
    if (!drawLine_called) {
        glGenVertexArrays(1, &drawLine_VertexArrayID);
        glBindVertexArray(drawLine_VertexArrayID);
        glGenBuffers(1, &drawLine_Vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, drawLine_Vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*2, drawLine_buffer_data, GL_DYNAMIC_DRAW);
        drawLine_called = true;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, drawLine_Vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*2, drawLine_buffer_data);
    }
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    int loc1 = glGetAttribLocation(programID, "vPosition");
    int loc2 = glGetAttribLocation(programID, "UV");
    glUniform4f(inColorID, r, g, b, a);
    glUniform1i(useTextureID, 0);
    glBindVertexArray(drawLine_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, drawLine_Vertexbuffer);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_LINES, 0, 2);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


bool drawTriangle_called = false;
GLuint drawTriangle_VertexArrayID = 0;
GLuint drawTriangle_Vertexbuffer = 0;
GLfloat drawTriangle_buffer_data[] = {
    0,0,0, 0,0,
    0,0,0, 0,0,
    0,0,0, 0,0,
};


void drawTriangle(float x0, float y0,
                  float x1, float y1,
                  float x2, float y2,
                  float r, float g, float b, float a)
{
    drawTriangle_buffer_data[0] = x0;
    drawTriangle_buffer_data[1] = y0;
    drawTriangle_buffer_data[5] = x1;
    drawTriangle_buffer_data[6] = y1;
    drawTriangle_buffer_data[10] = x2;
    drawTriangle_buffer_data[11] = y2;
    
    if (!drawTriangle_called) {
        glGenVertexArrays(1, &drawTriangle_VertexArrayID);
        glBindVertexArray(drawTriangle_VertexArrayID);
        glGenBuffers(1, &drawTriangle_Vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, drawTriangle_Vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*5*3, drawTriangle_buffer_data, GL_DYNAMIC_DRAW);
        drawTriangle_called = true;
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, drawTriangle_Vertexbuffer);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat)*5*3, drawTriangle_buffer_data);
    }
    
    glm::mat4 tmp = glm::mat4(1.0f);
    glUniformMatrix4fv(MMatrixID, 1, GL_FALSE, &tmp[0][0]);
    int loc1 = glGetAttribLocation(programID, "vPosition");
    int loc2 = glGetAttribLocation(programID, "UV");
    glUniform4f(inColorID, r, g, b, a);
    glUniform1i(useTextureID, 0);
    glBindVertexArray(drawTriangle_VertexArrayID);
    glBindBuffer(GL_ARRAY_BUFFER, drawTriangle_Vertexbuffer);
    glEnableVertexAttribArray(loc1);
    glEnableVertexAttribArray(loc2);
    glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, (void*)0);
    glVertexAttribPointer(loc2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, (GLvoid*)(sizeof(float)*3));
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(loc1);
    glDisableVertexAttribArray(loc2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void clearDrawPrimitivesData()
{
    if (drawQuad_called) {
        glDeleteBuffers(1,&drawQuad_Vertexbuffer);
#ifdef __APPLE__
        glDeleteVertexArraysAPPLE(1,&drawQuad_VertexArrayID);
#else
        glDeleteVertexArrays(1,&drawQuad_VertexArrayID);
#endif
        drawQuad_called = false;
    }
}
