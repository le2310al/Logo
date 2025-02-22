#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <regex.h>
#include <unistd.h> 
#include <assert.h>
#include <sys/syscall.h>
#include <SDL2/SDL.h> 
#include <SDL2/SDL_timer.h> 
#include <SDL2/SDL_image.h>

#define MAXNUMTOKENS 150
#define MAXTOKENSIZE 30
#define ARGUMENT 100
#define MAXVARS 26
#define MAXVALS 20

#define UPPERA 65
#define UPPERZ 90
#define DOLLAR 36
#define DOUBLEQUOTE 34

#define COORDINATES 3
#define ROWS 32
#define COLS 50
#define STARTROW 16
#define STARTCOL 25
#define LINEX 30.000000
#define LINEY 40.000000
#define UP -1


#define INSTRUCTIONS 7

#define MAXSPEED 100

#define HALFCIRCLE 180
#define QUARTERCIRCLE 90

#define STACK 3
#define MAXATTEMPTS 10

#define VALQUEUE 2

#define PI 3.14159265358979323846

#define COLOURS 8
#define ASCIIMOD 65
#define ANSIFGMOD 30
#define ANSIBGMOD 47

#define WINDOW_WIDTH 510
#define WINDOW_HEIGHT 330
#define SCALE 10

#define VELOCITY 50
#define STARTX 260
#define STARTY 170

#define ERROR(PHRASE){fprintf(stderr,\
"Fatal Error %s occurred in %s, line %d\n",PHRASE,\
__FILE__, __LINE__);\
exit(EXIT_FAILURE);}

typedef struct PROGRAM{
   char wds[MAXNUMTOKENS][MAXTOKENSIZE];
   int mw;
   int cw;
   char* var[MAXVARS][MAXVALS];
   int loop [VALQUEUE];
   int pos[COORDINATES];
   unsigned speed;
   float nav[COORDINATES];
}PRG;

typedef struct LAYERS{
   int name;
   int grd[ROWS][COLS]; // ANSI and TXT
   float line[COORDINATES]; // PS
   int clr; // PS
   float alpha; // SDL
   unsigned speed;// SDL and ANSI
   struct LAYERS* next;
}LYR;

typedef struct TERRARIUN{
   SDL_Texture* tex[COLOURS];
   SDL_Rect* ttl[COLOURS];
}TRR;

enum {ROW, COL, QUADRANT};
enum {DIR, ROWINC, COLINC};
enum {FWD, RGT, CLR, LOOP, SET, SPEED, END};
enum {Q1=0, Q2=3, Q3=2, Q4=1};
enum {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, EMPTY};
enum {ADD=43, SUBTRACT=45, DIVIDE=47, MULTIPLY=42};
enum {X, Y};
enum {OPERANDS, OP1, OP2};

PRG* input(int argc, char *argv[]);
PRG* prg_alloc(FILE* input);
PRG* processing( PRG* p, PRG* preprocessor);
LYR* prog(PRG *p);
LYR* lyr_alloc(int i);

void ins(PRG *p, LYR *head);

void fwd(PRG *p, LYR *head);
void draw_line(PRG *p, LYR* head, LYR* l, double steps);
double angle_mod(PRG *p, double steps);
void cartesian(PRG *p, double gamma, double steps);

void rgt(PRG *p, LYR *head);

void clr(PRG *p, LYR *head);

void set(PRG *p, LYR *head);
void pfix(PRG *p, double* calculator);
void arithmetic(PRG *p, double* calculator);

void loop(PRG *p, LYR *head);
void lst(PRG *p);
void items(PRG *p);
void item(PRG *p);
bool word(char* w);
void var_mod(PRG *p, int var, int loops);

void speed(PRG *p, LYR *head);

bool var(char* v);
bool ltr(char l);
bool num(char* n);

void output(LYR* head, int argc, char *argv[]);
void ansi(LYR* head);
void txt(LYR* head, char* filename);
void ps(LYR* head, char* psname);
void sys_call(char*psname);

void render(LYR* head);
SDL_Window* win_init();
SDL_Renderer* ren_init(SDL_Window* win);
TRR* img_init(SDL_Window* win, SDL_Renderer* ren);
void ttl_init(TRR* t);
void animate(SDL_Renderer* ren, TRR* t, LYR* node, int *pos);
void clean(SDL_Window* win, SDL_Renderer* ren, TRR* t);
void freeall(PRG* p, LYR* head);
