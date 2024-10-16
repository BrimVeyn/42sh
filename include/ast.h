/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:17:44 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/16 17:04:57 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
#define AST_H

#include "lexer.h"
#include "parser.h"

#include <termios.h>

typedef struct {
	TokenList **data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
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
	Node **data;
	size_t size;
	size_t capacity;
	size_t	size_of_element;
	int		gc_level;
} NodeStack;


//----------------AST-----------------//
Node				*ast_build(TokenList *tokens);
Node				*generate_tree(TokenListStack *list);
int					ast_execute(Node *AST, Vars *shell_vars, bool foreground);
Node				*gen_operator_node(TokenList *tok, Node *left, Node *right);
Node				*gen_operand_node(TokenList *list);
Node				*generate_tree(TokenListStack *list);

TokenListStack		*branch_stack_to_rpn(TokenListStack *list);
TokenListStack		*split_operator(TokenList *list);
void				skip_subshell(TokenList *newlist, TokenList *list, size_t *i);
//----------------Job------------------//
Process				*process_init(Node *node, TokenList *list);
void				process_push_back(Process **lst, Process *new_value);

//----------------Job List------------------//
Process *build_executer_list(TokenList *token_list);

#endif // !AST_H
