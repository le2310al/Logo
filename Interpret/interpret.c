#include "interpret.h"

int main(int argc, char *argv[]){
   PRG* p=input(argc, argv);
   LYR* head=prog(p);
   if(head!=NULL){
      output(head, argc, argv);
   }
   freeall(p, head);
   return 0;
}

// Check for valid input file and number of command line
// arguments
PRG* input(int argc, char *argv[]){
   if(argc<2 || argc>3){
      ERROR("Invalid number of arguments");
   }
   regex_t reg;
   regcomp(&reg,"[.]ttl$",0);
   if(regexec(&reg,argv[1],0,NULL,0)!=0){
      ERROR("Invalid FILE extension");
   }
   regfree(&reg);
   FILE* input=fopen(argv[1],"r");
   if(input==NULL)  {
      ERROR("Invalid FILE");
   }
   if(ftell(input)==-1L){
      ERROR("Empty FILE");
   }
   PRG* p=prg_alloc(input);
   fclose(input);
   return p;
}

// Allocate memory for struct that tracks important values
// and read input file into it
PRG* prg_alloc(FILE* input){
   PRG* p=(PRG*)calloc(1,sizeof(PRG));
   if(p==NULL){
      ERROR("Memory not allocated");
   }
   p->mw=1;
   p->cw=0;
   p->loop[0]=0;
   p->loop[1]=0;
   p->pos[ROW]=STARTROW;
   p->pos[COL]=STARTCOL;
   p->pos[QUADRANT]=Q1;
   p->nav[DIR]=0;
   p->nav[ROWINC]=-1;
   p->nav[COLINC]=0;
   for(int i=0;i<MAXNUMTOKENS;i++){
      fscanf(input,"%s",&p->wds[i][0]);
      p->mw++;
   }
   for(int i=0;i<MAXVARS;i++){
      for(int j=0;j<MAXVALS;j++){
         p->var[i][j]=(char*)calloc(MAXTOKENSIZE,sizeof(char));
      }
   }
   return p;
}

// Check for valid START
LYR* prog(PRG *p){
   LYR* head=lyr_alloc(0), *node=lyr_alloc(1);
   head->next=node;
   if(strcmp(p->wds[p->cw],"START")==0){
      p->cw++;
      ins(p, head);
   }
   return head;
}

// Allocate memory for linked list that holds individual 
// FORWARD instructions
LYR* lyr_alloc(int i){
   LYR* node=(LYR*)calloc(1,sizeof(LYR));
   if(node==NULL){
      ERROR("Memory not allocated");
   }
   for(int row=0;row<ROWS;row++){
      for(int col=0;col<COLS;col++){
         node->grd[row][col]=EMPTY;
      }
   }
   node->name=i;
   node->line[0]=LINEX;
   node->line[1]=LINEY;
   node->clr=WHITE;
   node->next=NULL;
   return node;
}

// Redirect to appropriate INSTRUCTION
void ins(PRG *p, LYR *head){
   char *I[]={"FORWARD","RIGHT","COLOUR","LOOP","SET","END"};
   int i=0;
   do{
      if(p->cw+1<p->mw && strcmp(p->wds[p->cw],I[i])==0){
         switch(i){
            case FWD:
               fwd(p, head);
               return;
            case RGT:
               rgt(p, head);
               return;
            case CLR:
               clr(p, head);
               return;
            case LOOP:
               loop(p, head);
               return;
            case SET:
               set(p, head);
               return;
            case END:
               return;
         }
         i=6;
      }
      i++;
   }while(i<6);
   p->cw++;
   return;
}

// Link new list for new valid FORWARD instruction, inlcude
// current colour instruction
void fwd(PRG *p, LYR *head){
   p->cw++;
   double steps=0;
   if(sscanf(p->wds[p->cw],"%lf",&steps)==1 
      || (var(p->wds[p->cw]) && 
      sscanf(p->var[p->wds[p->cw][1]-ASCIIMOD][0],"%lf",&steps)==1)){
      if((int)steps==0){
         p->cw++;
         return;
      }
      angle_mod(p, steps);
      LYR* l=head;
      while (l->next!=NULL) {
         l=l->next;
      }
      draw_line(p, head, l, steps);
      LYR* node;
      l->next=node=lyr_alloc(l->name+1);
      node->clr=l->clr;
   }
   p->cw++;
   if(p->cw<p->mw){
      ins(p, head);
   }
   return;
}

// Check for valid variable
bool var(char* v){
   if(strlen(v)==2 && v[0]==DOLLAR && ltr(v[1])){
      return true;
   }
   return false;
}

// Adjust angle to be positive and under 90 degrees,
// note quadrant of cartesian coordinate system
void angle_mod(PRG *p, double steps){
   if(steps<0){
      steps*=-1;
      p->nav[DIR]+=180;
   }
   while(p->nav[DIR]<0){
      p->nav[DIR]+=90;
      p->pos[QUADRANT]--;
   }
   while(p->nav[DIR]>=90){
      p->nav[DIR]-=90;
      p->pos[QUADRANT]++;
   }
   double gamma=180-90-p->nav[DIR];
   cartesian(p, gamma, steps);
   return;
}

// Calculate opposite and adjacent sides of right triangle 
// based on quadrant in cartesian coordinate system
void cartesian(PRG *p, double gamma, double steps){
   while(p->pos[QUADRANT]>3){
      p->pos[QUADRANT]-=4;
   }
   while(p->pos[QUADRANT]<0){
      p->pos[QUADRANT]+=4;
   }
   switch(p->pos[QUADRANT]){
      case Q1:
         p->nav[ROWINC]=-steps*sin(gamma*PI/180)/sin(90*PI/180)/(int)steps;
         p->nav[COLINC]=steps*sin(p->nav[DIR]*PI/180)/sin(90*PI/180)/(int)steps;
         break;
      case Q2:
         p->nav[ROWINC]=-steps*sin(p->nav[DIR]*PI/180)/sin(90*PI/180)/(int)steps;
         p->nav[COLINC]=-steps*sin(gamma*PI/180)/sin(90*PI/180)/(int)steps;
         break;
      case Q3:
         p->nav[ROWINC]=steps*sin(gamma*PI/180)/sin(90*PI/180)/(int)steps;
         p->nav[COLINC]=-steps*sin(p->nav[DIR]*PI/180)/sin(90*PI/180)/(int)steps;
         break;
      case Q4:
         p->nav[ROWINC]=steps*sin(p->nav[DIR]*PI/180)/sin(90*PI/180)/(int)steps;
         p->nav[COLINC]=steps*sin(gamma*PI/180)/sin(90*PI/180)/(int)steps;
         break;
   }
   return;
}

// Note destination coordinates and convert to steps within
// the grid, accumulate all instrutions in head and 
// individual instructions in node, note frst step of next instructions
void draw_line(PRG *p, LYR* head, LYR* l, double steps){
   l->line[0]=p->nav[COLINC]*(int)steps;
   l->line[1]=((-1)*(p->nav[ROWINC]*(int)steps));
   for(int j=1;j<(int)steps+2;j++){
      if(j==1 && p->pos[ROW]>=0 && p->pos[ROW]<ROWS && 
         p->pos[COL]>=0 && p->pos[COL]<COLS){
         l->grd[p->pos[ROW]][p->pos[COL]]=l->clr;
         head->grd[p->pos[ROW]][p->pos[COL]]=l->clr;
      }
      int R=p->pos[ROW]+round(p->nav[ROWINC]*(j-1));
      int C=p->pos[COL]+round(p->nav[COLINC]*(j-1));
      if(j>1 && j<(int)steps+1 && R>=0 && R<ROWS && C>=0 && C<COLS){
         l->grd[R][C]=l->clr;
         head->grd[R][C]=l->clr;
      }
      if(j==(int)steps+1){
         p->pos[ROW]+=round(p->nav[ROWINC]*(j-1));
         p->pos[COL]+=round(p->nav[COLINC]*(j-1));
      }   
   }
   return;
}

// Add RIGHT instruction to overall angle of movement
void rgt(PRG *p, LYR* head){
   p->cw++;
   double deg=0;
   if((num(p->wds[p->cw]) && sscanf(p->wds[p->cw],"%lf",&deg)==1 ) 
      || (var(p->wds[p->cw]) && 
      sscanf(p->var[p->wds[p->cw][1]-ASCIIMOD][0],"%lf",&deg)==1)){
      p->nav[DIR]+=deg;
   }
   p->cw++;
   if(p->cw<p->mw){
      ins(p, head);
   }
   return;
}

// Check for valid NUMBER
bool num(char* n){
   double number;
   if(sscanf(n,"%lf",&number)==1){
      return true;
   }
   return false;
}

// Check for valid COLOUR and note it
void clr(PRG *p, LYR *head){
   LYR* l=head;
   while (l->next!=NULL) {
      l=l->next;
   }
   p->cw++;
   char colour[MAXTOKENSIZE]="";
   strcpy(colour,p->wds[p->cw]);
   if(var(p->wds[p->cw])){
      strcpy(colour,p->var[p->wds[p->cw][1]-ASCIIMOD][0]);
   }
   char *C[]={"\"BLACK\"","\"RED\"","\"GREEN\"","\"YELLOW\"",
      "\"BLUE\"","\"MAGENTA\"","\"CYAN\"","\"WHITE\""};
   for(int i=0;i<8;i++){
      if(strcmp(colour,C[i])==0){
         l->clr=i;
      }
   }
   p->cw++;
   if(p->cw<p->mw){
      ins(p, head);
   }
   return;
}

// Check for valid initiation of SET instruction
void set(PRG *p, LYR* head){
   p->cw++;
   if(p->cw+2<p->mw && strlen(p->wds[p->cw])==1 && ltr(p->wds[p->cw][0]) && 
      strcmp(p->wds[p->cw+1],"(")==0){
      int var=p->wds[p->cw][0]-ASCIIMOD;
      p->cw++;
      double calculator[3];
      calculator[0]=1;
      pfix(p, calculator);
      sprintf(p->var[var][0],"%lf",calculator[1]);
   }
   p->cw++;
   if(p->cw<p->mw){
      ins(p, head);
   }
   return;
}

// Check for valid LETTER
bool ltr(char l){
   if(l>=UPPERA && l<=UPPERZ){
      return true;
   }
   return false;
}

// Evaluate expression within SET instruction
void pfix(PRG *p, double* calculator){
   p->cw++;
   if(var(p->wds[p->cw]) && num(p->var[p->wds[p->cw][1]-ASCIIMOD][0])){
      sscanf(p->var[p->wds[p->cw][1]-ASCIIMOD][0],"%lf",&calculator[(int)calculator[0]]);
      calculator[0]++;
      p->cw++;
   }
   if(num(p->wds[p->cw])){
      sscanf(p->wds[p->cw],"%lf",&calculator[(int)calculator[0]]);
      calculator[0]++;
      p->cw++;
   }
   if(strlen(p->wds[p->cw])==1 && (int)calculator[0]==3){
      arithmetic(p, calculator);
      p->cw++;
   }
   if(strcmp(p->wds[p->cw],")")==0){
      return;
   }
   if(p->cw+1<p->mw){
      pfix(p, calculator);
   }
   return;
}

// Perform arithmetic operation within SET instruction
void arithmetic(PRG *p, double* calculator){
   switch(p->wds[p->cw][0]){
      case ADD:
         calculator[1]+=calculator[2];
         calculator[0]--;
         break;
      case SUBTRACT:
         calculator[1]-=calculator[2];
         calculator[0]--;
         break;
      case DIVIDE:
         calculator[1]/=calculator[2];
         calculator[0]--;
         break;
      case MULTIPLY:
         calculator[1]*=calculator[2];
         calculator[0]--;
         break;
   }
   return;
}

// Check for valid initiation of LOOP instruction
void loop(PRG *p, LYR * head){
   p->cw++;
   if(p->cw+2<p->mw && strlen(p->wds[p->cw])==1 && ltr(p->wds[p->cw][0]) && 
      strcmp(p->wds[p->cw+1],"OVER")==0){
      int var=p->wds[p->cw][0]-ASCIIMOD;
      p->loop[0]=p->wds[p->cw][0];
      p->cw+=2;
      lst(p);
      int anchor=p->cw;
      int iterations=p->loop[1];
      p->loop[0]=0;
      p->loop[1]=0;
      for(int i=0;i<iterations;i++){
         p->cw=anchor;
         ins(p, head);
         var_mod(p, var, iterations);
      }
   }
   p->cw++;
   if(p->cw<p->mw){
      ins(p, head);
   }
   return;
}

// Check for start of LIST
void lst(PRG *p){
   if(p->cw+1<p->mw && strcmp(p->wds[p->cw],"{")==0){
      p->cw++;
      items(p);
      p->cw++;
   }
   p->attempts=0;
   return;
}

// Check for end of LIST
void items(PRG *p){
   if(strcmp(p->wds[p->cw],"}")!=0){
      item(p);
      p->cw++;
      p->attempts++;
      if(p->cw<p->mw){
         items(p);
      }
   }
   return;
}

// Check for valid ITEM
void item(PRG *p){
   if(var(p->wds[p->cw])){
      strcpy(p->var[p->loop[0]-ASCIIMOD][p->loop[1]],p->var[p->wds[p->cw][1]-ASCIIMOD][0]);
      p->loop[1]++;
      return;
   }
   if(num(p->wds[p->cw]) || word(p->wds[p->cw])){
      strcpy(p->var[p->loop[0]-ASCIIMOD][p->loop[1]],p->wds[p->cw]);
      p->loop[1]++;
      return;
   }
   return;
}

// Check for valid WORD
bool word(char* w){
   int len=strlen(w);
   if(w[0]==DQUOTE && w[len-1]==DQUOTE){
      return true;
   }
   return false;
}

// remove value from queue after finishing a loop
void var_mod(PRG *p, int var, int iterations){
   for(int i=0;i<iterations;i++){
      strcpy(p->var[var][i],p->var[var][i+1]);
   }
   return;
}

// Redirect to appropriate OUTPUT format
void output(LYR* head, int argc, char *argv[]){
   if(argc==3){
      regex_t reg;
      regcomp(&reg,"[.]txt$",0);
      if(regexec(&reg,argv[2],0,NULL,0)==0){
         regfree(&reg);
         txt(head, argv[2]);
         return;
      }
      regfree(&reg);
      regcomp(&reg,"[.]ps$",0);
      if(regexec(&reg,argv[2],0,NULL,0)==0){
         regfree(&reg);
         ps(head, argv[2]);
         return;
      }
      regfree(&reg);
      ERROR("Invalid OUTPUT FILE INSTRUCTION");
   }
   ansi(head); 
   return;
}

// Print grid to terminal
void ansi(LYR* head){
   LYR* node=head->next;
   while(node!=NULL){
      printf("\033[2J");
      printf("\033[H");
      printf("\033[2J");
      char colour[]={'K','R','G','Y','B','M','C','W',' '};
      for(int row=0;row<ROWS;row++){
         for(int col=0;col<COLS;col++){
            if(node->grd[row][col]==EMPTY){
               printf("\033[0m");
               printf("%c",' ');
            }
            if(node->grd[row][col]!=EMPTY){
               printf("\033[%im", node->clr+ANSIFGMOD);
               printf("\033[%im", ANSIBGMOD-node->clr);
               printf("%c", colour[node->grd[row][col]]);
            }
         }
         printf("\n");
      }
      printf("\033[2J");
      printf("\033[H");
      printf("\033[2J");
      sleep(1);
      node=node->next;
   }
   printf("\033[0m");
   return;
}

// Print grid to text file
void txt(LYR* head, char* filename){
   FILE* txt_file=fopen(filename,"w");
   if(txt_file==NULL){
      ERROR("Invalid FILE");
   }
   char colour[]={'K','R','G','Y','B','M','C','W',' '};
   for(int row=0;row<ROWS;row++){
      for(int col=0;col<COLS;col++){
         fprintf(txt_file,"%c",colour[head->grd[row][col]]);
      }
      fprintf(txt_file,"\n");
   }
   fclose(txt_file);
   return;
}

// Print instructions to postscript file
void ps(LYR* head, char* psname){
   FILE* ps_file=fopen(psname,"w+");
   if(ps_file==NULL){
      ERROR("Invalid FILE");
   }
   char* colour[8]={"0 0 0", "1 0 0 ", "0 1 0", "1 1 0", 
      "0 0 1", "1 0 1", "0 1 1", "0.8 0.8 0.8"};
   fprintf(ps_file,"0.2 setlinewidth");   
   fprintf(ps_file,"\n10 10 scale");
   double moveto[COORDINATES];
   moveto[0]=LINEX;
   moveto[1]=LINEY;
   LYR* node=head->next;
   while(node->next!=NULL){
      fprintf(ps_file,"\nnewpath");
      fprintf(ps_file,"\n%f %f moveto",moveto[0],moveto[1]);
      moveto[0]+=node->line[0];
      moveto[1]+=node->line[1];
      fprintf(ps_file,"\n%f %f lineto",moveto[0],moveto[1]);
      fprintf(ps_file,"\n%s setrgbcolor",colour[node->clr]);
      fprintf(ps_file,"\nstroke");
      node=node->next;
   }
   fprintf(ps_file,"\nshowpage");
   fclose(ps_file);
   sys_call(psname);
   return;
}

// Convert postscript file to pdf
void sys_call(char* psname){
   char root[MAXTOKENSIZE]="";
   strcpy(root,psname);
   char pdfname[MAXTOKENSIZE]="";
   strcat(pdfname,strtok(root,"."));
   strcat(pdfname,".pdf");
   char call[ARGUMENT]="";
   sprintf(call,"ps2pdf ./%s", psname);
   system(call);
   return;
}

// Free all allocated memory
void freeall(PRG* p, LYR* head){
   for(int i=0;i<MAXVARS;i++){
      for(int j=0;j<MAXVALS;j++){
         free(p->var[i][j]);
      }
   }
   free(p);
   LYR* node;
   while(head!=NULL){
      node=head;
      head=head->next;
      free(node);
   }
   return;
}