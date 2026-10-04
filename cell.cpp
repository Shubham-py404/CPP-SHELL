#include "cell.hpp"

t_builtin g_builtin[] = {
    {.builtin_name = "env", .foo = cell_env},
    {.builtin_name = "exit", .foo = cell_exit},
    {.builtin_name = "time", .foo = cell_time},
    {.builtin_name = "cd", .foo = cell_cd},
    {.builtin_name = "ai", .foo = cell_ai},
    {.builtin_name = "cmd", .foo = cell_cmd},
    {.builtin_name = NULL, .foo = nullptr},
};

int status = 0;

void cell_launch(vector<char *> &arg) {
  int pid = fork();
  if (pid == 0) {
    // Restore default Ctrl+C behavior for the child process
    signal(SIGINT, SIG_DFL);
    if (execvp(arg[0], arg.data()) == -1) {
      cout << RED << "Command not found : " << arg[0] << "\n" << RST;
      exit(EXIT_FAILURE);
    }
  } else if (pid < 0) {
    cout << RED << "Failed to fork process\n" << RST;
  } else {
    // Properly wait for the specific child process
    waitpid(pid, &status, WUNTRACED);
  }
}

void cell_exec(vector<char *> &args) {
  if (args.empty() || args[0] == nullptr)
    return;
  int i = 0;
  const char *curr;
  // Add extra parentheses to silence the assignment warning
  while ((curr = g_builtin[i].builtin_name)) {
    if (!strcmp(curr, args[0])) {
      status = g_builtin[i].foo(args);
      return;
    }
    ++i;
  }
  cell_launch(args);
}

string cell_read_line() {
  if (!isatty(STDIN_FILENO)) {
    string line;
    if (!getline(cin, line))
      return "__CELL_EOF__";
    return line;
  }

  char cwd[BUFSIZ] = {0};
  Getcwd(cwd, sizeof(cwd));
  string prompt_str = string(C) + string(cwd) + " $$> " + string(RST);
  int prompt_len = string(cwd).length() + 5; // Visible characters in prompt

  cout << prompt_str << flush;
  enableRawMode();

  string line = "";
  string ghost_text = "";
  char c;
  ssize_t bytes_read;

  while ((bytes_read = read(STDIN_FILENO, &c, 1)) == 1) {
    // Fetch current terminal width dynamically
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int cols = w.ws_col;
    int total_len = prompt_len + line.length();

    if (c == '\n' || c == '\r') {
      cout << "\033[K\n" << flush; // Clear ghost text on enter
      break;

    } else if (c == 127 || c == '\b') {
      ghost_text = ""; // Clear prediction on backspace
      if (!line.empty()) {
        line.pop_back();
        // If cursor is at the exact left edge of the terminal, manually move UP
        // and to the FAR RIGHT
        if (total_len % cols == 0) {
          cout << "\033[1A\033[" << cols << "C \b\033[K" << flush;
        } else {
          cout << "\b \b\033[K" << flush;
        }
      }

    } else if (c == 3) {
      cout << "\033[K^C\n" << flush;
      line = "";
      break;
    } else if (c == 4) {
      if (line.empty()) {
        disableRawMode();
        return "__CELL_EOF__";
      }
      continue;
    } else if (c == 27) {
      char seq[2];
      if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
          read(STDIN_FILENO, &seq[1], 1) == 1)
        continue;

    } else if (c == '\t') {
      if (!line.empty()) {
        if (ghost_text.empty()) {
          // STAGE 1: Fetch and display ghost text
          cout << "\n\033[1;90m[AI Thinking...]\033[0m" << flush;
          ghost_text = get_ai_autocomplete(line);

          // Redraw prompt and input cleanly
          cout << "\r\033[1A\033[2K" << prompt_str << line << flush;

          if (!ghost_text.empty()) {
            // Print ghost text in grey
            cout << "\033[90m" << ghost_text << "\033[0m" << flush;
            // Manually backspace the cursor behind the ghost text, respecting
            // multiline wrap
            for (size_t i = 0; i < ghost_text.length(); ++i) {
              int current_pos =
                  prompt_len + line.length() + ghost_text.length() - i;
              if (current_pos % cols == 0)
                cout << "\033[1A\033[" << cols << "C" << flush;
              else
                cout << "\b" << flush;
            }
          }
        } else {
          // STAGE 2: Tab pressed again - Accept ghost text
          line += ghost_text;
          cout << "\033[0m" << ghost_text << "\033[K" << flush;
          ghost_text = "";
        }
      }
    } else {
      // Standard typing clears ghost text
      if (!ghost_text.empty()) {
        cout << "\033[K" << flush;
        ghost_text = "";
      }
      line += c;
      cout << c << flush;

      // Handle typing past the right edge gracefully
      if ((prompt_len + line.length()) % cols == 0) {
        cout << " \b" << flush; // Force terminal wrap
      }
    }
  }

  disableRawMode();
  return line;
}

vector<char *> split_line(const string &line) {
  vector<char *> tokens;
  string current_token;
  bool in_quotes = false;
  char quote_char = '\0';
  bool token_has_content =
      false; // Tracks if a token exists, even if it is explicitly empty ("")

  for (char c : line) {
    if (in_quotes) {
      if (c == quote_char) {
        in_quotes = false;
      } else {
        current_token += c;
      }
      token_has_content = true; // Inside quotes implies explicit content
    } else {
      if (c == '"' || c == '\'') {
        in_quotes = true;
        quote_char = c;
        token_has_content = true; // Starting a quote implies explicit content
      } else if (isspace(c)) {
        if (token_has_content || !current_token.empty()) {
          tokens.push_back(strdup(current_token.c_str()));
          current_token.clear();
          token_has_content = false; // Reset for the next token
        }
      } else {
        current_token += c;
        token_has_content = true;
      }
    }
  }

  if (token_has_content || !current_token.empty()) {
    tokens.push_back(strdup(current_token.c_str()));
  }
  tokens.push_back(nullptr);

  return tokens;
}

int main() {
  load_env_file();
  PB1();
  // Prevent Ctrl+C from killing the parent shell
  signal(SIGINT, SIG_IGN);

  string line;
  while (true) {

    line = cell_read_line();
    if (line == "__CELL_EOF__")
      break;

    vector<char *> arg = split_line(line);
    add_to_history(line);
    cell_exec(arg);
    for (char *token : arg) {
      if (token != nullptr) {
        free(token);
      }
    }
  }
  return EXIT_SUCCESS;
}