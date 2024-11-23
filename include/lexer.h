/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 14:44:10 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/23 22:56:41 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


typedef enum {
	WORD_WORD,
	WORD_CMD_SUB,
	WORD_SUBSHELL,
	WORD_PARAM,
	WORD_ARITHMETIC,
	WORD_ARITHMETIC_PAREN,
	WORD_SINGLE_QUOTE,
	WORD_DOUBLE_QUOTE,
	NONE,
} WordContext;

typedef struct {
	unsigned char bit0 : 1; //DOUBLE_QUOTE
	unsigned char bit1 : 1; //SINGLE_QUOTE
	unsigned char bit2 : 1; //ARITHMETIC_PAREN
	unsigned char bit3 : 1; //ARITHMETIC
	unsigned char bit4 : 1; //PARAM
	unsigned char bit5 : 1; //SUBSHELL
	unsigned char bit6 : 1; //CMD_SUB
	unsigned char bit7 : 1; //WORD ignored
} BitMap8;

typedef struct {
	char *start;
	char *end;
	BitMap8 bitmap;
} WordContextBounds;

typedef struct {
	char	*data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} WordContextList;

#define WORD_MAP (BitMap8) {0, 1, 0, 1, 1, 0, 1, 1}
#define PARAM_MAP (BitMap8) {0, 0, 0, 1, 0, 0, 0, 0}
#define CMD_SUB_MAP (BitMap8) {0, 1, 1, 1, 1, 0, 1, 1}
#define SUBSHELL_MAP (BitMap8) {0, 1, 1, 1, 1, 0, 1, 1}
#define ARITHMETIC_MAP (BitMap8) {0, 1, 0, 1, 1, 1, 0, 0}
#define ARITHMETIC_PAREN_MAP (BitMap8) {0, 1, 0, 1, 1, 1, 0, 0}
#define SINGLE_QUOTE_MAP (BitMap8) {0, 0, 0, 0, 0, 0, 0, 0}
#define DOUBLE_QUOTE_MAP (BitMap8) {0, 1, 0, 1, 1, 0, 0, 0}

typedef enum {
	T_REDIRECTION,
	T_WORD, 
	T_SEPARATOR,
	T_FOR,
	T_WHILE,
	T_CASE,
	T_NONE, // Generic none token when no prefix/suffix
} type_of_token;

typedef enum {
	ERROR_NONE, //error free 
	ERROR_UNEXPECTED_TOKEN, //<> ||| <<<< <<<>
	ERROR_UNCLOSED_SQ, //'....\0
	ERROR_UNCLOSED_DQ, //'....\0
	ERROR_UNCLOSED_CG, //".....\0
	ERROR_BAD_SUBSTITUTION, // cf invalid identifier
	ERROR_ESCAPED_SQ, //'..\'..'
} type_of_error;

typedef struct {
	char		*input;
	short		ch;
	size_t	input_len;
	size_t	position;
	size_t	read_position;
} Lexer;

typedef Lexer * Lexer_p;

typedef struct {
	char		**data;
	size_t		size;
	size_t		capacity;
	size_t		size_of_element;
	int		gc_level;
} StringList;

typedef struct {
	StringList *env;
	StringList *set;
	StringList *local;
} Vars;

typedef enum {
	H_NO_EVENT,
	H_LAST,
	H_WORD,
	H_NUMBER,
} type_of_history_event;

void parse_input(char *in, char *filename, Vars *shell_vars);
void			lexer_debug(Lexer_p lexer);
void			lexer_deinit(Lexer_p lexer);
void			lexer_read_char(Lexer_p l);
void			lexer_read_x_char(Lexer_p l, uint16_t n);
//-----------------Lexer------------------//
void parse_input(char *in, char *filename, Vars *shell_vars);

bool			is_whitespace(char c);
bool			is_number(char *str);
void eat_whitespace(Lexer_p l);

//----------------StringList----------------//
void			string_list_add_or_update(StringList *sl, char *var);
bool			string_list_update(StringList *sl, const char *var);
void			string_list_append(StringList *sl, char *var);
void			string_list_clear(StringList *list);
bool			string_list_remove(StringList *sl, char *id);
char			*string_list_get_value(StringList *sl, char *id);
char			*shell_vars_get_value(Vars *shell_vars, char *id);
void			string_list_print(const StringList *list);

#endif // !LEXER_H
