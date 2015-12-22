// Julian Segert and Ziting Shen. 11/15/15
// This program runs a shell for executing programs in the 
// background or foreground
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include "parsecmd.h"

#define MAXHIST 10   // max number of jobs at any point in time 


// global variables: add only globals for history list state
//                   all other variables should be allocated on the stack
// static: means these variables are only in scope in this .c file
// history: a circular list of the most recent command input strings.  
//          (note: there are MAXHIST elements, each element is a
//           string (a statically declared char array of size MAXLINE))
//          (remember: you can use the strcpy function to copy one string
//          to another)
static char history[MAXHIST][MAXLINE]; 
static int history_next=0;
static int history_size=0;




void foreground(char **argv);
void background(char **argv);
void historyAdd(char *cmdline);
void historyPrint();
int num(char *cmdline, char **argv, int *bg);
void sigchld_handler(int sig);

/******************************************************************/
int main( ){ 

  char cmdline[MAXLINE];
  char *argv[MAXARGS];
  int bg;

  //declare SIGCHLD handler
  signal(SIGCHLD, sigchld_handler);

  while(1) {
    // (1) print the shell prompt
    printf("cs31shell> ");  
    fflush(stdout);

    // (2) read in the next command entered by the user
    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
      perror("fgets error");
    }
    if (feof(stdin)) { /* End of file (ctrl-d) */
      fflush(stdout);
      exit(0);
    }

    //printf("DEBUG: %s\n", cmdline);
    
    if(strcmp(cmdline, "\n") == 0)
      continue;

    // (3) make a call to parse_cmd function to parse it into its
    //     argv format

    // return decides whether process is in the background
    bg = parse_cmd(cmdline, argv);

    //check for !num input
    if(cmdline[0] == '!') {
      if(num(cmdline, argv, &bg) < 0) {
        printf("%s: event not found\n", argv[0]);
        fflush(stdout);
        continue;
      }
    }

    historyAdd(cmdline);
      
    // (4) determine how to execute it, and then execute it
    //
    if(strcmp(argv[0], "exit") == 0) {
      printf("bye bye\n");
      fflush(stdout);
      exit(0);
    }
    if(strcmp(argv[0], "history") == 0) {
      historyPrint();
      continue;
    }
    if(!bg) {
      foreground(argv);
    } else {
      background(argv);
    }
  }
  return 0;
}
/******************************************************************/
/* sigchld_handler
Called in response to sichld
Cleans up all zombie children
*/
void sigchld_handler(int sig) {
  int status;
  waitpid(-1, &status, WNOHANG);
}

/* runs a command in the foreground
Takes the array of command line args
Returns nothing
*/
void foreground(char **argv) {
  int status;
  pid_t pid;
  pid = fork();

  if(pid == 0) {
    if (execvp(argv[0], argv)<0){
      printf("%s:Command not found.\n", argv[0]);
      fflush(stdout);
      exit(0);
    }
  } else {
    waitpid(pid, &status, 0);
  }
  
}

/* Runs a program in the background, does not wait for child
Takes the array of command line args
Returns nothing
*/
void background(char **argv) {
  pid_t pid = fork();

  if(pid == 0) {
    if (execvp(argv[0], argv)<0){
      printf("%s:Command not found.\n", argv[0]);
      fflush(stdout);
      exit(0);
    }
  } 
}

/* Adds a string to the history circular queue
Takes a pointer to the inputted cmdline
Uses global variables
Returns nothing
*/
void historyAdd(char *cmdline) {
  strcpy(history[history_next], cmdline);
  history_next = (history_next + 1) % MAXHIST;
  history_size++;
}

/* Print out the stored items in history
No parameters, uses global variables
Returns nothing but prints to terminal
*/
void historyPrint() {
  int i;
  if(history_size < MAXHIST) {
    for(i = 0; i < history_size; i++) {
      printf("%d %s\n", i, history[i]);
      fflush(stdout);
    }
  } else {
    for(i = history_next; i < history_next + MAXHIST; i++){
      printf("%d %s\n", history_size - MAXHIST + i - history_next, 
          history[i % MAXHIST]);
      fflush(stdout);
    }
  }
  printf("\n");
  fflush(stdout);
}
/* This function handles the !num commands
 Parameters: a a pointers to cmdline, the argv array, and bg
 Returns: 0 on success, -1 if command not found
 Modifies inputted parameters
*/
int num(char *cmdline, char **argv, int *bg) {
  int inputid;
  char *ptr;

  //change string input to number
  inputid = strtol(&argv[0][1], &ptr, 10);

  if(history_size <= MAXHIST){
    if((inputid >= 0) && (inputid < history_size)) {//check bounds
      strcpy(cmdline, history[inputid]);
    } else {
      return -1; //failed
    }
  } else {
    if(inputid >= (history_size - MAXHIST) && (inputid < history_size)){
      //fetch command at corresponding queue index
      strcpy(cmdline, history[(history_next - history_size + MAXHIST 
            + inputid) % MAXHIST]); 
    } else {
      return -1;
    }
  }
  *bg = parse_cmd(cmdline, argv);//parse command
  return 0;
}
