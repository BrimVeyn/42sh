// /* ************************************************************************** */
// /*                                                                            */
// /*                                                        :::      ::::::::   */
// /*   parser_arithmetic_expansion.c                      :+:      :+:    :+:   */
// /*                                                    +:+ +:+         +:+     */
// /*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
// /*                                                +#+#+#+#+#+   +#+           */
// /*   Created: 2024/09/18 15:46:07 by bvan-pae          #+#    #+#             */
// /*   Updated: 2024/10/10 16:16:05 by bvan-pae         ###   ########.fr       */
// /*                                                                            */
// /* ************************************************************************** */
//
// #include "arithmetic.h"
// #include "ast.h"
// #include "debug.h"
// #include "lexer.h"
// #include "libft.h"
// #include "parser.h"
// #include "utils.h"
// #include "assert.h"
// #include <stdio.h>
//
// static void skip_parenthesis(char *str, int *i) {
// 	(*i)++;
// 	while (str[*i] && str[*i] != ')') {
// 		if (str[*i] == '(') {
// 			skip_parenthesis(str, i);
// 		}
// 		(*i)++;
// 	}
// }
//
// static int get_arithmetic_exp_range_end(char *str, int *i) {
// 	(*i) += 3; //skip '$(('
//
// 	while (str[*i] && ft_strncmp("))", &str[*i], 2)) {
// 		if (!ft_strncmp("$((", &str[*i], 3)) {
// 			get_arithmetic_exp_range_end(str, i);
// 			if (*i == -1) return -1;
// 		}
// 		if (str[*i] == '(') {
// 			skip_parenthesis(str, i);
// 		}
// 		(*i)++;
// 	}
// 	if (ft_strncmp(&str[*i], "))", 2)) return -1;
// 	(*i) += 1;
// 	return *i;
// }
//
// static Range get_arithmetic_exp_range(char *str) {
// 	Range self = {
// 		.start = -1,
// 		.end = -1,
// 	};
//
// 	self.start = ft_strstr(str, "$((");
// 	if (self.start != -1) {
// 		int start_copy = self.start;
// 		self.end = get_arithmetic_exp_range_end(str, &start_copy);
// 	}
// 	return self;
// }
//
// static operator_precedence get_precedence(const AToken *token) {
// 	static const struct {
// 		arithmetic_operators OP;
// 		operator_precedence P;
// 	} pre[] = {
// 		{O_POST_DECR, P6}, {O_POST_INCR, P6},
// 		{O_PREF_DECR, P5}, {O_PREF_INCR, P5},
// 		{O_MULT, P4}, {O_DIVIDE, P4}, {O_MODULO, P4},
// 		{O_PLUS, P3}, {O_MINUS, P3},
// 		{O_LE, P2}, {O_GE, P2}, {O_LT, P2}, {O_GT, P2},
// 		{O_EQUAL, P1}, {O_DIFFERENT, P1},
// 		{O_AND, P0}, {O_OR, P0},
// 		{O_PCLOSE, PP}, {O_POPEN, PP},
// 	};
//
// 	for (size_t i = 0; i < sizeof(pre) / sizeof(pre[0]); i++) {
// 		if (token->operator == pre[i].OP) return pre[i].P;
// 	}
// 	return P0;
// }
//
// static bool has_higher_prec(const ATokenStack *operator, const AToken *current) {
// 	if (!operator->size) return false;
// 	const AToken *stack_top = operator->data[operator->size - 1];
// 	return (get_precedence(stack_top) >= get_precedence(current));
// }
//
// ATokenStack *tokens_to_rpn(ATokenStack *list) {
// 	ATokenStack *output = atoken_stack_init();
// 	ATokenStack *operator = atoken_stack_init();
// 	
// 	// aTokenListToString(list);
// 	for (int i = 0; i < list->size; i++) {
// 		AToken *current = list->data[i];
// 		if (is_lparen(current)) {
// 			atoken_stack_push(operator, current);
// 		} else if (is_rparen(current)) {
// 			while (operator->size) {
// 				AToken *popped = atoken_stack_pop(operator);
// 				if (is_lparen(popped))
// 					break;
// 				atoken_stack_push(output, popped);
//             }
// 		} else if (!is_aoperator(current)) {
// 			atoken_stack_push(output, current);
// 		} else {
// 			while (has_higher_prec(operator, current)) {
// 				atoken_stack_push(output, atoken_stack_pop(operator));
// 			}
// 			atoken_stack_push(operator, current);
// 		}
// 	}
// 	while (operator->size) {
// 		atoken_stack_push(output, atoken_stack_pop(operator));
// 	}
// 	return output;
// }
//
// static void copy_till_end_of_nested(const char *ref, char *buffer, int *i, int *j, const int n) {
// 	buffer[(*j)++] = '(';
// 	(*i) += n;
// 	while (ref[*i])
//     {
// 		if (!ft_strncmp(&ref[*i], "$((", 3))
// 			copy_till_end_of_nested(ref, buffer, i, j, 3);
// 		if (ref[*i] == '(')
// 			copy_till_end_of_nested(ref, buffer, i, j, 1);
//
// 		buffer[(*j)] = ref[(*i)];
// 		if (!ft_strncmp(&ref[*i], "))", 2)) {
// 			(*i) += 2; (*j) += 1;
// 			return;
// 		}
// 		if (ref[*i] == ')') {
// 			(*i) += 1; (*j) += 1;
// 			return;
//         }
// 		(*i)++; (*j)++;
//     }
// }
//
// static void replace_nested_greedy(const char *ref, char *buffer) {
// 	int i = 0;
// 	int j = 0;
// 	while (ref[i]) {
// 		if (!ft_strncmp(&ref[i], "$((", 3)) {
// 			copy_till_end_of_nested(ref, buffer, &i, &j, 3);
// 			continue;
// 		}
// 		buffer[j++] = ref[i++];
// 	}
// 	buffer[j] = '\0';
// }
//
// #include "colors.h"
//
// bool parser_arithmetic_expansion(TokenList *tokens, const int idx, const size_t start, Vars *shell_vars) {
// 	Token *elem = tokens->t[idx];
// 	char *string = &elem->w_infix[start];
//
// 	const Range range = get_arithmetic_exp_range(string);
// 	if (is_a_match(range)) {
// 		if (!is_range_valid(range, UNCLOSED_ARITMETIC_EXP_STR)) { g_exitno = 126; return false; }
// 		else { return false; }
// 	}
//
// 	char infix[MAX_WORD_LEN] = {0};
// 	ft_memset(infix, '\0', MAX_WORD_LEN);
// 	ft_memcpy(infix, &string[range.start + 3], (range.end - range.start) - 4);
// 	char *tmp = ft_strdup(infix);
// 	replace_nested_greedy(tmp, infix);
// 	FREE_POINTERS(tmp);
//
// 	printf(C_RED"------"C_RESET"\n");
// 	printf("%s\n", infix);
// 	printf(C_RED"------"C_RESET"\n");
//
// 	long result = 0;
// 	Lexer_p lexer = lexer_init(infix);
// 	ATokenStack *token_stack = lexer_arithmetic_exp_lex_all(lexer);
// 	// aTokenListToString(token_stack);
//
// 	if (!token_stack) { g_exitno = 1; return false; }
// 	if (!token_stack->size) { goto empty; }
// 	if (!arithmetic_syntax_check(token_stack)) {
// 		return false;
// 	}
//
// 	ATokenStack *token_queue = tokens_to_rpn(token_stack);
// 	ANode *AST = generate_atree(token_queue);
// 	int error = 0;
// 	result = aAST_execute(AST, shell_vars, &error);
// 	if (error == -1) {
// 		dprintf(2, DIVISION_BY_0"\n"); g_exitno = 1;
// 		return false;
// 	}
//
// empty: {
// 		char *prefix = ft_substr(elem->w_infix, start, range.start);
// 		char *postfix = ft_substr(string, range.end + 1, (strlen(string) - 1) - range.end);
// 		char *result_str = ft_ltoa(result);
// 		char *prefix_result = ft_strjoin(prefix, result_str);
// 		char *prefix_result_postfix= ft_strjoin(prefix_result, postfix);
// 		FREE_POINTERS(prefix, postfix, result_str, prefix_result);
// 		elem->w_infix = gc(GC_ADD, prefix_result_postfix, GC_SUBSHELL);
// 	}
// 	return true;
// }
