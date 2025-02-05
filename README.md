![IMG](https://github.com/BrimVeyn/42sh/blob/main/gifs/final_grade.png)

# 42sh - A POSIX-Compliant Shell

## Overview

42sh is a custom shell implementation built as part of the 42 school curriculum. It aims to replicate and extend the functionality of standard POSIX shells while supporting advanced features such as job control, command substitution, and custom parsing.

## Features

### ✅ Core Shell Features

- Command execution (builtins and external commands)
- Argument parsing with proper tokenization
- Full edition of command line
- History managment
- Expansions (described below)
- Control flows (for, if, until, while, case, function)
- Handling of environment variables
- Handling of exit statuses

### 🔗 Operators & Redirections

- **Pipes (`|`)**: Connect command outputs to inputs.
- **I/O Redirections (`<`, `>`, `>>`, `2>`, `&>`)**: Manage input and output streams.
- **Logical Operators (`&&`, `||`)**: Conditional execution based on exit codes.
- **Subshells (`(...)`)**: Execute commands in a separate process.
- **Command Grouping (`{ ... }`)**: Execute multiple commands in the current shell.

### 🔄 Expansions & Substitutions

- **Variable Expansion (`$VAR`)** (Advanced formats supported)
- **Positional Parameters (`$1`, `$2`, etc.)**
- **Arithmetic Expansion (`$((expr))`)**
- **Command Substitution (`$(command)`)** (No support for legacy `` `command` `` syntax)
- **History Expansion (`!!`, `!$`, `!n`)**
- **Pattern Matching (`*`, `?`, `[...]`, POSIX character classes)**
- **Tilde Expansion (`~`, `~user`)**
- **Process Substitution (`<(command)`, `>(command)`)**

### 📌 Job Control

- **Foreground and Background Execution (`&`, `fg`, `bg`)**
- **Listing Jobs (`jobs`)**
- **Signal Handling**: Handles all signals properly
- **Proper Process Group Management**

### 🛠️ Built-in Commands

- `alias` - Manage command aliases
- `bg` - Resume a job in the background
- `cd` - Change directory
- `echo` - Print to stdout
- `env` - Display environment variables
- `exit` - Exit the shell
- `export` - Set environment variables
- `fc` - Fix command (history manipulation)
- `fg` - Resume a job in the foreground
- `hash` - Manage command lookup hash table
- `jobs` - Display active jobs
- `pwd` - Print working directory
- `return` - Return from a function
- `set` - Set shell options
- `test` - Evaluate expressions
- `type` - Display command type
- `unalias` - Remove command aliases
- `unset` - Unset environment variables

### 🖊️ Command Line Editing

42sh includes a **custom readline library remake** that supports:

- **Full command-line editing**
- **Readline shortcuts** (e.g., `Ctrl+A`, `Ctrl+E`, etc.)
- **Vi mode** (`set -o vi` to enable full vi-like editing)

### ⚙️ Customization

- The shell loads a **configuration file (`~/.42shrc`)** at startup.
- Users can **define aliases** and customize their **prompt** in this file.
- Supports full **prompt customization** using all syntax elements described in the `Controlling the Prompt` section of Bash's manual (`PS1` and `PS2` customization).

### 🔁 POSIX Control Flows

- Fully supports POSIX **control structures**, including:
  - `if`, `while`, `for`, `until`, `case`, `switch`
  - Function definitions and execution

## Internal Logic

### 🔍 Parsing and Tokenization

42sh uses a **LALR(1) parser** for parsing complex shell grammar. Since parser generators like Bison or Yacc were not allowed, the parser was implemented manually. The automaton was generated using **jsmachines' project**, and the C code was dynamically generated from the automaton using **Python**.

Additionally, we implemented our own tiny regex library to ease parsing. The API is:

```c
regex_match(format, target)
```

Supported operators:

```
'^ ? + * [^a-z] $'
```

### 🏗️ Execution Flow

1. **Lexing & Parsing**: The command line is tokenized and parsed into an AST.
2. **Expansion & Substitution**: Variables, command substitution, and arithmetic expressions are expanded before execution.
3. **Execution Dispatch**:
   - Built-in commands are executed directly.
   - External commands are forked and executed using `execve`.
4. **Job Control**:
   - Foreground and background processes are managed properly.
   - Process groups and signals are handled to ensure correct job behavior.
5. **Redirections & Pipes**:
   - File descriptors are modified as needed before execution.
   - Pipes are created dynamically between commands in a pipeline.

### ⚙️ Memory Management

- Uses a **custom garbage collector** to handle memory allocations dynamically.
- Implements a **growable arena allocator** to efficiently manage memory usage.
- Avoids memory leaks through automatic deallocation of structures after execution.
- Includes a **header library for generic vector creation**, making it easier to manage dynamic arrays.

## Installation & Usage

```sh
# Clone the repository
git clone https://github.com/BrimVeyn/42sh.git
cd 42sh

# Build the shell
make

# Run the shell
./42sh

# Run tests
make test  # Runs more than 1100 tests to validate the functionalities described in this README
```

## Tester Overview

42sh includes a dedicated test suite that ensures correctness across all functionalities. The tester consists of:

- **Backend**: Implemented in Express.js
- **Frontend**: Built with React
- **Testing Execution**: When running `make test`, Docker Compose is used to:
  - Build and launch the test environment
  - Run the tests using a multithreaded program
  - Provide a link to visualize test outputs in a web browser
 
![GIF](https://github.com/BrimVeyn/42sh/blob/main/gifs/tester_demo.gif)

## Useful Links & Resources

- [LALR(1) Parser Generator](https://jsmachines.sourceforge.net/machines/lalr1.html)
- [GNU Bash Manual](https://www.gnu.org/software/bash/manual/bash.html)
- [POSIX Specification](https://pubs.opengroup.org/onlinepubs/9699919799.2018edition/)
- [Stanford CS143](http://web.stanford.edu/class/cs143/)
- [Crafting Interpreters](https://craftinginterpreters.com/)
- [YouTube: Bash Parsing Overview](https://www.youtube.com/watch?v=TZ5a3gCCZYo)
- [YouTube: Shell Job Control](https://www.youtube.com/watch?v=IroPQ150F6c)
- [Efficient C Code Optimization](https://www.codeproject.com/Articles/6154/Writing-Efficient-C-and-C-Code-Optimization)
- [Tiny Garbage Collector](https://github.com/orangeduck/tgc/tree/master)
- [Job Control and Termios](https://blog.nelhage.com/2010/01/a-brief-introduction-to-termios-signaling-and-job-control/)
- [Beautiful Code - Princeton](https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html)
- [GNU Libc Job Control](https://www.gnu.org/software/libc/manual/html_node/Job-Control.html)

## License

MIT License

## Authors

- Byan VAN PAEMEL
- Nathan BARDAVID

