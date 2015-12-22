//
// Julian Segert and Ziting Shen. CS31 lab 8.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parsecmd.h"

// for parse_cmd: a copy of the command line to make argv parm point to
// a static global variable means private to code in this file
static char cmdline_copy[MAXLINE];

/* parse_cmd. Takes the command line string and finds tokens.
Parameters: t, the command line string, argv, a pointer to the array of args
Returns: 1 if program runs in background, 0 otherwise
*/
int parse_cmd(const char *t, char **argv) {
  int start, end;
  int len;
  int argv_counter;
  
  len = strlen(t);
  start = 0;
  argv_counter = 0;
  strcpy(cmdline_copy, t);

  
  while(start < len) {
    //find start of the token
    for(; isspace(t[start]) && (start < len); start++) {}
    //find end of the token
    for(end = start; !isspace(t[end]) && (end < len); end++) {}
    if(start >= len - 1) break;
    argv[argv_counter] = (char *) &cmdline_copy[start];
    argv[argv_counter][end - start] = '\0';//add terminator
    argv_counter++;
    start = end + 1;
  }

  argv[argv_counter] = NULL;
  if((argv_counter > 0) && (strcmp(argv[argv_counter - 1], "&") == 0)) {
    //if last token in &, remove it from the list and return 1
    argv[argv_counter - 1] = NULL; //the loop overcounts by one
    return 1;
  }
  return 0;
}

/* parse_cmd_dynamic. Finds command line tokens with dynamic memory allocation
Parameters: cmdline, the command line string, bg, a pointer to the background
indicator
Returns: a pointer to the array of tokens, end marked with NULL
*/
char **parse_cmd_dynamic(const char *cmdline, int *bg){
  char **argv;
  int start, end;
  int len;
  int counter, i;
  int argv_counter;

  len = strlen(cmdline);
  start = 0;
  counter = 0;
  argv_counter = 0;

  //count arguments to allocate space
  for(i = 0; i < len; i++) {
    if(isspace(cmdline[i])) continue;
    if(i == 0){
      counter++;
      continue;
    }
    if(isspace(cmdline[i - 1])) counter++;
  }

  //allocate space for array including NULL at the end
  argv = (char **) malloc(sizeof(char *)*(counter + 1));

  //loop as before
  while(start < len) {
    for(; isspace(cmdline[start]) && (start < len); start++) {}
    for(end = start; !isspace(cmdline[end]) && (end < len); end++) {}
    if(start >= len - 1) break;

    //allocate space for each string
    argv[argv_counter] = (char *) malloc(sizeof(char)*(end - start + 1));
    strncpy(argv[argv_counter], &cmdline[start], end - start);
    argv[argv_counter][end - start] = '\0';

    argv_counter++;
    start = end + 1;
  }

  //change bg depending on whether last argument is &
  if(argv_counter > 0 && (strcmp(argv[argv_counter - 1], "&") == 0)) {
    free(argv[argv_counter - 1]);
    argv[argv_counter - 1] = NULL;
    *bg = 1;
  } else {
    argv[argv_counter] = NULL;
    *bg = 0;
  }
  return argv;
}
