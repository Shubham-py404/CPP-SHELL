#ifndef CELL_H
#define CELL_H

// C++ Standard Library
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <vector>


// POSIX System Headers
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

// Third-Party Dependencies
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

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

#define DEL " \t\r\n\a"

#define p(...) cout << __VA_ARGS__
#define cell_jr 0

typedef struct s_builtin {
  const char *builtin_name;
  int (*foo)(vector<char *> &args);
} t_builtin;

// System & Utility Functions
void Getcwd(char *, size_t);
void PB1();
void PB2();
void load_env_file();

// Terminal Raw Mode & Auto-Complete
void enableRawMode();
void disableRawMode();
std::string get_ai_autocomplete(const std::string &input);

// Built-in Shell Commands
void add_to_history(const std::string &cmd);
int cell_cd(vector<char *> &args);
int cell_env(vector<char *> &args);
int cell_exit(vector<char *> &args);
int cell_time(vector<char *> &args);
int cell_ai(vector<char *> &args);
int cell_cmd(vector<char *> &args);
// Shell Execution & Parsing
std::vector<char *> split_line(const string &line);
void cell_exec(std::vector<char *> &args);
void cell_launch(std::vector<char *> &arg);

// AI Network Function
std::string call_gemini_api(const std::string &prompt,
                            const std::string &api_key);

#endif
