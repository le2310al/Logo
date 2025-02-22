#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <regex.h>
#include <unistd.h> 
#include <assert.h>
#include <unistd.h>

#define MAXNUMTOKENS 150
#define MAXTOKENSIZE 30
#define ARGUMENT 100
#define MAXVARS 26
#define MAXVALS 20
#define STACK 20

#define UPPERA 65
#define UPPERZ 90
#define DOLLAR 36
#define DQUOTE 34

#define COORDINATES 3
#define ROWS 32
#define COLS 50
#define STARTROW 16
#define STARTCOL 25
#define LINEX 30.000000
#define LINEY 40.000000

#define VALQUEUE 2

#define PI (3.14159265358979323846)

#define ASCIIMOD 65
#define ANSIFGMOD 30
#define ANSIBGMOD 47

#define ERROR(PHRASE){fprintf(stderr,\
"Fatal Error %s occurred in %s, line %d\n",PHRASE,\
__FILE__,__LINE__);\
exit(EXIT_FAILURE);}

typedef struct PROGRAM{
   char wds[MAXNUMTOKENS][MAXTOKENSIZE];
   int mw; // Max words
   int cw; // Current word
   int attempts;
   char* var[MAXVARS][MAXVALS];
   int loop [VALQUEUE];
   int pos[COORDINATES];
   double nav[COORDINATES];
}PRG;

typedef struct LAYER{
   int name;
   int grd[ROWS][COLS]; // ANSI and TXT
   double line[COORDINATES]; // PS
   int clr; // PS
   struct LAYER* next;
}LYR;

enum {FWD, RGT, CLR, LOOP, SET, END};
enum {Q1=0, Q2=3, Q3=2, Q4=1};
enum {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, EMPTY};
enum {ADD=43, SUBTRACT=45, DIVIDE=47, MULTIPLY=42};
enum {ANSI, TXT, PS};
enum {DIR, ROWINC, COLINC};
enum {ROW, COL, QUADRANT};

PRG* input(int argc, char *argv[]);
PRG* prg_alloc(FILE* input);
LYR* prog(PRG *p);
LYR* lyr_alloc(int i);

void ins(PRG *p, LYR *head);

void fwd(PRG *p, LYR *head);
void angle_mod(PRG *p, double steps);
void cartesian(PRG *p, double gamma, double steps);
void draw_line(PRG *p, LYR* head, LYR* l, double steps);

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
void var_mod(PRG *p, int var, int iterations);

bool var(char* v);
bool ltr(char l);
bool num(char* n);

void output(LYR* head, int argc, char *argv[]);
void ansi(LYR* head);
void txt(LYR* head, char* filename);
void ps(LYR* head, char* psname);
void sys_call(char*psname);
void freeall(PRG* p, LYR* head);