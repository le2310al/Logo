#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define MAXNUMTOKENS 150
#define MAXTOKENSIZE 30
#define DOLLAR 36
#define UPPERA 65
#define UPPERZ 90
#define DQUOTE 34
#define ERROR(PHRASE){fprintf(stderr,\
"Fatal Error %s occurred in %s, line %d\n",PHRASE,\
__FILE__, __LINE__);\
exit(EXIT_FAILURE);}

typedef struct PROGRAM{
   char wds[MAXNUMTOKENS][MAXTOKENSIZE]; // Words
   int cw; // Current word
   bool test; // Test mode
} PRG;

enum {FORWARD, RIGHT, COLOUR, LOOP, SET};

bool prog(PRG *p);
bool inslst(PRG *p);
bool ins(PRG *p);
bool col(PRG *p);
bool loop(PRG *p);
bool set(PRG *p);
bool varnum(PRG *p);
bool var(PRG *p);
bool ltr(PRG *p, char c);
bool num(PRG *p);
bool word(PRG *p);
bool lst(PRG *p);
bool items(PRG *p);
bool item(PRG *p);
bool pfix(PRG *p);
void test(void);