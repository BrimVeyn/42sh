/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:17:44 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/10 14:25:03 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
#define AST_H

#include "lexer.h"
#include "parser.h"

#include <termios.h>

typedef struct {
	TokenList **data;
	uint16_t size;
	uint16_t capacity;
} TokenListStack;

typedef enum {
	N_OPERAND,
	N_OPERATOR,
} type_of_node;

typedef enum {
	TREE_SUBSHELL,
	TREE_COMMAND_GROUP,
	TREE_DEFAULT,
} type_of_tree;

typedef struct Node {
	type_of_node	tag;
	type_of_tree	tree_tag;
	RedirectionList *redirs;
	union {
		TokenList *operand;
		type_of_separator	operator;
	} value;
	struct Node		*left;
	struct Node		*right;
} Node;

typedef enum {
	DATA_NODE,
	DATA_TOKENS,
} type_of_data;

typedef struct Process {
	struct Process *next;
	pid_t pid;
	bool completed;
	bool stopped;
	int status;

	type_of_data data_tag;
	union {
		Node *n_data;
		TokenList *s_data;
	};
	SimpleCommand *command;
} Process;

typedef struct Job {
	struct Process *first_process; //pointer to the first process
	size_t id; //job control id
	pid_t pgid; //process group id
	bool notified; //true if user told about stopped jobs
	bool bg; 
	int sig; //stopped by signal
	struct termios tmodes; //saved terminal modes
} Job;

typedef struct {
	Process **data;
	uint16_t size;
	uint16_t capacity;
} ExecuterList;

typedef struct {
	Node **data;
	uint16_t size;
	uint16_t capacity;
} NodeStack;


//----------------AST-----------------//
Node				*ast_build(TokenList *tokens);
Node				*generate_tree(TokenListStack *list);
int					ast_execute(Node *AST, Vars *shell_vars, bool foreground);
Node				*gen_operator_node(TokenList *tok, Node *left, Node *right);
Node				*gen_operand_node(TokenList *list);
Node				*generate_tree(TokenListStack *list);

//----------------Token List Stack------------------//
TokenListStack		*token_list_stack_init(void);
TokenList			*token_list_stack_pop(TokenListStack *self);
void				token_list_stack_push(TokenListStack *self, TokenList *token);
TokenListStack		*branch_stack_to_rpn(TokenListStack *list);
TokenListStack		*split_operator(TokenList *list);
void				skip_subshell(TokenList *newlist, TokenList *list, int *i);

//----------------Node Stack------------------//
NodeStack			*node_stack_init(void);
Node				*node_stack_pop(NodeStack *self);
void				node_stack_push(NodeStack *tl, Node *token);

//----------------Job------------------//
Process				*process_init(Node *node, TokenList *list);
void				process_push_back(Process **lst, Process *new_value);

//----------------Job List------------------//
ExecuterList		*build_executer_list(TokenList *list);
ExecuterList		*executer_list_init(void);
void				executer_list_push(ExecuterList *tl, Process *process);

#endif // !AST_H
