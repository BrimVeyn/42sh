/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_execute.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:11:53 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/13 15:25:47 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ast.h"
#include "exec.h"

#include <stdlib.h>
#include <stdio.h>

int ast_execute(Node *node, StringList *env) {
	if (node->tag == N_OPERATOR) {
		if (node->value.operator == S_AND) {
			int exit_lhs = ast_execute(node->left, env);
			if (exit_lhs == EXIT_SUCCESS) {
				int exit_rhs = ast_execute(node->right, env);
				// printf(C_GREEN);
				// printTree(node->left);
				// printTree(node->right);
				// printf("Right Side AND = %d\n", exit_rhs);
				// printf("PID = %d\n", getpid());
				// printf(C_RESET"\n");
				return exit_rhs;
			} else
				return exit_lhs;
		}
		if (node->value.operator == S_OR) {
			int exit_lhs = ast_execute(node->left, env);
			// printf(C_GREEN);
			// printTree(node->left);
			// printTree(node->right);
			// printf("Left Side OR = %d\n", exit_lhs);
			// printf("PID = %d\n", getpid());
			// printf(C_RESET"\n");
			if (exit_lhs != EXIT_SUCCESS) {
				int exit_rhs = ast_execute(node->right, env);
				return exit_rhs;
			}
			else
				return exit_lhs;
		}
		if (node->value.operator == S_SEMI_COLUMN || node->value.operator == S_BG) {
			int last_exit;
			if (node->left) {
				last_exit = ast_execute(node->left, env);
			}
			if (node->right) {
				last_exit = ast_execute(node->right, env);
			}
			return last_exit;
		}
	}
	if (node->tag == N_OPERAND) {
		// printf("executing ---------------------------\n");
		// printTree(node);
		int exit_status = exec_node(node, env);
		// printf("exit status = %d -----------------------------\n", exit_status);
		return exit_status;
	}
	
	printf("FATAL AST ERROR\n");
	exit(EXIT_FAILURE);
}
