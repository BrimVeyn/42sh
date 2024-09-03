/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:06:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/03 17:13:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
#define AST_H

#include "../../include/42sh.h"

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
	DATA_NODE,
	DATA_TOKENS,
} type_of_data;

typedef struct Node {
	type_of_node	tag;
	union {
		TokenList			*operand;
		type_of_separator	operator;
	} value;
	struct Node		*left;
	struct Node		*right;
} Node;

typedef struct Executer {
	type_of_data data_tag;
	union {
		Node *n_data;
		TokenList *s_data;
	};
	struct Executer *next;
} Executer;

typedef struct {
	Executer **data;
	uint16_t size;
	uint16_t capacity;
} ExecuterList;

typedef struct {
	Node **data;
	uint16_t size;
	uint16_t capacity;
} NodeStack;


TokenListVector *token_list_vector_init(void);
void token_list_vector_add(TokenListVector *tl, TokenList *token);
NodeStack *node_stack_init(void);
Node *node_stack_pop(NodeStack *self);
void node_stack_push(NodeStack *tl, Node *token);
void printTree(Node *self);
Node *ast_build(TokenList *tokens);
int ast_execute(Node *AST);

void tokenListToStringAll(TokenListVector *cont);
void branch_list_to_rpn(TokenListVector *list);
TokenListVector *split_operator(TokenList *list);
Node *generateTree(TokenListVector *list);


ExecuterList *build_executer_list(TokenList *list);

//----------------Executer List------------------//
void			executer_list_push(ExecuterList *tl, Executer *token);
ExecuterList	*executer_list_init(void);
Executer *executer_init(Node *node, TokenList *list);
void executer_push_back(Executer **lst, Executer *new_value);

#endif // !AST_H
