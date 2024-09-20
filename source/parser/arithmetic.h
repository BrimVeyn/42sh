/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 10:46:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/20 16:29:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARITHMETIC_H
# define ARITHMETIC_H

#include "../../include/lexer.h"
#include "../../include/parser.h"
#include "../../libftprintf/header/libft.h"

typedef enum {
	O_INCR, //++
	O_DECR, //--
	O_PLUS, // +
	O_MINUS, // -
	O_MULT, // *
	O_DIVIDE, // /
	O_MODULO, // %
	O_LE, // <=
	O_GE, // >=
	O_LT, // <
	O_GT, // >
	O_EQUAL, // == 
	O_DIFFERENT, // !=
	O_OR, // ||
	O_AND, // &&
} arithmetic_operators;

typedef enum {
	A_OPERATOR,
	A_OPERAND,
} arithemtic_token_tag;


typedef struct {
	arithemtic_token_tag tag;
	union {
		int value;
		arithmetic_operators operator;
	};
} AToken;

typedef struct {
	AToken **data;
	uint16_t size;
	uint16_t capacity;
} ATokenList;

//--------------------------ATokenList-----------------
ATokenList *atoken_list_init(void);
void		atoken_list_add(ATokenList *tl, AToken *token);
void 		atoken_list_add_list(ATokenList *t1, ATokenList *t2);
void 		atoken_list_insert(ATokenList *tl, AToken *token, const int index);
void 		atoken_list_insert_list(ATokenList *dest, ATokenList *src, const int index);
void 		atoken_list_remove(ATokenList *tl, int index);
//-----------------------------------------------------
ATokenList *lexer_arithmetic_exp_lex_all(Lexer_p lexer);
AToken *lexer_get_next_atoken(Lexer_p l);

#endif // !ARITHMETIC
