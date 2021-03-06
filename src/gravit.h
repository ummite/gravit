/*

This file is part of
Gravit - A gravity simulator
Copyright 2003-2014 Gravit Development Team: 2003-2014 Gerald Kaszuba / 2011-2014 Frank Moehle

Gravit is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Gravit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Gravit; if not, write to the Free Software
Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#ifndef GRAVIT_H_
#define GRAVIT_H_

#define GRAVIT_VERSION "Gravit 0.5.1"
#define GRAVIT_COPYRIGHT "Copyright 2003-2014 Gravit Development Team"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


// a few gcc-specific attributes
#ifdef __GNUC__
  #if __GNUC__ >= 4 && __GNUC_MINOR__ >= 3
    #define HOT __attribute__((hot))
  #else
    #define HOT
  #endif

  #define CONST_F __attribute__((const))
  #define PURE_F  __attribute__((pure))
#else
  #define HOT
  #define CONST_F
  #define PURE_F
#endif


// normally /etc
#ifndef SYSCONFDIR
#define SYSCONFDIR "."
#endif

// DATA_DIR is normally /usr/share/gravit/data or /usr/local/share/gravit/data
// no -DDATA_DIR="path" assume we're working in the current directory
#ifndef DATA_DIR
#define DATA_DIR ""
#endif

#define SPAWNDIR DATA_DIR "spawn"
#define MISCDIR DATA_DIR "data"

#if defined(WIN32) || (!defined(HAVE_CONFIG_H) && !defined(__MACH__))
#   define CONFIG_PATH "cfg/"
#   define CONFIG_FILE "cfg/gravit.cfg"
#   define SCREENSAVER_FILE "cfg/screensaver.cfg"
#   define SCREENSHOT_PATH "screenshots"
#   define SAVE_PATH "save"
#else
#   define CONFIG_PATH ""
#   define CONFIG_FILE "gravit.cfg"
#   define SCREENSAVER_FILE "screensaver.cfg"
#   ifdef __MACH__
#       define SCREENSHOT_PATH va("%s/Library/Application Support/com.slowchop.gravit/screenshots", getenv("HOME"))
#       define SAVE_PATH va("%s/Library/Application Support/com.slowchop.gravit/save", getenv("HOME"))
#   else
#       define SCREENSHOT_PATH va("%s/.gravit/screenshots", getenv("HOME"))
#       define SAVE_PATH va("%s/.gravit/save", getenv("HOME"))
#   endif
#endif

// #define NO_GUI

#define MAX_THREADS 64

#ifdef WIN32

    #define WIN32SCREENSAVER
    #define WINVER 0x0501

    #include <windows.h>
    #include <conio.h>
    #include <stdio.h>

    #ifdef WIN32SCREENSAVER

        #include <scrnsave.h>
        #include <SDL_syswm.h>

    #endif
    #if defined(USE_PTHREAD) && !defined(_OPENMP)
        #include <unistd.h>
        #include <sys/timeb.h>
        #include <pthread.h>
    #endif


    #include <vfw.h>

    #define REGISTRY_KEY "Software\\Gravit"
    #define REGISTRY_NAME_PATH "path"

    // stupid hack to fix linking errors with isspace
    #undef isspace
    #define isspace(x) (x == 32)

    #ifndef __GNUC__
        // stop conversion from 'double ' to 'float ', possible loss of data
        #pragma warning ( disable : 4244 )
        // silence warning C4996: 'strdup': The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _strdup.
        #define strdup(x) _strdup(x)
        // silence warning C4055: typecast from data pointer 'void *' to function pointer 'FPglPointParameterfARB'
        #pragma warning ( disable : 4055 )
#endif

#else

    #include <stdarg.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <termios.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
    #include <sys/timeb.h>
    #include <pthread.h>
    #include <dirent.h>
    #include <sys/stat.h>
    #include "dirent.h"

    // used for the opengl extensions
    #ifndef APIENTRY
        #define APIENTRY
    #endif

#endif

#if __MACH__

    #include <Foundation/Foundation.h>

#endif

#ifndef NO_GUI

    // this removes the header requirement for glext.h in gl.h
    #define GL_GLEXT_LEGACY

    #include <SDL.h>
    #include <SDL_ttf.h>
    #include <SDL_opengl.h>
    #include <SDL_image.h>

#ifndef WITHOUT_AGAR
    #include <agar/core.h>
    #include <agar/gui.h>
#endif

    // according to the OpenGL specs, glGetError should be called in a loop, until it returns GL_NO_ERROR
    #define glCheck() { GLenum er; while ((er = glGetError()) != GL_NO_ERROR) { conAdd(LERR, "glError: %s:%i %i %s", __FILE__, __LINE__, (int)er, gluErrorString(er)); } }
    #define sdlCheck() { char *er = SDL_GetError(); if ((er!=NULL) && (strlen(er)>0)) { conAdd(LERR, "SDL Error: %s:%i %s", __FILE__, __LINE__, er); SDL_ClearError(); } }
#ifndef WITHOUT_AGAR
    #define agarCheck() { const char *aer = AG_GetError(); if ((aer!=NULL) && (strlen(aer)>0)) { conAdd(LERR, "agar Error: %s:%i %s", __FILE__, __LINE__, aer); AG_SetError("%s", "");} }
#endif

#else

    // sdlkeys mappings
    #include "sdlk.h"

#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef HAVE_LUA
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
#endif

#ifndef MAX_PATH
    #define MAX_PATH 260
#endif

#define FILE_CHUNK_SIZE (1024*1024)
#define FILE_CHUNK_SIZE_SMALL 1024

#define frand(min,max) ((min) + ((float)rand() / RAND_MAX) * ((max) - (min)))
#define FRAMESIZE (sizeof(particle_t)*state.particleCount)
#define FRAMEDETAILSIZE (sizeof(particleDetail_t) * state.particleCount)
#define SAVEDETAILSIZE (sizeof(saveDetail_t) * state.particleCount)

#define getParticleCurrentFrame(i) state.particleHistory + state.particleCount * state.currentFrame + (i)
#define getParticleFirstFrame(i) state.particleHistory + (i)
#define getParticleDetail(i) state.particleDetail + (i)

/* just in case these macros don't already exist */
#ifndef FALSE
#define FALSE   0
#endif
#ifndef TRUE
#define TRUE    1
#endif

#ifndef PI
#define PI 3.14159265358979f
#endif

#define STRING_RECORD "Recording"
#define STRING_PLAY "Playing"

#define CM_MASS 0
#define CM_VEL 1
#define CM_ACC 2
#define CM_KIN 3
#define CM_MOM 4
#define CM_LAST 5

#define METHOD_PP 0
#define METHOD_OT 1

#define NBODY_METHOD METHOD_PP

#define VectorNew(a) float a[3]

#define VectorCopy(a,b) { b[0] = a[0]; b[1] = a[1]; b[2] = a[2]; }
#define VectorAdd(a,b,c) { c[0] = a[0] + b[0]; c[1] = a[1] + b[1]; c[2] = a[2] + b[2]; }
#define VectorSub(a,b,c) { c[0] = a[0] - b[0]; c[1] = a[1] - b[1]; c[2] = a[2] - b[2]; }
#define VectorMultiply(a, b, c) { c[0] = a[0] * b; c[1] = a[1] * b; c[2] = a[2] * b; }
#define VectorMultiplyAdd(a, b, c) { c[0] += a[0] * b; c[1] += a[1] * b; c[2] += a[2] * b; }
#define VectorDivide(a, b, c) { c[0] = a[0] / b; c[1] = a[1] / b; c[2] = a[2] / b; }
#define VectorZero(x) { x[0] = 0; x[1] = 0; x[2] = 0; }

#define distance2(a,b,c) {\
  float tmp;\
  c = a[0] - b[0];\
  c = c * c;\
  tmp = a[1] - b[1];\
  c += tmp * tmp;\
  tmp = a[2] - b[2];\
  c += tmp * tmp;\
}

#define distance(a,b,c) { float d; distance2(a,b,d); c = (float)sqrt((double)d); }

#ifndef Uint32
    #define Uint32 unsigned int
#endif

#ifndef Uint8
    #define Uint8 unsigned char
#endif

#ifdef WIN32
    #include <malloc.h>
    #include <memory.h>
#endif

#define CONSOLE_HISTORY 10
#define CONSOLE_LENGTH 255
#define CONSOLE_BLINK_TIME 500
#define CONSOLE_WIDTH 500
#define CONSOLE_TYPED_HISTORY 50

#define LLOW 0
#define LNORM 1
#define LERR 2
#define LHELP 3

#define TM_STANDARD 0
#define TM_HELP1 1
#define TM_HELP2 2

#define SM_RECORD 2
#define SM_PLAY 4

#define MAX_FONT_LENGTH 255

// this define is to render video in the middle of a recording
#define doVideoUpdate() \
    if (view.recordingVideoRefreshTime) { \
        if (!(state.mode & SM_RECORD) ) return; \
        if (view.lastVideoFrame + view.recordingVideoRefreshTime < getMS()) { \
            runInput(); \
            runVideo(); \
            if (!(state.mode & SM_RECORD)) return; \
        } \
    }

// this define is to render video in the middle of a recording
// without aborting (needed for OMP)
#define doVideoUpdate2() \
    if (view.recordingVideoRefreshTime) { \
        if (view.lastVideoFrame + view.recordingVideoRefreshTime < getMS()) { \
            runInput(); \
            runVideo(); \
        } \
    }

// this define is to render video in the middle of a spawn
#define doVideoUpdateInSpawn() \
    if (view.recordingVideoRefreshTime) { \
        if (((view.recordParticlesDone % 128) == 1) && (view.lastVideoFrame + (view.recordingVideoRefreshTime*2) < getMS())) { \
            runInput(); \
            runVideo(); \
        } \
        if (view.quit) return 0; \
    }

#include "command.h"

#ifndef NO_GUI
    #include "font.h"
#endif

#ifndef NO_GUI

    // function pointers for gl extensions
    typedef void (APIENTRY *FPglPointParameterfARB)(GLenum, GLfloat);
    typedef void (APIENTRY *FPglPointParameterfvARB)(GLenum, const GLfloat*);

    extern FPglPointParameterfARB glPointParameterfARB_ptr;
    extern FPglPointParameterfvARB glPointParameterfvARB_ptr;

    // gl defines
    #define GL_POINT_SIZE_MIN_ARB 0x8126
    #define GL_POINT_SIZE_MAX_ARB 0x8127
    #define GL_POINT_FADE_THRESHOLD_SIZE_ARB 0x8128
    #define GL_POINT_DISTANCE_ATTENUATION_ARB 0x8129

    #define GL_POINT_SPRITE_ARB 0x8861
    #define GL_COORD_REPLACE_ARB 0x8862

    #ifndef GL_CLAMP_TO_EDGE
        #define GL_CLAMP_TO_EDGE 0x812F
    #endif
    #ifndef GL_ALIASED_POINT_SIZE_RANGE
        #define GL_ALIASED_POINT_SIZE_RANGE 0x846D
    #endif

    typedef struct conf_s {

        int screenBPP;
        int screenW;
        int screenH;
        int screenFS;
        int screenAA;
        int flags;

        int screenWtoApply;
        int screenHtoApply;

        int supportPointSprite;
        int supportPointParameters;

        char fontFile[MAX_FONT_LENGTH];
        int fontSize;

        SDL_VideoInfo *gfxInfo;
        SDL_Surface *sdlScreen;

        int sdlStarted;
        int agarStarted;

    } video_t;

#endif

typedef struct col_s {

    float r;
    float g;
    float b;

} col_t;

// things that change per frame
typedef struct particle_s {

    VectorNew(pos);
    VectorNew(vel);

} particle_t;

// things that change less often
typedef struct particleDetail_s {

    float mass;
    VectorNew(accel);       // new acceleration
    float col[4];
    unsigned int particleSprite;

} particleDetail_t;

// particleDetail_t without temporal things
typedef struct saveDetail_s {

    float mass;
    float col[4];

} saveDetail_t;

// physics mode: CLASSIC, MODIFIED or PROPER
typedef enum {PH_CLASSIC=0, PH_MODIFIED=1, PH_PROPER=2} physics_t;

// global simulation state
typedef struct state_s {

    particle_t *particleHistory;
    particleDetail_t *particleDetail;

    int memoryAvailable;    // MB
    int memoryPercentage;   // Detect memory available and use a percentage of it

    int particleCount;
    int frame;
    int totalFrames;
    int currentFrame;
    int targetFrame;
    int historyFrames;
    int historyNFrame;

    float gbase;
    float g;

    int frameCompression;

    VectorNew(center);
    int mode;
    float massRange[2];

    int processFrameThreads;

    int particlesToSpawn;

    size_t memoryAllocated;

    int lastSave;    // last frame saved
    int autoSave;    // auto save every n frames. 0 for off.
    char *fileName; // if null dont autosave or incsave.

    int currentlySpawning;
    int restartSpawning;

    int dontExecuteDefaultScript;

    int autoRecord;    // will start recording after spawning
    int autoRecordNext; // is a value to do a cmdRecord in the main loop, it goes to 0 after doing this

#ifdef HAVE_LUA
    lua_State *lua;
#endif

    int have_old_accel;
    physics_t physics;

} state_t;

typedef struct saveInfo_s {

    int particleCount;
    int historyFrames;
    int totalFrames;
    int frame;
    int historyNFrame;
    // up to here: gravit version 0.5.0 or earlier

    float zoom;            // view parameters
    VectorNew(rot);        //   ...
    VectorNew(pos);        //   ...
    VectorNew(face);       //   ...
    VectorNew(lastCenter); //   ...
    int glow;              //   ...
    float g;               // physics parameters
    float gbase;           //   ...
    physics_t physics;     //   ...
    // up to here: gravit version 0.5.1


} saveInfo_t;

typedef struct view_s {

    Uint32 lastVideoFrame;
    Uint32 deltaVideoFrame;

    Uint32 lastRecordFrame;
    Uint32 deltaRecordFrame;

    // recordingVideoRefreshTime
    // Set this to 0 to never refresh video while rendering
    // Set this to > 0 to refresh video every Nms while rendering
    Uint32 recordingVideoRefreshTime;

    // minimum time that a single video frame stays on screen
    // Can be used to reduce CPU usage. Warning: this also slows down recording mode!
    // Any value < SDL_TIMESLICE (usually 10 ms) disables this "handbrake"
    int minVideoRefreshTime;

    // number of video display frames skipped
    Uint32 lastVideoFrameSkip;

    Uint32 firstTimeStamp;

    // timer for rendering
    Uint32 totalRenderTime;
    Uint32 timed_frames;
    Uint32 lastRenderTime;

    // set to 1 to redraw ASAP (for smooth mouse movements etc)
    int dirty;

    int quit;

    Uint8 mouseButtons[2];    // 0 now, 1 last
    int currentMousePosition[2];
    int lastMousePosition[2];
    int showCursor;

    VectorNew(rot);
    VectorNew(rotTarget);
    VectorNew(rotSpeed);

    float zoom;
    float zoomTarget;
    float zoomSpeed;

    VectorNew(lastCenter); //center of view; set by translateToCenter()
    int zoomFitAuto;

    int maxVertices;    // 0 for infinite, otherwise tailskip will double when hit

#ifndef NO_GUI
    int keys[SDLK_LAST];
#endif

    VectorNew(pos);    // todo, position of camera
    VectorNew(face);

    VectorNew(autoRotate);

    // mouse movement/keys will quit
    int screenSaver;

    float tailWidth;
    int tailLength;
    float tailOpacity;
    int tailFaded;
    int tailSkip;

    int drawAxis;

    int textMode;
    int consoleMode;

    int drawTree;
    int frameSkip;
    int frameSkipCounter;

    int drawOSD;
    int drawColourScheme;
    int drawSky;
    int drawSkyRandom;

    float fps;
    int vertices;

    int verboseMode;
    int useStdout;         // 1 to copy console output to stdout
    int silentMode;        // is set during script execution

    int blendMode;

    int screenshotIndex;    // the next available screenshot file (eg screenshot/gravit00001.bmp)
    int screenshotLoop;    // will do a screenshot every frame

    int particleColourMode;    // 0 for colour based on mass, 1 for colour based on velocity
    int particleRenderMode;    // 0 for standard GL_POINT, 1 for GL_ARB_point_sprite

    // in particleRenderMode 1, you can render a texture onto the point
    // this doesnt work in some implementations...
    int particleRenderTexture;

    float particleSizeMin;    // can be anything 0 or higher
    float particleSizeMax;    // -1 for the maximum supported. if its bigger then supported, it will simply use the supported value.

    float *colourSpectrum;        // determines what colour to draw a particle with
    int colourSpectrumSteps;

    int stereoMode;
    float stereoSeparation;
    int stereoModeCurrentBit;
    int stereoOSD;

    int recordStatus;
    int recordParticlesDone;
    int recordNodes;

    char popupTextMessage[255];
    Uint32 popupTextStart;
    Uint32 popupTextLength;
    float popupTextFadeTime;    // ms

    int autoCenter;
    
    int glow;          //0: no glow; 1-4: small stars; 4-8: big stars

#ifndef WITHOUT_AGAR
    AG_Window *playbackWindow;
    AG_Style osdStyle;
    AG_Button *playButton;
    AG_Button *recordButton;
#endif

} view_t;

typedef struct spawnVars_s {

    int minGalCount;
    int maxGalCount;
    float minGalMass;
    float maxGalMass;
    float minGalSize;
    float maxGalSize;
    float minGalVel;
    float maxGalVel;
    float minSpawnRange;
    float maxSpawnRange;

} spawnVars_t;

typedef struct con_s {

    col_t c;
    char s[255];

} con_t;

// for processframeot
typedef struct otinfo_s {

    particle_t *p;
    particleDetail_t *pd;
    struct node_s *n;

} otinfo_t;

// for otComputeParticleToTreeRecursive
typedef struct pttr_s {

    particle_t *p;
    particleDetail_t *pd;
    struct node_s *n;

} pttr_t;

typedef struct pib_s {

    float *min;
    float *max;
    particle_t *gp;
    float m;
    float *cm;

} pib_t;

// main.c
#ifdef WIN32

#ifndef NO_GUI
extern video_t video;
#endif
extern state_t state;
extern view_t view;

#else

#ifndef NO_GUI
video_t video;
#endif
state_t state;
view_t view;

#endif

void cleanMemory();
void runVideo();
void runInput();
void viewInit();
void spawnDefaults();
void stateInit();
int commandLineRead(int argc, char *argv[]);

// tool.c
char * va( char *format, ... );
CONST_F int gfxPowerOfTwo(int input);
size_t LoadMemoryDump(char *fileName, unsigned char *d, size_t size, size_t chunk);
int SaveMemoryDump(char *FileName, unsigned char *d, size_t total);

Uint32 getMS();
void setTitle(char *state);
int mymkdir(const char *path);
void setFileName(char *name);
char *getRegistryString(char *variable);
void setRegistryString(char *variable, char *value);
int myunlink(char *filename);
void freeFileName();
char *findFile(char *file); // finds a file in gravit's search path
int fileExists(char *file); // sees if a file is openable
int checkHomePath();
size_t getMemoryAvailable();

// png_save.c
extern int png_save_surface(char *filename, SDL_Surface *surf);


// spawn.c
extern spawnVars_t spawnVars;
int pickPositions();
void spawnDefaults();
int isSpawning();

// console.c
extern con_t con[CONSOLE_HISTORY+1];
void conAdd(int mode, char *f, ... );
void conInit();
void conDraw();
void conInput(SDLKey keySym, SDLMod modifier, Uint16 unicode);
void conTypedHistoryAdd(char *s);
void conTypedHistoryChange(int i);
void conFree();
void conAutoComplete();

#ifndef NO_GUI

// osd.c
void drawOSD();
void osdInitDefaultWindows();
void osdUpdate();

// input.c
int processKeys();
void processMouse();

// texture.c
GLuint loadTexture(char *fileName, int isSkybox);

// gfx.c
void drawAll();
void drawFrame();
void drawFrameSet2D();
void drawFrameSet3D();
int gfxInit();
int gfxSetResolution();
void checkPointParameters();
void checkPointSprite();
void drawPopupText();
void loadSkyBox(void);
// toDo: auto-detect list of availeable skyboxes
#define SKYBOX_LAST 2

#define SPRITE_DEFAULT 0
#define SPRITE_GLOW 1
#define SPRITE_RED 2
#define SPRITE_GREEN 3
#define SPRITE_BLUE 4
#define SPRITE_GRAY 5
#define SPRITE_GRAY2 6
#define SPRITE_GLOW2 7
#define SPRITE_LAST 8

// color.c
void setColours();
void setColoursByMass();
void colourSpectrumClear();
void colourFromNormal(float *c, float n);
unsigned int colourSprite(float *c, float mass);


#else

int processKeys();

#endif

// frame.c
int initFrame();
void processFrame();
void forceToCenter();
void processCollisions();

// frame-pp.c
void processFramePP(int s, int n);

// frame-pp_sse.c
void processFramePP_SSE(int s, int n);

// frame-ot.c
typedef struct node_s {

    VectorNew(min);
    VectorNew(max);
    VectorNew(c);
    VectorNew(cm);

    particle_t *p;
    struct node_t *b[8];
    float mass;
    float length2;

} node_t;

void otDrawTree();
void otFreeTree();
void processFrameOT(int,int);
void otDrawFieldRecursive(float *pos, node_t *node, float *force);

// void frDoGravity(particle_t *p, node_t *n, float d);

extern float fpsCurrentAverageFPS;
extern float fpsCurrentAverageFT;
void fpsInit();
void fpsUpdate(float);

int configRead(char *filename, int ignoreMissing);

void drawCube();

extern const char *colourModes[];

// timer.c
void timerInit();
void timerFree();
void timerUpdate();
void timerAdd(char *name, float seconds, int reps, char *command);
void timerDel(char *name);
void timerList();

#ifdef HAVE_LUA
// lua.c
int luaInit();
int luag_spawn(lua_State *L);
void luaFree();
int luag_log(lua_State *L);
int luag_load(lua_State *L);
int luaExecute(char *f);
#endif

#endif

