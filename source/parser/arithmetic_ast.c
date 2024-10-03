/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arithmetic_ast.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/27 11:11:06 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/02 12:47:26 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "arithmetic.h"
#include "assert.h"
#include "ast.h"

static long get_value(ANode *self, Vars *shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		char *str_value = string_list_get_value(shell_vars->set, self->value->variable);
		long int_value = (str_value == NULL) ? 0 : ft_atol(str_value);
		return int_value;
	} else {
		return self->value->litteral;
	}
}

static ANode *incr(ANode *self, Vars *shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		char tmp[MAX_WORD_LEN] = {0};
		long var_value = get_value(self, shell_vars);
		ft_sprintf(tmp, "%s=%ld", self->value->variable, var_value + 1);
		char *new_value = gc(GC_ADD, ft_strdup(tmp), GC_GENERAL);
		string_list_add_or_update(shell_vars->set, new_value);
	}
	return self;
}

static ANode *decr(ANode *self, Vars *shell_vars) {
	if (self->value->operand_tag == VARIABLE) {
		char tmp[MAX_WORD_LEN] = {0};
		long var_value = get_value(self, shell_vars);
		ft_sprintf(tmp, "%s=%ld", self->value->variable, var_value - 1);
		char *new_value = gc(GC_ADD, ft_strdup(tmp), GC_GENERAL);
		string_list_add_or_update(shell_vars->set, new_value);
	}
	return self;
}

static ANode *gen_aoperator_node(AToken *tok, ANode *left, ANode *right) {
	ANode *self = gc(GC_ADD, ft_calloc(1, sizeof(ANode)), GC_SUBSHELL);
	self->value = tok;
	self->left = left;
	self->right = right;
	return self;
}

static ANode *gen_aoperand_node(AToken *tok) {
	ANode *self = gc(GC_ADD, ft_calloc(1, sizeof(ANode)), GC_SUBSHELL);
	self->value = tok;
	self->left = NULL;
	self->right = NULL;
	return self;
}

ANode *generate_atree(ATokenStack *list) {
	ANodeStack *self = anode_stack_init();
	for (int i = 0; i < list->size; i++) {
		if (!is_aoperator(list->data[i])) {
			anode_stack_push(self, gen_aoperand_node(list->data[i]));
		} else {
			if (self->size == 1) {
				ANode *left = anode_stack_pop(self);
				anode_stack_push(self, gen_aoperator_node(list->data[i], left, NULL));
			} else {
				ANode *right = anode_stack_pop(self);
				ANode *left = anode_stack_pop(self);
				anode_stack_push(self, gen_aoperator_node(list->data[i], left, right));
			}
		}
	}
	assert(self->size == 1);
	return anode_stack_pop(self);
}

long aAST_execute(ANode *node, Vars *shell_vars, int *error) {
	if (!node) return 0;

	switch (node->value->tag) {
		case A_OPERATOR:
			switch (node->value->operator) {
				case O_PREF_INCR: return aAST_execute(incr(node->left, shell_vars), shell_vars, error);
				case O_POST_INCR: {
					long var_value = get_value(node->left, shell_vars);
					incr(node->left, shell_vars);
					return var_value;
				}
				case O_PREF_DECR: return aAST_execute(decr(node->left, shell_vars), shell_vars, error);
				case O_POST_DECR: {
					long var_value = get_value(node->left, shell_vars);
					decr(node->left, shell_vars);
					return var_value;
				}
				case O_PLUS: return aAST_execute(node->left, shell_vars, error) + aAST_execute(node->right, shell_vars, error);
				case O_MINUS: return aAST_execute(node->left, shell_vars, error) - aAST_execute(node->right, shell_vars, error);
				case O_MULT: return aAST_execute(node->left, shell_vars, error) * aAST_execute(node->right, shell_vars, error);
				case O_DIVIDE: {
					const long left_result = aAST_execute(node->left, shell_vars, error);
					const long right_result = aAST_execute(node->right, shell_vars, error);
					if (right_result == 0) {
						*error = -1;
						return 0;
					}
					return left_result / right_result;
                }
				case O_MODULO: {
					const long left_result = aAST_execute(node->left, shell_vars, error);
					const long right_result = aAST_execute(node->right, shell_vars, error);
					if (right_result == 0) {
						*error = -1;
						return 0;
					}
					return left_result % right_result;
                }
				case O_GE: return aAST_execute(node->left, shell_vars, error) >= aAST_execute(node->right, shell_vars, error);
				case O_LE: return aAST_execute(node->left, shell_vars, error) <= aAST_execute(node->right, shell_vars, error);
				case O_GT: return aAST_execute(node->left, shell_vars, error) > aAST_execute(node->right, shell_vars, error);
				case O_LT: return aAST_execute(node->left, shell_vars, error) < aAST_execute(node->right, shell_vars, error);
				case O_EQUAL: return aAST_execute(node->left, shell_vars, error) == aAST_execute(node->right, shell_vars, error);
				case O_DIFFERENT: return aAST_execute(node->left, shell_vars, error) != aAST_execute(node->right, shell_vars, error);
				case O_AND: return aAST_execute(node->left, shell_vars, error) && aAST_execute(node->right, shell_vars, error);
				case O_OR: return aAST_execute(node->left, shell_vars, error) || aAST_execute(node->right, shell_vars, error);
				default:
                {
					dprintf(2, "AST unhandled case\n");
					exit(EXIT_FAILURE);
                }
		}
		case A_OPERAND: {
			return get_value(node, shell_vars);
		}
		default:
			exit(EXIT_FAILURE);
	}
}
