/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansions.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/06 10:07:58 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/26 16:41:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "libft.h"
#include "signals.h"
#include "utils.h"
#include "lexer.h"
#include "ft_readline.h"
#include "c_string.h"
#include "final_parser.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

//FIX: unlink heredoc
void exp_kind_list_print(ExpKindList *list);

char *here_doc(char *eof, heredoc_mode mode, Vars *shell_vars){
	char *input = NULL;
	const char *PS2 = string_list_get_value(shell_vars->set, "PS2");

	static int heredoc_number = 0;
	static int line_number = 0;
	char filename[50]; 
	sprintf(filename, "/tmp/here_doc_%d", heredoc_number++);
	int file_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if (file_fd == -1) {
		return NULL;
	}

	signal_manager(SIG_HERE_DOC);
	while(++line_number && (input = ft_readline(PS2, shell_vars)) && ft_strcmp(eof, input) && rl_done != 2){
		if (mode == HD_EXPAND){
			ft_dprintf(file_fd, "%s\n", input);
		} else if (mode == HD_NO_EXPAND){
			ft_dprintf(file_fd, "%s\n", input);
		}
		free(input);
	}

	signal_manager(SIG_PROMPT);
	if (!input){
		ft_dprintf(2, "42sh: warning: here-document at line %d delimited by end-of-file (wanted `%s')", line_number, eof);
	}
	else if (rl_done == 2){
		rl_done = 0;
		return NULL;
	}

	free(input);
	close(file_fd);
	return gc(GC_ADD, ft_strdup(filename), GC_GENERAL);
}

void remove_boundaries(Str *exp) {
	static const size_t boundaries[][2] = {
		[EXP_WORD] = {0, 0},
		[EXP_CMDSUB] = {2, 1},
		[EXP_ARITHMETIC] = {3, 2},
		[EXP_VARIABLE] = {0, 0},
	};
	//We aint't trimming for these types
	if (exp->kind == EXP_WORD || exp->kind == EXP_VARIABLE) {
		return ;
	}

	char *word = exp->str;
	size_t len = ft_strlen(word);
	size_t up_bound = boundaries[exp->kind][0];
	size_t low_bound = boundaries[exp->kind][1];

	char *tmp = ft_substr(word, up_bound, len - up_bound - low_bound);
	gc(GC_ADD, tmp, GC_SUBSHELL);
	gc(GC_FREE, word, GC_SUBSHELL);
	exp->str = tmp;
}

char *parser_tilde_expansion(char *word, Vars *shell_vars) {
	int result = -1;
	string str = string_init_str(word);
    char *home = string_list_get_value(shell_vars->env, "HOME");
	while ((result = ft_strstr(str.data, "~")) != -1){
		str_erase(&str, result, 1);
		if (home){
			str_insert_str(&str, home, result);
		}
	}
	char *resultchar = ft_strdup(str.data);
	free(str.data);
	return resultchar;
}

void string_list_consume(StrList *str_list, Vars *shell_vars) {
	for (size_t i = 0; i < str_list->size; i++) {
		char *result = NULL;
		Str *curr = str_list->data[i];
		ExpKind kind = curr->kind;

		if (curr->kind == EXP_WORD) 
			continue;
		else
			remove_boundaries(curr);
		// printf("trimmed: %s\n", curr->str);
		result = parser_tilde_expansion(curr->str, shell_vars);
		
		// printf("result: %s\n", result);
		if (result) {
			gc(GC_FREE, str_list->data[i]->str, GC_SUBSHELL);
			str_list->data[i]->str = gc(GC_ADD, result, GC_SUBSHELL);
		} else {
			str_list->data[i]->str = NULL;
		}

		switch (kind) {
			case EXP_CMDSUB: { result = parser_command_substitution(curr->str, shell_vars); break;}
			case EXP_ARITHMETIC: {result = parser_arithmetic_expansion(curr->str, shell_vars); break;}
			case EXP_VARIABLE: {result = parser_parameter_expansion(curr->str, shell_vars); break;}
			default: {}
		}
		if (result) {
			gc(GC_FREE, str_list->data[i]->str, GC_SUBSHELL);
			str_list->data[i]->str = gc(GC_ADD, result, GC_SUBSHELL);
		} else {
			str_list->data[i]->str = NULL;
		}
	}
}

ExpKind identify_exp_begin(char *str) {
	if (!ft_strncmp(str, "$((", 3)) {
		return EXP_ARITHMETIC;
	} else if (!ft_strncmp(str, "$(", 2)) {
		return EXP_CMDSUB;
	} else if (!ft_strncmp(str, "${", 2)) {
		return EXP_VARIABLE;
	} else if (!ft_strncmp(str, "(", 1)) {
		return EXP_SUB;
	} else {
		return EXP_WORD;
	}
}

typedef struct ContextMap {
	char *begin;
	char *end;
} ContextMap;

ExpKind identify_exp_end(const char *str, const ContextMap *context_map, const ExpKind context) {
	if (!ft_strncmp(str, context_map[context].end, ft_strlen(context_map[context].end))) {
		return context;
	} else {
		return EXP_WORD;
	}
}

bool is_var_expand_context(ExpKindList *exp_stack) {
	bool	var_expand = (exp_stack->size != 0);
	for (size_t i = 0; exp_stack->size && i < exp_stack->size - 1; i++) {
		if (exp_stack->data[i] != EXP_ARITHMETIC) {
			var_expand = false;
		}
	}
	return var_expand;
}

StrList *get_range_list(char *candidate, Vars *shell_vars, bool *error) {
	
	static const ContextMap map[] = {
		[EXP_ARITHMETIC] = { .begin = "$((", .end = "))" },
		[EXP_CMDSUB] = { .begin = "$(", .end = ")"},
		[EXP_VARIABLE] = { .begin = "${", .end = "}" },
		[EXP_SUB] = { .begin = "(", .end = ")" },
	};

	da_create(self, StrList, sizeof(Str *), GC_SUBSHELL);
	da_create(exp_stack, ExpKindList, sizeof(ExpKind), GC_SUBSHELL);
	da_create(word, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(cache_stack, StringStream, sizeof(char), GC_SUBSHELL);
	ss_push_string(word, candidate);
	bool can_push = false;

	while (word->size) {
		ExpKind maybe_begin = 0;
		ExpKind maybe_end = 0;
		ExpKind top_context = da_peak_back(exp_stack);
		ExpKind bottom_context = da_peak_front(exp_stack);
		bool	var_expand = is_var_expand_context(exp_stack);

		if ((maybe_begin = identify_exp_begin(word->data)) != EXP_WORD) {
			if (!exp_stack->size && can_push) {
				Str *res = str_init(EXP_WORD, ss_get_owned_slice(cache_stack), false);
				da_push(self, res);
			}
			da_push(exp_stack, maybe_begin);
			for (size_t i = 0; i < ft_strlen(map[maybe_begin].begin); i++) {
				da_push(cache_stack, da_pop_front(word));
			}
			continue;
		}
		if (exp_stack->size && (maybe_end = identify_exp_end(word->data, map, top_context)) != EXP_WORD) {
			if (maybe_end == top_context) {
				da_pop(exp_stack);
				for (size_t i = 0; i < ft_strlen(map[maybe_end].end); i++) {
					da_push(cache_stack, da_pop_front(word));
				}
				if (bottom_context == EXP_ARITHMETIC && top_context == EXP_CMDSUB) {
					da_push(cache_stack, 0);
					size_t idx = ft_strrstr(cache_stack->data, map[top_context].begin);
					char *tmp = ft_substr(&cache_stack->data[idx], 2, ft_strlen(&cache_stack->data[idx]) - 2 - 1);
					char *result = parser_command_substitution(tmp, shell_vars);
					free(tmp);
					// dprintf(2, "result: %s\n", result);
					ss_cut(cache_stack, idx);
					ss_push_string(cache_stack, result);
					free(result);
					// dprintf(2, "cache: %s\n", cache_stack->data);
				}
				if (var_expand && top_context == EXP_VARIABLE) {
					da_push(cache_stack, 0);
					size_t idx = ft_strrstr(cache_stack->data, map[top_context].begin);
					char *result = parser_parameter_expansion(&cache_stack->data[idx], shell_vars);
					if (result) {
						ss_cut(cache_stack, idx);
						ss_push_string(cache_stack, result);
						free(result);
					} else {
						ss_cut(cache_stack, idx);
						ss_push_string(cache_stack, "");
					}
				}
				if (!exp_stack->size) {
					Str *res = str_init(maybe_end, ss_get_owned_slice(cache_stack), false);
					da_push(self, res);
				}
			} 
			can_push = false;
		} else {
			da_push(cache_stack, da_pop_front(word));
			can_push = true;
		}
	}
	if (can_push) {
		Str *res = str_init(EXP_WORD, ss_get_owned_slice(cache_stack), false);
		da_push(self, res);
	}
	if (exp_stack->size) {
		UNEXPECTED_EOF(')');
		g_exitno = 2;
		*error = true;
	}
	gc(GC_FREE, cache_stack->data, GC_SUBSHELL);
	gc(GC_FREE, word->data, GC_SUBSHELL);
	return self;
}


void print_node(Str *node) {
	Str *head = node;
	while (head) {
		printf("n: %s\n", head->str);
		head = head->next;
	}
}

void str_add_back(Str **lst, Str *new_value) {
	Str	*temp;

	if (*lst == NULL) {
		*lst = new_value;
	} else {
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}


void string_list_split(StrList *list, Vars *shell_vars) {
	char *IFS = string_list_get_value(shell_vars->set, "IFS");
	if (!IFS) {
		IFS = "\n\t ";
	}

	unsigned char map[32] = {[0 ... 31] = 0};

	for (size_t i = 0; IFS[i]; i++) {
		const unsigned char c = IFS[i];
		map[c / 8] |= (1 << (c % 8));
	}

	for (size_t i = 0; i < list->size; i++) {
		if (list->data[i]->kind == EXP_WORD)
			continue;
		Str *node = NULL;
		char *curr_str = list->data[i]->str;
		const size_t str_len = ft_strlen(curr_str);
		for (size_t it = 0; it < str_len; it++) {

			const size_t start = it;
			while (it < str_len && !(map[curr_str[it] / 8] & (1 << (curr_str[it] % 8)))){
				it++;
			}
			const size_t end = it;
			const char match_char = curr_str[it];

			if (start == end && !(match_char == ' ' || match_char == '\n' || match_char == '\t')) {
				str_add_back(&node, str_init(list->data[i]->kind, ft_strdup(""), true));
			} else if (start != end) {
				str_add_back(&node, str_init(list->data[i]->kind, ft_substr(curr_str, start, end - start), true));
			}
		}
		gc(GC_FREE, list->data[i]->str, GC_SUBSHELL);
		gc(GC_FREE, list->data[i], GC_SUBSHELL);
		list->data[i] = node;
	}
}


StringListL *string_list_merge(StrList *list) {
	da_create(new, StringListL, sizeof(char *), GC_SUBSHELL);
	da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);
	for (size_t i = 0; i < list->size; i++) {
		Str *curr = list->data[i];
		Str *next = (i < list->size - 1) ? list->data[i + 1] : NULL;
		if (curr->kind == EXP_WORD) {
			ss_push_string(ss, curr->str);
			if (!next || next->kind == EXP_WORD) {
				da_push(new, ss_get_owned_slice(ss));
			}
		} else {
			Str *node = curr;
			while (node) {
				ss_push_string(ss, node->str);
				if (!(node->next == NULL && next)) {
					da_push(new, ss_get_owned_slice(ss));
				}
				node = node->next;
			}
		}
	}
	return new;
}

void string_list_push_list(StringListL *lhs, StringListL *rhs) {
	for (size_t i = 0; i < rhs->size; i++) {
		da_push(lhs, rhs->data[i]);
	}
}

void string_erase_nulls(StrList *list) {
	for (size_t i = 0; i < list->size;) {
		Str *curr = list->data[i];
		if (!curr || !curr->str) {
			da_erase_index(list, i);
			if (list->data[i])
				list->data[i]->kind = EXP_SUB;
			continue;
		} else {
			i++;
		}
	}
}

StringListL *do_expansions(const StringListL * const word_list, Vars * const shell_vars, const bool split) {

	bool error = false;
	da_create(arg_list, StringListL, sizeof(char *), GC_SUBSHELL);
	
	for (size_t it = 0; it < word_list->size; it++) {
		StrList *string_list = get_range_list(word_list->data[it], shell_vars, &error);
		if (error) 
			return NULL;
		string_list_consume(string_list, shell_vars);
		if (split)
			string_list_split(string_list, shell_vars);
		string_erase_nulls(string_list);
		StringListL *result = string_list_merge(string_list);
		string_list_push_list(arg_list, result);
	}
	return arg_list;
}
