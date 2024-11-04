/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   final_parser.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 16:46:24 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/04 11:47:11 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FINAL_PARSER
# define FINAL_PARSER

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
	Separator,
	Sequential_Sep,
} TokenType;

typedef enum {SHIFT, REDUCE, GOTO, ACCEPT, ERROR} Action;

typedef struct TableEntry {
	Action action;
	int value;
}	TableEntry;

char *tokenTypeStr(TokenType type);
char *actionStr(const Action action);

typedef enum {LEX_SET, LEX_GET, LEX_PEAK, LEX_PEAK_CHAR, LEX_DEBUG} LexMode;
char *lex_interface(LexMode mode, void *input);

#endif // !FINAL_PARSER
