/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 10:46:55 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/27 15:46:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARITHMETIC_H
# define ARITHMETIC_H

#include "../../include/lexer.h"
#include "../../include/parser.h"
#include "../../libftprintf/header/libft.h"
#include "utils.h"

typedef enum {
	P0 = 6, // - 'a++' 'a--'
	P1 = 5, // - '++a' '--a'
	P3 = 4, // - '*' '/' '%'
	P2 = 3, // - '+' '-'
	P4 = 2, // - '<=' '>=' '<' '>'
	P5 = 1, // - '==' '!='
	P6 = 0, // - '&&' '||'
	PP = -1, // - ()
} operator_precedence;

typedef enum {
	O_POPEN, //(
	O_PCLOSE, //)
	O_POST_INCR, //a++
	O_POST_DECR, //a--
	O_PREF_INCR, //++a
	O_PREF_DECR, //--a
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
	O_ERROR, //unexecpected char
} arithmetic_operators;

typedef enum {
	A_OPERATOR,
	A_OPERAND,
	A_EOF,
} arithemtic_token_tag;

typedef enum {
	VALUE,
	VARIABLE,
} arithemtic_operand_tag;

typedef struct {
	arithemtic_token_tag tag;
	arithemtic_operand_tag operand_tag;
	union {
		char *variable;
		long litteral;
		arithmetic_operators operator;
	};
} AToken;

typedef struct {
	AToken **data;
	uint16_t size;
	uint16_t capacity;
} ATokenStack;

typedef struct ANode {
	AToken *value;
	struct ANode *left;
	struct ANode *right;
} ANode;

typedef struct {
	ANode **data;
	uint16_t size;
	uint16_t capacity;
} ANodeStack;

//----------------Token List Stack------------------//
ATokenStack		*lexer_arithmetic_exp_lex_all(Lexer_p lexer);
ATokenStack		*atoken_stack_init(void);
void			atoken_stack_push(ATokenStack *self, AToken *token);
AToken			*atoken_stack_pop(ATokenStack *self);
AToken			*lexer_get_next_atoken(Lexer_p l);

//----------------Arithmetic AST------------------//
ANode		*generate_atree(ATokenStack *list);
long		aAST_execute(ANode *node, Vars *shell_vars, int *error);

//----------------ANode stack---------------------//
ANodeStack	*anode_stack_init(void);
ANode		*anode_stack_pop(ANodeStack *self);
void		anode_stack_push(ANodeStack *tl, ANode *token);

//---------------Syntax error--------------------//
bool arithmetic_syntax_check(ATokenStack *list);
bool has_higher_prec(const ATokenStack *operator, const AToken *current);
bool is_rparen(const AToken *token);
bool is_lparen(const AToken *token);
bool is_incr_or_decr(const AToken *token);
bool is_aoperator(const AToken *token);
operator_precedence get_precedence(const AToken *token);

#endif // !ARITHMETIC
