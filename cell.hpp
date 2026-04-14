#ifndef CELL_H 
#define CELL_H 

#include <iostream> 
// #ifdef __unix__
#include <unistd.h>  // required for fork() and execvp() used in linux systems
#include <sys/wait.h> // Required for wait()
// #endif
#include <cstdlib>    // Required for EXIT_SUCCESS
#include <vector> 
#include <cstdio>
#include <string>
#include <sstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <ctime> 
using namespace std ; 
/*
** ANSI color code for terminal output formatting
 Y- yellow
  RED- red
  C- cyan
  G- green
  RST - reset to default color
*/

#define Y "\033[1;33m"
#define RED "\033[1;31m"
#define C "\033[1;36m"
#define G "\033[1;32m"
#define BL "\033[1;30m"
#define B "\033[1;34m"
#define RST "\033[0m"

#define DEL "\t \v\f\r\n"
#

#define p(...) cout << __VA_ARGS__     // it is used for printing output to the terminal with a newline at the end
#define cell_jr 0 
typedef struct s_builtin{
  const char* builtin_name ; 
  int (*foo)(vector<char*> &args) ;
} t_builtin ; 

void Getcwd(char * ,size_t ) ; 
void PB1() ; 
void PB2() ;
int cell_exit(vector <char*> &) ; 
int cell_time(vector<char*> &);
int cell_env(vector<char*> &);

#endif