/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 13:46:35 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/03 09:00:35 by bvan-pae         ###   ########.fr       */
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
#define WORD_MAP				0b11011011  // Binary equivalent of {0, 1, 0, 1, 1, 0, 1, 1}
#define CMD_SUB_MAP        		0b11111011  // Binary equivalent of {0, 1, 1, 1, 1, 0, 1, 1}
#define PARAM_MAP          		0b11011110  // Binary equivalent of {0, 0, 0, 1, 0, 0, 0, 0}
#define SUBSHELL_MAP       		0b01111011  // Binary equivalent of {0, 1, 1, 1, 1, 0, 1, 1}
#define ARITHMETIC_MAP			0b11011100  // Binary equivalent of {0, 1, 0, 1, 1, 1, 0, 0}
#define ARITHMETIC_PAREN_MAP	0b01011100 // Binary equivalent of {0, 1, 0, 1, 1, 1, 0, 0}
#define SINGLE_QUOTE_MAP		0b00000000  // Binary equivalent of {0, 0, 0, 0, 0, 0, 0, 0}
#define DOUBLE_QUOTE_MAP   		0b00010110  // Binary equivalent of {0, 1, 0, 1, 1, 0, 0, 0}

typedef struct {
	char		*input;
	short		ch;
	size_t	input_len;
	size_t	position;
	size_t	read_position;
} Lexer;


typedef enum {
	H_NO_EVENT,
	H_LAST,
	H_WORD,
	H_NUMBER,
} type_of_history_event;

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

#endif // !LEXER_H
