/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 14:44:10 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 17:25:05 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

//-----------------Lexer------------------//
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
	char *start;
	char *end;
	int bitmap;
} WordContextBounds;

typedef struct {
	char	*data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} WordContextList;

//This represent a small automaton. 1 means can transition to
#define WORD_MAP				0b01011011  // Binary equivalent of {0, 1, 0, 1, 1, 0, 1, 1}
#define PARAM_MAP          		0b00010000  // Binary equivalent of {0, 0, 0, 1, 0, 0, 0, 0}
#define CMD_SUB_MAP        		0b01111011  // Binary equivalent of {0, 1, 1, 1, 1, 0, 1, 1}
#define SUBSHELL_MAP       		0b01111011  // Binary equivalent of {0, 1, 1, 1, 1, 0, 1, 1}
#define ARITHMETIC_MAP			0b01011100  // Binary equivalent of {0, 1, 0, 1, 1, 1, 0, 0}
#define ARITHMETIC_PAREN_MAP	0b01011100 // Binary equivalent of {0, 1, 0, 1, 1, 1, 0, 0}
#define SINGLE_QUOTE_MAP		0b00000000  // Binary equivalent of {0, 0, 0, 0, 0, 0, 0, 0}
#define DOUBLE_QUOTE_MAP   		0b01011000  // Binary equivalent of {0, 1, 0, 1, 1, 0, 0, 0}

typedef struct {
	char		*input;
	short		ch;
	size_t	input_len;
	size_t	position;
	size_t	read_position;
} Lexer;

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
	StringList *positional;
} Vars;

typedef enum {
	H_NO_EVENT,
	H_LAST,
	H_WORD,
	H_NUMBER,
} type_of_history_event;

void			parse_input(char *in, char *filename, Vars * const shell_vars);
void			lexer_debug(Lexer * lexer);
void			lexer_deinit(Lexer * lexer);
void			lexer_read_char(Lexer * l);
void			lexer_read_x_char(Lexer * l, uint16_t n);
//------------------------------------------//


//-----------------Utils------------------//
bool			is_whitespace(char c);
bool			is_number(char *str);
void			eat_whitespace(Lexer * l);
//------------------------------------------//


//----------------StringList----------------//
char			*get_variable_value(Vars * const shell_vars, char * const name);
void			string_list_add_or_update(StringList * const sl, char * const var);
bool			string_list_update(StringList *sl, const char *var);
void			string_list_append(const StringList * const sl, char * const var);
void			string_list_clear(StringList *list);
bool			string_list_remove(StringList *sl, char *id);
char			*string_list_get_value(const StringList * const sl, char * const id);
char			*shell_vars_get_value(const Vars * const shell_vars, char * const id);
void			string_list_print(const StringList *list);
//------------------------------------------//

#endif // !LEXER_H
