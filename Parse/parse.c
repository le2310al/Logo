#include "parse.h"

// Test for valid command line arguments, 
// allocate memory and read input into array
int main(int argc, char *argv[]){
   test();
   if(argc!=2){
      ERROR("Invalid number of arguments");
   }
   FILE *input=fopen(argv[1],"r");
   if(input==NULL )  {
      ERROR("Invalid FILE");
   }
   if(ftell(input)==-1L){
      ERROR("Empty FILE");
   }
   PRG *p=calloc(1,sizeof(PRG));
   if(p==NULL){
      ERROR("Memory allocation failed");
   }
   for(int i=0;i<MAXNUMTOKENS;i++){
      fscanf(input,"%s",&p->wds[i][0]);
   }
   p->cw=0;
   p->test=false;
   prog(p);
   fclose(input);
   free(p);
   return 0;
}

// Check for valid program START
bool prog(PRG *p){
   if(strcmp(p->wds[p->cw],"START")!=0){
      if(p->test){
         return false;
      }
      ERROR("No START statement");
   }
   if(p->test){
      return true;
   }
   p->cw++;
   inslst(p);
   return true;
}

// Check for program END
bool inslst(PRG *p){
   if(strcmp(p->wds[p->cw],"END")==0){
      return true;
   }
   if(!ins(p)&&p->test){
      return false;
   }
   inslst(p);
   return true;
}

// Redirect to correct INSTRUCTION
bool ins(PRG *p){
   char *I[5]={"FORWARD","RIGHT","COLOUR","LOOP","SET"};
   for(int i=0;i<5;i++){
      if(strcmp(p->wds[p->cw],I[i])==0){
         p->cw++;
         switch(i){
            case FORWARD:
               varnum(p);
               break;
            case RIGHT:
               varnum(p);
               break;
            case COLOUR:
               col(p);
               break;
            case LOOP:
               loop(p);
               break;
            case SET:
               set(p);
               break;
         }
         return true;
      }
   }
   if(p->test){
      return false;
   }
   ERROR("No INSTRUCTION found");
}

// Check for valid COLOUR grammar
bool col(PRG *p){
   if(var(p)==true){
      return true;
   }
   char *C[8]={"\"BLACK\"","\"RED\"","\"GREEN\"","\"BLUE\"",
      "\"YELLOW\"","\"CYAN\"","\"MAGENTA\"","\"WHITE\""};
   for(int i=0;i<8;i++){
      if(strcmp(p->wds[p->cw],C[i])==0){
         p->cw++;
         return true;
      }
   }
   if(p->test){
      return false;
   }
   ERROR("No COLOUR found");
}

// Check for LOOP grammar/initiation
bool loop(PRG *p){
   if(strlen(p->wds[p->cw])==1 && ltr(p,p->wds[p->cw][0]) 
      && strcmp(p->wds[p->cw+1],"OVER")==0){
      p->cw=p->cw+2;
      if(p->test){
         return true;
      }
      lst(p);
      inslst(p);
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR("LOOP incomplete");
}

// Check for SET grammar
bool set(PRG *p){
   if(strlen(p->wds[p->cw])==1 && ltr(p,p->wds[p->cw][0]) 
      && strcmp(p->wds[p->cw+1],"(")==0){
      if(p->test){
         return true;
      }
      p->cw+=2;
      pfix(p);
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR("SET incomplete");
}

// Redirect to VARIABLE and NUMBER checks
bool varnum(PRG *p){
   if(var(p)){
      return true;
   }
   if(num(p)){
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR("No VARIABLE or NUMBER found");
}


// Check for valid VARIABLE grammar
bool var(PRG *p){
   if(strlen(p->wds[p->cw])==2 && p->wds[p->cw][0] == DOLLAR 
      && ltr(p,p->wds[p->cw][1])){
      p->cw++;
      return true;
   }
   if(p->test){
      return false;
   }
   return false;
}

// Check for valid LETTER
bool ltr(PRG *p, char c){
   if(c>=UPPERA && c<=UPPERZ){
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR("No UPPERCASE LETTER found");
}

// Check for valid NUMBER
bool num(PRG *p){
   double number;
   if(sscanf(p->wds[p->cw],"%lf",&number)==1){
      p->cw++;
      return true;
   }
   return false;
}

// Check for valid WORD
bool word(PRG *p){
   int len=strlen(p->wds[p->cw]);
   if(p->wds[p->cw][0]==DQUOTE && p->wds[p->cw][len-1]==DQUOTE){
      p->cw++;
      return true;
   }
   return false;
}

// Check if LIST is initiated
bool lst(PRG *p){
   if(strcmp(p->wds[p->cw],"{")==0){
      p->cw++;
      if(p->test){
         return true;
      }
      items(p);
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR(" No LIST found ");
}

// Check for end of LIST
bool items(PRG *p){
   if(strcmp(p->wds[p->cw],"}")==0){
      p->cw++;
      return true;
   }
   if(!item(p) && p->test){
      return false;
   }
   items(p);
   return true;
}

// Redirect to correct ITEM check
bool item(PRG *p){
   if(var(p)){
      return true;
   }
   if(num(p)){
      return true;
   }
   if(word(p)){
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR("No ITEM found");
}

// Test for valid POSTFIX NOTATION
bool pfix(PRG *p){
   if(strcmp(p->wds[p->cw],")")==0){
      p->cw++;
      return true;
   }
   char *O={"=+-/*"};
   if(strlen(p->wds[p->cw])==1 && strchr(O,p->wds[p->cw][0])!=NULL){
      p->cw++;
      if(p->test){
         return true;
      }
      pfix(p);
      return true;
   }
   if(var(p) || num(p)){
      if(p->test){
         return true;
      }
      pfix(p);
      return true;
   }
   if(p->test){
      return false;
   }
   ERROR("Incomplete POSTFIX NOTATION");
}

void test(void){
   PRG *p=calloc(1,sizeof(PRG));
   p->test=true;
   char words[MAXNUMTOKENS][MAXTOKENSIZE]={
      "START", "LOOP", "C", "OVER", "{", 
      "\"BLACK\"", "\"RED\"","\"GREEN\"", "\"BLUE\"", "\"YELLOW\"", 
      "\"APPLE\"", "}", "COLOUR", "$C", "FORWARD", 
      "5", "RIGHT", "90", "END", "RIGHT", 
      "-90", "COLOUR", "\"MAGENTA\"", "FORWARD", "3", 
      "LOOP", "C", "OVER", "{", "1", 
      "2", "3", "}", "COLOUR", "\"WHITE\"", 
      "RIGHT", "-120", "SET", "A", "(", 
      "$A", "1.10", "+", ")", "SET", 
      "A", "(", "$A", "1.10", "-",
      ")","SET", "A", "(", "$A", 
      "1.10", "/", ")","SET", "A",
      "(", "$A", "1.10", "*", ")",
      "END", "END"};
   for (int i=0; i<MAXNUMTOKENS; i++){
      strcpy(p->wds[i], words[i]);
   }

   // prog()
   p->cw=0; //start
   assert(prog(p)==true);
   p->cw=66; //end
   assert(prog(p)==false);
   p->cw=14; //forward
   assert(prog(p)==false);
   p->cw=16; //right
   assert(prog(p)==false);
   p->cw=12; //colour
   assert(prog(p)==false);
   p->cw=1; //loop
   assert(prog(p)==false);
   p->cw=37; //set
   assert(prog(p)==false);
   p->cw=13; //variable
   assert(prog(p)==false);
   p->cw=2; //letter OVER
   assert(prog(p)==false);
   p->cw=38; //letter (
   assert(prog(p)==false);
   p->cw=15; //number
   assert(prog(p)==false);
   p->cw=10; // non-colour word
   assert(prog(p)==false);
   p->cw=4; // "{"
   assert(prog(p)==false);
   p->cw=11; // "}"
   assert(prog(p)==false);
   p->cw=39; // "("
   assert(prog(p)==false);
   p->cw=43; // ")"
   assert(prog(p)==false);
   p->cw=42; // "+"
   assert(prog(p)==false);
   p->cw=49; // "-"
   assert(prog(p)==false);
   p->cw=56; // "/"
   assert(prog(p)==false);
   p->cw=63; // "*"
   assert(prog(p)==false);

   // inslst()
   p->cw=0; //start
   assert(inslst(p)==false);
   p->cw=66; //end
   assert(inslst(p)==true);
   p->cw=14; //forward
   assert(inslst(p)==true);
   p->cw=16; //right
   assert(inslst(p)==true);
   p->cw=12; //colour
   assert(inslst(p)==true);
   p->cw=1; //loop
   assert(inslst(p)==true);
   p->cw=37; //set
   assert(inslst(p)==true);
   p->cw=13; //variable
   assert(inslst(p)==false);
   p->cw=2; //letter OVER
   assert(inslst(p)==false);
   p->cw=38; //letter (
   assert(inslst(p)==false);
   p->cw=15; //number
   assert(inslst(p)==false);
   p->cw=10; // non-colour word
   assert(inslst(p)==false);
   p->cw=4; // "{"
   assert(inslst(p)==false);
   p->cw=11; // "}"
   assert(inslst(p)==false);
   p->cw=39; // "("
   assert(inslst(p)==false);
   p->cw=43; // ")"
   assert(inslst(p)==false);
   p->cw=42; // "+"
   assert(inslst(p)==false);
   p->cw=49; // "-"
   assert(inslst(p)==false);
   p->cw=56; // "/"
   assert(inslst(p)==false);
   p->cw=63; // "*"
   assert(inslst(p)==false);

   // ins()
   p->cw=0; //start
   assert(ins(p)==false);
   p->cw=66; //end
   assert(ins(p)==false);
   p->cw=14; //forward
   assert(ins(p)==true);
   p->cw=16; //right
   assert(ins(p)==true);
   p->cw=12; //colour
   assert(ins(p)==true);
   p->cw=1; //loop
   assert(ins(p)==true);
   p->cw=37; //set
   assert(ins(p)==true);
   p->cw=13; //variable
   assert(ins(p)==false);
   p->cw=2; // letter OVER
   assert(ins(p)==false);
   p->cw=38; //letter (
   assert(ins(p)==false);
   p->cw=15; //number
   assert(ins(p)==false);
   p->cw=10; // non-colour word
   assert(ins(p)==false);
   p->cw=4; // "{"
   assert(ins(p)==false);
   p->cw=11; // "}"
   assert(ins(p)==false);
   p->cw=39; // "("
   assert(ins(p)==false);
   p->cw=43; // ")"
   assert(ins(p)==false);
   p->cw=42; // "+"
   assert(ins(p)==false);
   p->cw=49; // "-"
   assert(ins(p)==false);
   p->cw=56; // "/"
   assert(ins(p)==false);
   p->cw=63; // "*"
   assert(ins(p)==false);

   //col()
   p->cw=0; //start
   assert(col(p)==false);
   p->cw=66; //end
   assert(col(p)==false);
   p->cw=14; //forward
   assert(col(p)==false);
   p->cw=16; //right
   assert(col(p)==false);
   p->cw=12; //colour
   assert(col(p)==false);
   p->cw=1; //loop
   assert(col(p)==false);
   p->cw=37; //set
   assert(col(p)==false);
   p->cw=13; //variable
   assert(col(p)==true);
   p->cw=2; //letter OVER
   assert(col(p)==false);
   p->cw=38; //letter (
   assert(col(p)==false);
   p->cw=15; //number
   assert(col(p)==false);
   p->cw=5; // black
   assert(col(p)==true);
   p->cw=6; // red
   assert(col(p)==true);
   p->cw=7; // green
   assert(col(p)==true);
   p->cw=8; // blue
   assert(col(p)==true);
   p->cw=9; // yellow
   assert(col(p)==true);
   p->cw=22; // magenta
   assert(col(p)==true);
   p->cw=34; // white
   assert(col(p)==true);
   p->cw=10; // non-colour word
   assert(col(p)==false);
   p->cw=4; // "{"
   assert(col(p)==false);
   p->cw=11; // "}"
   assert(col(p)==false);
   p->cw=39; // "("
   assert(col(p)==false);
   p->cw=43; // ")"
   assert(col(p)==false);
   p->cw=42; // "+"
   assert(col(p)==false);
   p->cw=49; // "-"
   assert(col(p)==false);
   p->cw=56; // "/"
   assert(col(p)==false);
   p->cw=63; // "*"
   assert(col(p)==false);

   //loop()
   p->cw=0; //start
   assert(loop(p)==false);
   p->cw=66; //end
   assert(loop(p)==false);
   p->cw=14; //forward
   assert(loop(p)==false);
   p->cw=16; //right
   assert(loop(p)==false);
   p->cw=12; //colour
   assert(loop(p)==false);
   p->cw=1; //loop
   assert(loop(p)==false);
   p->cw=37; //set
   assert(loop(p)==false);
   p->cw=13; //variable
   assert(loop(p)==false);
   p->cw=2; //letter OVER
   assert(loop(p)==true);
   p->cw=38; //letter (
   assert(loop(p)==false);
   p->cw=15; //number
   assert(loop(p)==false);
   p->cw=10; // non-colour word
   assert(loop(p)==false);
   p->cw=4; // "{"
   assert(loop(p)==false);
   p->cw=11; // "}"
   assert(loop(p)==false);
   p->cw=39; // "("
   assert(loop(p)==false);
   p->cw=43; // ")"
   assert(loop(p)==false);
   p->cw=42; // "+"
   assert(loop(p)==false);
   p->cw=49; // "-"
   assert(loop(p)==false);
   p->cw=56; // "/"
   assert(loop(p)==false);
   p->cw=63; // "*"
   assert(loop(p)==false);

   //set()
   p->cw=0; //start
   assert(set(p)==false);
   p->cw=66; //end
   assert(set(p)==false);
   p->cw=14; //forward
   assert(set(p)==false);
   p->cw=16; //right
   assert(set(p)==false);
   p->cw=12; //colour
   assert(set(p)==false);
   p->cw=1; //loop
   assert(set(p)==false);
   p->cw=37; //set
   assert(set(p)==false);
   p->cw=13; //variable
   assert(set(p)==false);
   p->cw=2; //letter OVER
   assert(set(p)==false);
   p->cw=38; //letter (
   assert(set(p)==true);
   p->cw=15; //number
   assert(set(p)==false);
   p->cw=10; //non-colour word
   assert(set(p)==false);
   p->cw=4; //"{"
   assert(set(p)==false);
   p->cw=11; //"}"
   assert(set(p)==false);
   p->cw=39; //"("
   assert(set(p)==false);
   p->cw=43; //")"
   assert(set(p)==false);
   p->cw=42; //"+"
   assert(set(p)==false);
   p->cw=49; //"-"
   assert(set(p)==false);
   p->cw=56; //"/"
   assert(set(p)==false);
   p->cw=63; //"*"
   assert(set(p)==false);

   //varnum()
   p->cw=0; //start
   assert(set(p)==false);
   p->cw=66; //end
   assert(varnum(p)==false);
   p->cw=14; //forward
   assert(varnum(p)==false);
   p->cw=16; //right
   assert(varnum(p)==false);
   p->cw=12; //colour
   assert(varnum(p)==false);
   p->cw=1; //loop
   assert(varnum(p)==false);
   p->cw=37; //set
   assert(varnum(p)==false);
   p->cw=13; //variable
   assert(varnum(p)==true);
   p->cw=2; //letter OVER
   assert(varnum(p)==false);
   p->cw=38; //letter (
   assert(varnum(p)==false);
   p->cw=15; //number
   assert(varnum(p)==true);
   p->cw=10; //non-colour word
   assert(varnum(p)==false);
   p->cw=4; //"{"
   assert(varnum(p)==false);
   p->cw=11; //"}"
   assert(varnum(p)==false);
   p->cw=39; //"("
   assert(varnum(p)==false);
   p->cw=43; //")"
   assert(varnum(p)==false);
   p->cw=42; //"+"
   assert(varnum(p)==false);
   p->cw=49; //"-"
   assert(varnum(p)==false);
   p->cw=56; //"/"
   assert(varnum(p)==false);
   p->cw=63; //"*"
   assert(varnum(p)==false);

   //var()
   p->cw=0; //start
   assert(var(p)==false);
   p->cw=66; //end
   assert(var(p)==false);
   p->cw=14; //forward
   assert(var(p)==false);
   p->cw=16; //right
   assert(var(p)==false);
   p->cw=12; //colour
   assert(var(p)==false);
   p->cw=1; //loop
   assert(var(p)==false);
   p->cw=37; //set
   assert(var(p)==false);
   p->cw=13; //variable
   assert(var(p)==true);
   p->cw=2; //letter OVER
   assert(var(p)==false);
   p->cw=38; //letter (
   assert(var(p)==false);
   p->cw=15; //number
   assert(var(p)==false);
   p->cw=10; // non-colour word
   assert(var(p)==false);
   p->cw=4; //"{"
   assert(var(p)==false);
   p->cw=11; //"}"
   assert(var(p)==false);
   p->cw=39; //"("
   assert(var(p)==false);
   p->cw=43; //")"
   assert(var(p)==false);
   p->cw=42; //"+"
   assert(var(p)==false);
   p->cw=49; //"-"
   assert(var(p)==false);
   p->cw=56; //"/"
   assert(var(p)==false);
   p->cw=63; //"*"
   assert(var(p)==false);

   //ltr()
   p->cw=0; //start
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=66; //end
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=14; //forward
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=16; //right
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=12; //colour
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=1; //loop
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=37; //set
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=13; //variable
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=13; //variable
   assert(ltr(p, p->wds[p->cw][1])==true);
   p->cw=2; //letter OVER
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=38; // letter (
   assert(ltr(p, p->wds[p->cw][0])==true);
   p->cw=15; // number
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=10; // non colour - word
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=4; //"{"
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=11; //"}"
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=39; //"("
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=43; //")"
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=42; //"+"
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=49; //"-"
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=56; //"/"
   assert(ltr(p, p->wds[p->cw][0])==false);
   p->cw=63; //"*"
   assert(ltr(p, p->wds[p->cw][0])==false);

   //num(p)
   p->cw=0; //start
   assert(num(p)==false);
   p->cw=66; //end
   assert(num(p)==false);
   p->cw=14; //forward
   assert(num(p)==false);
   p->cw=16; //right
   assert(num(p)==false);
   p->cw=12; //colour
   assert(num(p)==false);
   p->cw=1; //loop
   assert(num(p)==false);
   p->cw=37; //set
   assert(num(p)==false);
   p->cw=13; //variable
   assert(num(p)==false);
   p->cw=2; //letter OVER
   assert(num(p)==false);
   p->cw=38; // letter (
   assert(num(p)==false);
   p->cw=15; // number
   assert(num(p)==true);
   p->cw=36; // number
   assert(num(p)==true);
   p->cw=41; // number
   assert(num(p)==true);
   p->cw=10; // non colour - word
   assert(num(p)==false);
   p->cw=4; //"{"
   assert(num(p)==false);
   p->cw=11; //"}"
   assert(num(p)==false);
   p->cw=39; //"("
   assert(num(p)==false);
   p->cw=43; //")"
   assert(num(p)==false);
   p->cw=42; //"+"
   assert(num(p)==false);
   p->cw=49; //"-"
   assert(num(p)==false);
   p->cw=56; //"/"
   assert(num(p)==false);
   p->cw=63; //"*"
   assert(num(p)==false);
   
   //word()
   p->cw=0; //start
   assert(word(p)==false);
   p->cw=66; //end
   assert(word(p)==false);
   p->cw=14; //forward
   assert(word(p)==false);
   p->cw=16; //right
   assert(word(p)==false);
   p->cw=12; //colour
   assert(word(p)==false);
   p->cw=1; //loop
   assert(word(p)==false);
   p->cw=37; //set
   assert(word(p)==false);
   p->cw=13; //variable
   assert(word(p)==false);
   p->cw=2; //letter OVER
   assert(word(p)==false);
   p->cw=38; // letter (
   assert(word(p)==false);
   p->cw=15; // number
   assert(word(p)==false);
   p->cw=36; // number
   assert(word(p)==false);
   p->cw=41; // number
   assert(word(p)==false);
   p->cw=10; // non-colour word
   assert(word(p)==true);
   p->cw=5; // colour word
   assert(word(p)==true);
   p->cw=4; //"{"
   assert(word(p)==false);
   p->cw=11; //"}"
   assert(word(p)==false);
   p->cw=39; //"("
   assert(word(p)==false);
   p->cw=43; //")"
   assert(word(p)==false);
   p->cw=42; //"+"
   assert(word(p)==false);
   p->cw=49; //"-"
   assert(word(p)==false);
   p->cw=56; //"/"
   assert(word(p)==false);
   p->cw=63; //"*"
   assert(word(p)==false);
   
   //lst()
   p->cw=0; //start
   assert(lst(p)==false);
   p->cw=66; //end
   assert(lst(p)==false);
   p->cw=14; //forward
   assert(lst(p)==false);
   p->cw=16; //right
   assert(lst(p)==false);
   p->cw=12; //colour
   assert(lst(p)==false);
   p->cw=1; //loop
   assert(lst(p)==false);
   p->cw=37; //set
   assert(lst(p)==false);
   p->cw=13; //variable
   assert(lst(p)==false);
   p->cw=2; //letter OVER
   assert(lst(p)==false);
   p->cw=38; // letter (
   assert(lst(p)==false);
   p->cw=15; // number
   assert(lst(p)==false);
   p->cw=36; // number
   assert(lst(p)==false);
   p->cw=41; // number
   assert(lst(p)==false);
   p->cw=10; // non-colour word
   assert(lst(p)==false);
   p->cw=5; // colour word
   assert(lst(p)==false);
   p->cw=4; //"{"
   assert(lst(p)==true);
   p->cw=11; //"}"
   assert(lst(p)==false);
   p->cw=39; //"("
   assert(lst(p)==false);
   p->cw=43; //")"
   assert(lst(p)==false);
   p->cw=42; //"+"
   assert(lst(p)==false);
   p->cw=49; //"-"
   assert(lst(p)==false);
   p->cw=56; //"/"
   assert(lst(p)==false);
   p->cw=63; //"*"
   assert(lst(p)==false);

   //items()
   p->cw=0; //start
   assert(items(p)==false);
   p->cw=66; //end
   assert(items(p)==false);
   p->cw=14; //forward
   assert(items(p)==false);
   p->cw=16; //right
   assert(items(p)==false);
   p->cw=12; //colour
   assert(items(p)==false);
   p->cw=1; //loop
   assert(items(p)==false);
   p->cw=37; //set
   assert(items(p)==false);
   p->cw=13; //variable
   assert(items(p)==true);
   p->cw=2; //letter OVER
   assert(items(p)==false);
   p->cw=38; // letter (
   assert(items(p)==false);
   p->cw=15; // number
   assert(items(p)==true);
   p->cw=36; // number
   assert(items(p)==true);
   p->cw=41; // number
   assert(items(p)==true);
   p->cw=10; // non-colour word
   assert(items(p)==true);
   p->cw=5; // colour word
   assert(items(p)==true);
   p->cw=4; //"{"
   assert(items(p)==false);
   p->cw=11; //"}"
   assert(items(p)==true);
   p->cw=39; //"("
   assert(items(p)==false);
   p->cw=43; //")"
   assert(items(p)==false);
   p->cw=42; //"+"
   assert(items(p)==false);
   p->cw=49; //"-"
   assert(items(p)==false);
   p->cw=56; //"/"
   assert(items(p)==false);
   p->cw=63; //"*"
   assert(items(p)==false);

   //item()
   p->cw=0; //start
   assert(item(p)==false);
   p->cw=66; //end
   assert(item(p)==false);
   p->cw=14; //forward
   assert(item(p)==false);
   p->cw=16; //right
   assert(item(p)==false);
   p->cw=12; //colour
   assert(item(p)==false);
   p->cw=1; //loop
   assert(item(p)==false);
   p->cw=37; //set
   assert(item(p)==false);
   p->cw=13; //variable
   assert(item(p)==true);
   p->cw=2; //letter OVER
   assert(item(p)==false);
   p->cw=38; // letter (
   assert(item(p)==false);
   p->cw=15; // number
   assert(item(p)==true);
   p->cw=36; // number
   assert(item(p)==true);
   p->cw=41; // number
   assert(item(p)==true);
   p->cw=10; // non-colour word
   assert(item(p)==true);
   p->cw=5; // colour word
   assert(item(p)==true);
   p->cw=4; //"{"
   assert(item(p)==false);
   p->cw=11; //"}"
   assert(item(p)==false);
   p->cw=39; //"("
   assert(item(p)==false);
   p->cw=43; //")"
   assert(item(p)==false);
   p->cw=42; //"+"
   assert(item(p)==false);
   p->cw=49; //"-"
   assert(item(p)==false);
   p->cw=56; //"/"
   assert(item(p)==false);
   p->cw=63; //"*"
   assert(item(p)==false);
   
   //pfix()
   p->cw=0; //start
   assert(pfix(p)==false);
   p->cw=66; //end
   assert(pfix(p)==false);
   p->cw=14; //forward
   assert(pfix(p)==false);
   p->cw=16; //right
   assert(pfix(p)==false);
   p->cw=12; //colour
   assert(pfix(p)==false);
   p->cw=1; //loop
   assert(pfix(p)==false);
   p->cw=37; //set
   assert(pfix(p)==false);
   p->cw=13; //variable
   assert(pfix(p)==true);
   p->cw=2; //letter OVER
   assert(pfix(p)==false);
   p->cw=38; // letter (
   assert(pfix(p)==false);
   p->cw=15; // number
   assert(pfix(p)==true);
   p->cw=36; // number
   assert(pfix(p)==true);
   p->cw=41; // number
   assert(pfix(p)==true);
   p->cw=10; // non-colour word
   assert(pfix(p)==false);
   p->cw=5; // colour word
   assert(pfix(p)==false);
   p->cw=4; //"{"
   assert(pfix(p)==false);
   p->cw=11; //"}"
   assert(pfix(p)==false);
   p->cw=39; //"("
   assert(pfix(p)==false);
   p->cw=43; //")"
   assert(pfix(p)==true);
   p->cw=42; //"+"
   assert(pfix(p)==true);
   p->cw=49; //"-"
   assert(pfix(p)==true);
   p->cw=56; //"/"
   assert(pfix(p)==true);
   p->cw=63; //"*"
   assert(pfix(p)==true);

   free(p);
   return;
}
