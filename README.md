# CPP-SHELL (Cell Shell)

Welcome to **Cell**, a custom, AI-augmented command-line shell written in C++. It features a standard Read-Eval-Print Loop (REPL), parses user input, executes system commands, and deeply integrates with the Gemini API to offer AI-powered assistance, natural language command generation, and context-aware auto-completion.

---

## File Structure & Roles

*   **`cell.hpp`**: The main header file. It declares function prototypes, includes necessary POSIX/C++ standard libraries and third-party dependencies (`libcurl`, `nlohmann/json`), and defines terminal ANSI colors and the `t_builtin` struct for mapping commands.
*   **`cell.cpp`**: The core shell engine. Contains the REPL (`main()` loop), raw terminal input handling for live AI autocomplete (ghost text), tokenization logic, and the execution engine that spans child processes.
*   **`builtin.cpp`**: Implements custom built-in shell commands (e.g., `cd`, `exit`, `time`) as well as the specialized AI integration commands (`ai` and `cmd`).
*   **`utils.cpp`**: Contains helper utilities for making HTTP network requests to the Gemini API, parsing the `.env` file, toggling terminal raw mode, tracking history, and building AI context.
*   **`Makefile`**: A build configuration file that automates the compilation process using `g++` and links the required `libcurl` dependency.
*   **`json.hpp`**: A single-header library (`nlohmann/json`) used to parse JSON responses from the Gemini API and structure JSON payloads.

---

## System Utilities & POSIX Functions

Cell leverages various system-level utilities and POSIX (Unix-like) functions to emulate standard terminal behaviors:

*   **`make`**: A build automation tool that reads the `Makefile` to compile the `.cpp` files into object files and link them into the final `cpp-shell` executable. It speeds up recompilation by only building files that have changed.
*   **`fork()`**: A POSIX system call that creates a new process (child) by duplicating the current process (parent). Cell uses this when it needs to run a command like `ls` or `grep` without terminating the shell itself.
*   **`execvp()`**: Used inside the child process created by `fork()`. It replaces the child's process image with a new executable program, searching the system's `PATH` to find it.
*   **`waitpid()`**: Used by the parent shell process to wait for the child process to finish executing before re-drawing the `$$>` prompt.
*   **`tcgetattr()` / `tcsetattr()`**: Functions from `<termios.h>` used to manipulate terminal attributes. Cell uses these to toggle "raw mode", which disables line-buffering and local echo, allowing the shell to instantly intercept keypresses (like `TAB` for autocomplete).
*   **`popen()` / `pclose()`**: Used to open a pipe to a command, allowing the shell to execute a process and silently capture its `stdout` output (used heavily to fetch context for the AI).
*   **`system()`**: Executes a command via the host system's default shell (`/bin/sh`). Used by the `cmd` function to safely execute AI-generated commands containing pipes (`|`) or redirects (`>`).
*   **`libcurl`**: A client-side URL transfer library used to perform synchronous HTTP POST requests to the Google Gemini API.

---

## Functions Summary

### `cell.cpp`
*   **`cell_read_line()`**: Reads user input character-by-character in raw mode, calculates terminal wraps, handles backspaces, and fetches/renders AI autocomplete on `TAB`.
*   **`split_line()`**: Parses the user's raw input string and tokenizes it into an array of arguments, respecting single/double quotes.
*   **`cell_exec()`**: Determines if a command is a built-in. If it is, it routes to the corresponding C++ function; otherwise, it delegates to `cell_launch()`.
*   **`cell_launch()`**: Uses `fork()` and `execvp()` to execute external system programs, handling "command not found" errors.
*   **`main()`**: The shell's entry point. Loads the environment, prints the welcome banner, and runs the infinite REPL loop.

### `builtin.cpp`
*   **`cell_cd()`**: Built-in command to change directories, featuring support for `~` (home directory) expansion.
*   **`cell_exit()`**: Built-in command to gracefully terminate the shell, displaying an animated progress bar.
*   **`cell_env()`**: Prints out all environment variables inherited by the shell.
*   **`cell_time()`**: Fetches and prints the system's current local time.
*   **`cell_ai()`**: Takes natural language text from the user, prompts the Gemini API, and prints the AI's conversational response.
*   **`cell_cmd()`**: Translates a user's natural language request into a valid terminal command, displays it, and asks for `(y/n)` confirmation before executing it.

### `utils.cpp`
*   **`Getcwd()`**: A wrapper around `getcwd()` that safely fetches and prints the current working directory.
*   **`PB1()` / `PB2()`**: Utility functions that print the ASCII art banners during startup.
*   **`WriteCallback()`**: A required callback function used by `libcurl` to append network response chunks into a C++ string buffer.
*   **`call_gemini_api()`**: Configures `libcurl`, constructs the JSON payload, executes the API request, and parses the returned JSON safely.
*   **`load_env_file()`**: Reads a local `.env` file and uses `setenv()` to securely load variables (like `GEMINI_API_KEY`) into the process environment.
*   **`enableRawMode()` / `disableRawMode()`**: Configures the terminal to intercept individual keystrokes or restores standard canonical behavior.
*   **`add_to_history()`**: Adds successful commands to a vector tracking the current session's command history.
*   **`get_system_context()`**: Silently runs commands (like `ls` or `git branch`) via `popen()` to build contextual awareness for the AI.
*   **`get_ai_autocomplete()`**: Gathers the system context, session history, and user input, builds a highly specific prompt, and queries Gemini to provide shell autocomplete suggestions (ghost text).
