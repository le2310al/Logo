#include "./sput.h"
#include "./test_interp.h"

int main(void){
   sput_start_testing();

   sput_enter_suite("interp_main(): Correct Number of Command Line Arguments");
   sput_run_test(test_cla_num_correct);
   sput_enter_suite("interp_main(): Incorrect Number of Command Line Arguments");
   sput_run_test(test_cla_num_incorrect);

   sput_enter_suite("input(): Correct Command Line Arguments");
   sput_run_test(test_cla_correct);
   sput_enter_suite("input(): Inorrect Command Line Arguments");
   sput_run_test(test_cla_incorrect);

   sput_enter_suite("prg_alloc(): Program Allocated");
   sput_run_test(test_ptrogram_allocated);

   sput_finish_testing();

   return sput_get_return_value();
}

int interp_main(int argc){
   if(argc<2||argc>3){
      return 1;
   }
   //input(argc, argv);
   return 0;
}

static void test_cla_num_correct(){
   sput_fail_unless(interp_main(2)==0, "Command Line Arguments == 2");
   sput_fail_unless(interp_main(3)==0, "Command Line Arguments == 3");
}

static void test_cla_num_incorrect(){
   sput_fail_unless(interp_main(1)==1, "Command Line Arguments == 1");
   sput_fail_unless(interp_main(4)==1, "Command Line Arguments == 4");
}

int input(char *argv[]){
   //regex_t reg;
   //regcomp(&reg, "[.]ttl$", 0);
   //if(regexec(&reg, argv[1], 0, NULL, 0)!=0){
   //   return 1;
   //}
   FILE* inp=fopen(argv[1], "r");
   if(inp==NULL){
      return 1;
   }
   if(ftell(inp)==-1L){
      return 1;
   }
   //PRG* p=prg_alloc(inp);
   fclose(inp);
   //LYR* l=prog(p);
   //output(l, argc, argv);
   return 0;
}

static void test_cla_correct(){
   char *argva[]= {"./interp", "./TTLs/5x5.ttl"};
   sput_fail_unless(input(argva)==0, "Command Line Arguments == 5x5.ttl");
   char *argvb[]= {"./interp", "./TTLs/fail_parse_ok_interp.ttl", "fail_parse_ok_interp.txt"};
   sput_fail_unless(input(argvb)==0, "Command Line Arguments == fail_parse_ok_interp.ttl fail_parse_ok_interp.txt");
}

static void test_cla_incorrect(){
   char *argva[]= {"./interp", "5x5.txt"};
   sput_fail_unless(input(argva)==1, "Command Line Arguments == 5x5.txt");
   char *argvb[]= {"./interp", "fail_parse_ok_interp.ps", "fail_parse_ok_interp.txt"};
   sput_fail_unless(input(argvb)==1, "Command Line Arguments == fail_parse_ok_interp.ps fail_parse_ok_interp.txt");
   char *argvc[]= {"./interp", "Garbage"};
   sput_fail_unless(input(argvc)==1, "Command Line Arguments == Garbage");
   char *argvd[]= {"./interp", "Empty.txt"};
   sput_fail_unless(input(argvd)==1, "Empty File == 5x5.ttl");
}

int prg_alloc(){
   PRG* p=(PRG*)calloc(1,sizeof(PRG));
   if(p==NULL){
      return 1;
   }
   //for(int i = 0; i < MAXNUMTOKENS; i++){
      //fscanf(input, "%s", &p->wds[i][0]);
   //}
   p->cw=0;
   free(p);
   return 0;
}

static void test_ptrogram_allocated(){
   sput_fail_unless(prg_alloc()==0, "Program Allocated");
}