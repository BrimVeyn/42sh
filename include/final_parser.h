/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/04 10:46:33 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/04 10:47:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FINAL_PARSER
# define FINAL_PARSER

#include "utils.h"

#include <stdbool.h>
#include <sys/types.h>
#include <termios.h>

#define NO_WAIT 0x0
#define WAIT 0x1
#define IS_CHILD(pid) ((pid == 0) ? true : false)

extern pid_t g_masterPgid;
extern bool is_command_sub;
extern bool g_subshell;

typedef enum TokenType {
	AND_IF,
	OR_IF,
	BANG,
	PIPE,
	LPAREN,
	RPAREN,
	FOR,
	NAME,
	IN,
	WORD,
	CASE,
	ESAC,
	DSEMI,
	IF,
	THEN,
	FI,
	ELIF,
	ELSE,
	WHILE,
	UNTIL,
	LBRACE,
	RBRACE,
	DO,
	DONE,
	ASSIGNMENT_WORD,
	IO_NUMBER,
	LESS,
	LESSAND,
	GREAT,
	GREATAND,
	DGREAT,
	LESSGREAT,
	CLOBBER,
	DLESS,
	DLESSDASH,
	NEWLINE,
	AMPER,
	SEMI,
	END,
	Program,
	Complete_Commands,
	Complete_Command,
	List,
	And_Or,
	Pipeline,
	Pipe_Sequence,
	Command,
	Compound_Command,
	Subshell,
	Compound_List,
	Term,
	For_Clause,
	Name,
	In,
	Wordlist,
	Case_Clause,
	Case_List_Ns,
	Case_List,
	Case_Item_Ns,
	Case_Item,
	Pattern,
	If_Clause,
	Else_Part,
	While_Clause,
	Until_Clause,
	Function_Definition,
	Function_Body,
	Fname,
	Brace_Group,
	Do_Group,
	Simple_Command,
	Cmd_Name,
	Cmd_Word,
	Cmd_Prefix,
	Cmd_Suffix,
	Redirect_List,
	Io_Redirect,
	Io_File,
	Filename,
	Io_Here,
	Here_End,
	Newline_List,
	Linebreak,
	Separator_Op,
	Separatore,
	Sequential_Sep,
} TokenType;

typedef enum {SHIFT, REDUCE, GOTO, ACCEPT, ERROR} Action;

typedef struct TableEntry {
	Action action;
	int value;
}	TableEntry;

typedef struct {
	char *io_number;
	TokenType type;
	char *filename;
} RedirectionP;

typedef struct {
	RedirectionP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} RedirectionL;

typedef struct {
	char **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} StringListL;

typedef struct {
	StringListL **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} StringListVect;

typedef struct {
	RedirectionL *redir_list;
	StringListL	*word_list;
	StringListL *assign_list;
} SimpleCommandP;

typedef struct {
	char *value;
} FilenameP;

typedef struct IFClauseP IFClauseP;
typedef struct ForClauseP ForClauseP;
typedef struct WhileClauseP WhileClauseP;
typedef struct CaseClauseP CaseClauseP;
typedef struct PipeLineP PipeLineP;
typedef struct FunctionP FunctionP;
typedef struct AndOrP AndOrP;
typedef struct ListP ListP;

typedef struct {
	TokenType type;
	RedirectionL *redir_list;
	union {
		SimpleCommandP *simple_command;
		ListP *subshell;
		ListP *brace_group;
		WhileClauseP *while_clause;
		CaseClauseP *case_clause;
		ForClauseP *for_clause;
		IFClauseP *if_clause;
		FunctionP *function_definition;
	};
} CommandP;

struct FunctionP {
	char *function_name;
	CommandP *function_body;
};

typedef struct {
	ListP *list;
	TokenType separator;
} CompleteCommandP;

struct PipeLineP {
	bool banged;
	PipeLineP *next;
	//-----------Job_control-------//
	pid_t pid;
	bool completed;
	bool stopped;
	int status;
	//-----------------------------//
	CommandP *command;
};

struct AndOrP {
	AndOrP *next;
	TokenType separator;
	//-----------Job_control-------//
	size_t id; //job control id
	pid_t pgid; //process group id
	pid_t pid; //master pid
	bool notified;
	bool subshell; //remove notification
	bool bg; //bg/fg
	int sig; //stopped by signo
	bool completed;
	struct termios tmodes; //saved tmodes
	//-----------------------------//
	PipeLineP *pipeline; //first_process
};

struct ListP {
	AndOrP *and_or;
	TokenType separator;
	ListP *next;
};

typedef struct {
	ListP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} ListPVect;

struct IFClauseP {
	ListPVect *conditions;
	ListPVect *bodies;
};

struct WhileClauseP {
	ListP *condition;
	ListP *body;
};

struct ForClauseP {
	char *iterator;
	bool in;
	StringListL *word_list;
	ListP *body;
};

struct CaseClauseP {
	char *expression;
	StringListVect *patterns;
	ListPVect *bodies;
};

typedef struct {
	TokenType type;
	union {
		CaseClauseP *case_clause;
		StringListL *word_list;
		WhileClauseP *while_clause;
		ForClauseP *for_clause;
		IFClauseP *if_clause;
		TokenType separator_op;
		ListP *list;
		AndOrP *and_or;
		PipeLineP *pipeline;
		CommandP *command;
		char *raw_value;
		RedirectionP *redir;
		FilenameP *filename;
		RedirectionL *redir_list;
		SimpleCommandP *simple_command;
		CompleteCommandP *complete_command;
	};
} Tokenn;

typedef struct {
	Tokenn token;
	int state;
} StackEntry;

typedef struct {
	StackEntry **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} TokenStack;

char *tokenTypeStr(TokenType type);
char *actionStr(const Action action);

void		pipelineAddBack(PipeLineP **lst, PipeLineP *new_value);
PipeLineP	*pipelineNew(CommandP *command);

void		andOrAddBack(AndOrP **lst, AndOrP *new_value, TokenType separator);
AndOrP		*andOrNew(PipeLineP *pipeline, TokenType separator);

void		listAddBack(ListP **lst, ListP *new_value);
ListP		*listNew(AndOrP *and_or, TokenType separator);

IFClauseP	*ifClauseNew(ListP *condition, ListP *body);

CaseClauseP *caseClauseNew(StringListL *pattern, ListP *body);
void		caseClauseMerge(CaseClauseP *parent, const CaseClauseP *child);

SimpleCommandP *simpleCommandNew(void);


/*------------------------------DEBUG-----------------------*/
void print_redir_list(const RedirectionL *redir_list);
void print_word_list(const char *title, const StringListL *word_list);
void print_simple_command(const SimpleCommandP *self);
void print_subshell(const ListP *subshell);
void print_brace_group(const ListP *subshell);
void print_for_clause(const ForClauseP *for_clause);
void print_case_clause(const CaseClauseP *case_clause);
void print_command(const CommandP *command);
void print_function(const FunctionP *function);
void print_pipeline(const PipeLineP *pipeline);
void print_andor(const AndOrP *and_or);
void print_list(const ListP *list);
void print_complete_command(const CompleteCommandP *complete_command);
void print_if_clause(const IFClauseP *if_clause);
void print_while_clause(const WhileClauseP *while_clause);
void print_token_stack(const TokenStack *stack);
/*----------------------------------------------------------*/

typedef struct {
	StringListL *env;
	StringListL *set;
	StringListL *local;
	StringListL *positional;
} Vars;

void			parse_input(char *in, char *filename, Vars * const shell_vars);

char			*get_positional_value(const StringListL * const sl, const size_t idx);
char			*get_variable_value(Vars * const shell_vars, char * const name);
void			string_list_add_or_update(StringListL * const sl, const char * const var);
bool			string_list_update(StringListL *sl, const char *var);
void			string_list_append(const StringListL * const sl, char * const var);
void			string_list_clear(StringListL *list);
bool			string_list_remove(StringListL *sl, char *id);
char			*string_list_get_value(const StringListL * const sl, const char * const id);
char			*shell_vars_get_value(const Vars * const shell_vars, char * const id);
void			string_list_print(const StringListL *list);

//FIX: update raw_input when line_continuation

typedef struct {
	size_t line, column, absolute;
} CursorPosition;

typedef struct {
	TokenType *data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} TokenTypeVect;

typedef struct {
	char *filename; //either script name or terminal
	StringStream *raw_input_ss;
	StringStream *input;
	StringStream *peak;
	Vars *shell_vars;
	CursorPosition pos;
	TokenTypeVect *produced_tokens;
} Lex;

Lex *lex_init(const char * const input, char * const filename, Vars * const shell_vars);
char *lexer_get(Lex * const lexer, bool * const error);
char *lexer_peak(Lex * const lexer, bool * const error);
char *lexer_peak_char(Lex * const lexer);

void pretty_error(const Lex *const lexer, char *raw_token);


bool	is_continuable(const TokenType type);
void	line_continuation(const Lex * const lexer);
void	line_continuation_backslash(Lex *lexer, StringStream * const input, CursorPosition * const pos);
void 	pass_whitespace(StringStream * const input, CursorPosition * const pos);

// #define HIGH_FD_MAX 256

typedef struct {
	AndOrP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int	gc_level;
} JobListe;

typedef struct {
	FunctionP **data;
	size_t size;
	size_t capacity;
	size_t size_of_element;
	int gc_level;
} FunctionList;


extern JobListe *g_jobList;
extern FunctionList *g_funcList;

void		gc_move_function(FunctionP *func);
void		gc_move_andor(AndOrP *andor);
FunctionP	*gc_duplicate_function(FunctionP *func);
ListP		*gc_duplicate_list(ListP *list);

void	put_job_background(AndOrP *job, bool add);
void	put_job_foreground(AndOrP *job, int cont);
void	andor_move(AndOrP *job);
void	update_job_status(void);
void	job_wait(AndOrP *job);
void	job_wait_2 (AndOrP *job);
void	job_notification(void);
void	job_killall(void);
int		job_stopped(AndOrP *j);
int		job_completed(AndOrP *j);
int		mark_process(JobListe *list, pid_t pid, int status, bool print);

typedef enum {
	HD_EXPAND,
	HD_NO_EXPAND
} heredoc_mode;

char *here_doc(char *eof, heredoc_mode mode, Vars *shell_vars);

StringListL *do_expansions(const StringListL * const word_list, Vars * const shell_vars, const int options);

typedef enum { O_NONE = 0, O_SPLIT = 1, O_ALLOWNULLS = 2, } ExpansionsOpt;

void execute_complete_command(CompleteCommandP *complete_command, Vars *shell_vars, bool subshell, bool background);
bool execute_builtin(const SimpleCommandP *command, Vars *shell_vars);
#endif // !FINAL_PARSER
