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

int interp_main(int argc);
int input(char *argv[]);
int prg_alloc();

static void test_cla_num_correct();
static void test_cla_num_incorrect();
static void test_cla_correct();
static void test_cla_incorrect();
int prg_alloc();
static void test_ptrogram_allocated();