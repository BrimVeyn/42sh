#ifndef MINISHELL_STRUCT_H
# define MINISHELL_STRUCT_H

enum type_Token {
	//---------------
	REDIRECTION,
	COMMAND,
	ARGS, 
	EXPANSION, 
	PATTERN_MATCHING,
	//---------------
	SEPARATOR,
	//---------------
	PARSING_ERROR,
};

enum type_of_separator {
	S_AND,//&&
	S_OR, //||
	S_PIPE, //|
	S_SEMI, //;
};

enum type_redirection {
	R_INPUT_FILE, //<
	R_OUTPUT_FILE, //>
	R_APPEND_FILE, //>>
	R_HERE_DOC, //<<
	R_AMPERSAND, //<& //>&
};

enum type_command {
	C_BUILTIN, //echo, etc...
	C_SHELL, //grep, etc...
};

enum type_of_pattern_matching {
	STAR,
	QUESTION_MARK,
	EXCLAMATION,
};

struct Range {
	int s;
	int e;
};

union Token {
	struct {
		enum type_redirection type;
		struct Range range;
	} redir; //redirection

	struct {
		enum type_command type;
		struct Range range;
	} cmd; //shell command

	struct {
		struct Range range;
	} exp; //shell expansion
	
	struct {
		enum type_of_separator type;
		struct Range range;
	} sep;

	struct {
		enum type_of_pattern_matching type;
		struct Range range;
	}
};

struct Node {
	union Token token;
	struct Node *left;
	struct Node *right;
};

struct Utils {
	int temp;
	//ptr to utils AST functions;
	//build;
	//print;
	//deinit;
};

struct AST {
	struct Node *root;
	char		*input;
	struct Utils func;
};

#endif // !MINISHELL_STRUCT_H
