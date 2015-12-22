//  tester program for CS31 lab 11: parsecmd library
//
// Julian Segert and Ziting Shen. CS31 lab 8.
//
// note: the call to parse_cmd_dynamic is commented out here
//
#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<unistd.h>
#include <sys/types.h> 
#include"parsecmd.h"

// static: means these functions cannot be called from outside this file
static void print_cmd_args(char *cmdline, char **argv );
static void print_bg(int bg) ;

/******************************************************************/
int main( ){ 

  int bg;
  char cmdline[MAXLINE];
  char *argv[MAXARGS], **args;
  int i;

  printf("Enter quit to stop\n");
  while(1) {
    // (1) print the shell prompt
    printf("enter a cmd line> ");  
    fflush(stdout);

    // (2) read in the next command
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
      perror("fgets error");
    if (feof(stdin)) { /* End of file (ctrl-d) */
      fflush(stdout);
      exit(0);
    }

    /*
    bg = parse_cmd(cmdline, argv);

    if((argv[0]) && (strcmp(argv[0], "quit") == 0) ){
      break;
    }
    print_cmd_args(cmdline, argv);
    print_bg(bg);
    */

    bg = 0;
    args = NULL;
    
    args = parse_cmd_dynamic(cmdline, &bg);
    if(args) {
      print_cmd_args(cmdline, args);
      print_bg(bg);

      if((args[0]) && (strcmp(args[0], "quit")) == 0){
        i = 0;
        while(args[i] != NULL) {//free each bucket
          free(args[i++]);
        }
        free(args); //free the array
        break;
      }
      i = 0;
      while(args[i] != NULL) {
        free(args[i++]);
      }
      free(args);
    }
  }
  return 0;
}
/*******************************************************************/
// This function prints out a message based on the value of bg
// indicating if the command is run in the background or not
void print_bg(int bg) {
      if(bg) { 
        printf("run in the background is true\n");
      } else { 
        printf("run in the background is false\n");
      }
}
/*******************************************************************/
// This function prints out the cmdline and argv list
//   cmdline: the command line string
//   argv: the argv list of command line argumetns string
//
void print_cmd_args(char *cmdline, char **argv ){

  int i = 0;

  printf("\nCommand line: %s\n", cmdline);

  while (argv[i] != NULL) {
    // note: I'm printing out each argv string between # chars
    //       so that I can see if I have left any space or other 
    //       invisible characters in an argv[i] string (I shouldn't)
    printf("%3d  #%s#\n", i, argv[i]);
    i++;
  }
}
