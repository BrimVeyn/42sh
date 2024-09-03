/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:06:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/03 13:56:54 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
#define AST_H

#include "../../include/42sh.h"
#include <stdint.h>

typedef enum {
	N_OPERAND,
	N_OPERATOR,
	N_SUBSHELL,
} type_of_node;


typedef struct {
	TokenList **data;
	uint16_t size;
	uint16_t capacity;
} TokenListVector;

typedef enum {
	N_SIMPLE_COMMMAND,
	N_NODE,
} type_of_next;


typedef struct Executer {
	type_of_next data_tag;
	union {
		struct Node *n_data;
		struct TokenList *s_data;
	};
	type_of_next next_tag;
	union {
		struct Node *n_next;
		struct TokenList *s_next;
	};
} Executer;

typedef struct Node {
	type_of_node	tag;
	union {
		TokenList			*operand;
		type_of_separator	operator;
	} value;
	struct Node		*left;
	struct Node		*right;
} Node;

typedef struct {
	Node **data;
	uint16_t size;
	uint16_t capacity;
} NodeStack;


TokenListVector *commnandCont_init(void);
void commnandCont_add(TokenListVector *tl, TokenList *token);
NodeStack *node_stack_init(void);
Node *node_stack_pop(NodeStack *self);
void node_stack_push(NodeStack *tl, Node *token);
void printTree(Node *self);
Node *ast_build(TokenList *tokens);
void tokenListToStringAll(TokenListVector *cont);
void branch_list_to_rpn(TokenListVector *list);
TokenListVector *split_operator(TokenList *list);
Node *generateTree(TokenListVector *list);

#endif // !AST_H
