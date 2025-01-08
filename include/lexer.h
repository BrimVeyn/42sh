/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 13:46:35 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/24 10:31:22 by bvan-pae         ###   ########.fr       */
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
	WORD_PROC_SUB_IN,
	WORD_PROC_SUB_OUT,
	NONE,
} WordContext;

typedef struct {
	char	*start;
	char	*end;
	uint16_t bitmap;
} WordContextBounds;

typedef struct {
	char	*data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} WordContextList;

//This represent a small automaton. 1 means can transition to
#define WORD_MAP				0b1111011011
#define CMD_SUB_MAP        		0b0011111011
#define PARAM_MAP          		0b0011011110
#define SUBSHELL_MAP       		0b0001111011
#define ARITHMETIC_MAP			0b0011011100
#define ARITHMETIC_PAREN_MAP	0b0001011100
#define SINGLE_QUOTE_MAP		0b0000000000
#define DOUBLE_QUOTE_MAP   		0b0000010010
#define PROCESS_SUB_MAP			0b0111011011
#define PROCESS_SUB_MAP			0b0111011011

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
