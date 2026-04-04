
# Cell Shell Documentation

## Table of Contents

- [Overview](#overview)
- [Files](#files)
- [Key Components](#key-components)
- [Usage](#usage)
- [Current Limitations](#current-limitations)
- [Future Enhancements](#future-enhancements)

# GITHUB 
- `REPO` - https://github.com/Shubham-py404/CPP-SHELL.git

```bash
init repo - 
echo "# CPP-SHELL" >> README.md
git init
git add README.md
git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/Shubham-py404/CPP-SHELL.git
git push -u origin main
```

```bash
g++ cell.cpp -o cell && ./cell <command >
```


## Overview
Cell is a simple command-line shell implementation written in C++. It provides a basic Read-Eval-Print Loop (REPL) that reads user input, processes it, and displays output with color formatting.

## Files
- `cell.hpp`: Header file containing includes, color definitions, and utility macros
- `cell.cpp`: Main implementation file with the shell logic
- `inst.md`: Installation and setup instructions

## Key Components

### Header File (cell.hpp)

#### Includes
- `<iostream>`: For input/output operations
- `<unistd.h>`: Provides `fork()` and `execvp()` functions for process management (Linux systems)
- `<sys/wait.h>`: Contains `wait()` function for process synchronization
- `<cstdlib>`: Defines `EXIT_SUCCESS` constant
- `<vector>`: For dynamic arrays (included but not used in current implementation)
- `<string>`: For string operations (included but not used in current implementation)

#### Color Definitions
ANSI escape codes for terminal text coloring:
- `YELLOW`: Yellow text color
- `RED`: Red text color
- `CYAN`: Cyan text color
- `GREEN`: Green text color
- `RST`: Reset to default color

#### Utility Macro
- `p(...)`: A convenience macro that wraps `cout << __VA_ARGS__` for printing with automatic newline

### Implementation File (cell.cpp)

#### cell_read_line() Function
**Purpose**: Reads a single line of input from the user via stdin.

**Parameters**: None

**Return Value**: 
- `char*`: Pointer to the input string on success
- `NULL`: On EOF or error

**Behavior**:
1. Displays a green prompt "$$> " to indicate readiness for input
2. Uses `getline()` to read input from stdin
3. Handles EOF condition (Ctrl+D) by printing "[EOF]" in red
4. Handles getline errors by printing "Getline failed" in red
5. Echoes the entered text in yellow for confirmation
6. Returns the buffer containing the input

#### main() Function
**Purpose**: Implements the main REPL loop of the shell.

**Parameters**:
- `ac`: Argument count (unused)
- `av`: Argument vector (unused)

**Behavior**:
1. Enters an infinite loop
2. Calls `cell_read_line()` to get user input
3. If input is NULL (EOF/error), breaks out of loop
4. Prints the input line
5. Frees the memory allocated by `getline()`
6. Continues loop for next input

## Usage
Compile and run the shell:
```bash
g++ cell.cpp -o cell
./cell
```

The shell will display a green "$$> " prompt. Type commands or text, and it will echo them back in yellow. Press Ctrl+D to exit.

## Current Limitations
- Does not execute commands - only echoes input
- No command parsing or execution logic
- Basic error handling
- Linux-specific includes (unistd.h, sys/wait.h)

## Future Enhancements
- Command parsing and execution
- Built-in commands (cd, exit, etc.)
- Command history
- Tab completion
- Cross-platform compatibility
# CPP-SHELL
