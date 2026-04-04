#ifndef CELL_H 
#define CELL_H 

#include <iostream> 
#include <unistd.h>  // required for fork() and execvp() used in linux systems
#include <sys/wait.h> // Required for wait()
#include <cstdlib>    // Required for EXIT_SUCCESS
#include <vector> 
#include <string>
/*
** ANSI color code for terminal output formatting
 Y- yellow
  RED- red
  C- cyan
  G- green
  RST - reset to default color
*/

#define YELLOW "\033[1;33m"
#define RED "\033[1;31m"
#define CYAN "\033[1;36m"
#define GREEN "\033[1;32m"
#define RST "\033[0m"

#define p(...) cout << __VA_ARGS__   // it is used for printing output to the terminal with a newline at the end

#endif