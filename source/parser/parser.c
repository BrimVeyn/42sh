/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:27:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/21 11:14:15 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "ft_regex.h"
#include "libft.h"
#include "utils.h"
#include "lexer.h"
#include "exec.h"
#include "debug.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>

char *here_doc(char *eof){
	char *input = NULL;
	static int heredoc_number = 0;
	char filename[50]; 
	sprintf(filename, "/tmp/here_doc_%d", heredoc_number++);
	int file_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0664);
	if (file_fd == -1) {
		return NULL;
	}
	while((input = readline("> ")) && ft_strcmp(eof, input)){
		dprintf(file_fd, "%s\n", input);
	}
	close(file_fd);
	return gc(GC_ADD, ft_strdup(filename), GC_GENERAL);
}

bool heredoc_detector(TokenList *data) {
	for (uint16_t it = 0; it < data->size; it++) {
		const Token *curr = data->data[it];
		Token *const el = (curr->tag == T_WORD && curr->w_postfix->tag == T_REDIRECTION) ? curr->w_postfix : (Token *) curr;
		if (el->tag == T_REDIRECTION && el->r_type == R_HERE_DOC) {
			Token *filename = el->r_postfix;
			el->r_type = R_INPUT;
			filename->w_infix = here_doc(filename->w_infix);
			if (!filename->w_infix) return false;
		}
	}
	return true;
}

void add_redirection_from_token(RedirectionList *redir_list, const Token *el) {
	Redirection *redir = gc_unique(Redirection, GC_SUBSHELL); 
	// tokenToString((Token *)el, 0);

	if (el->tag == T_WORD) {
		redir->prefix_fd = ft_atoi(el->w_infix);
		redir->r_type = el->w_postfix->r_type;
		if (is_number(el->w_postfix->r_postfix->w_infix)) {
			redir->su_type = R_FD;
			redir->fd = ft_atol(el->w_postfix->r_postfix->w_infix);
		} else {
			redir->su_type = R_FILENAME;
			redir->filename = el->w_postfix->r_postfix->w_infix;
		}
	} else if (el->tag == T_REDIRECTION) {
		redir->prefix_fd = -1;
		redir->r_type = el->r_type;
		if (is_number(el->r_postfix->w_infix)) {
			redir->su_type = R_FD;
			redir->fd = ft_atol(el->r_postfix->w_infix);
		} else {
			redir->su_type = R_FILENAME;
			redir->filename = el->r_postfix->w_infix;
		}
	}
	da_push(redir_list, redir);
}

Token *get_candidate(TokenList *tl, const size_t idx) {
	if (is_redirection(tl, &idx)) {
		if (tl->data[idx]->tag == T_WORD) {
			return tl->data[idx]->w_postfix->r_postfix;
		} else {
			return tl->data[idx]->r_postfix;
		}
	} else if (is_word(tl, &idx)) {
		return tl->data[idx];
	} else {
		return NULL;
	}
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

typedef struct {
	ExpKind *data;
	int gc_level;
	size_t size;
	size_t capacity;
	size_t size_of_element;
} ExpKindList;

char *ss_get_owned_slice(StringStream *ss) {
	da_push(ss, '\0');
	char *str = ss->data;
	ss->data = gc(GC_ALLOC, ss->capacity, sizeof(char), GC_SUBSHELL);
	ss->size = 0;
	return str;
}

void ss_push_string(StringStream *ss, char *str) {
	for (size_t i = 0; str[i]; i++) {
		da_push(ss, str[i]);
	}
}

void ss_cut(StringStream *ss, size_t new_size) {
	ft_memset(&ss->data[new_size], 0, ss->size - new_size);
	ss->size = new_size;
}

void exp_kind_list_print(ExpKindList *list) {
	static const char *map[] = {
		[EXP_WORD] = "EXP_WORD",
		[EXP_CMDSUB] = "EXP_CMDSUB",
		[EXP_ARITHMETIC] = "EXP_ARITHMETIC",
		[EXP_VARIABLE] = "EXP_VARIABLE",
		[EXP_SUB] = "EXP_SUB",
	};

	for (size_t i = 0; i < list->size; i++) {
		dprintf(2, "[%zu]: %s\n", i, map[list->data[i]]);
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

StrList *get_range_list(Token *candidate, Vars *shell_vars, bool *error) {
	
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
	ss_push_string(word, candidate->w_infix);
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


StringList *string_list_merge(StrList *list) {
	da_create(new, StringList, sizeof(Str *), GC_SUBSHELL);
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

void string_list_push_list(StringList *lhs, StringList *rhs) {
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

bool is_variable_declaration(Token *token, bool *can_be_variable) {
	if (*can_be_variable && token->tag == T_WORD && regex_match("^[_a-zA-Z][a-zA-Z1-9_]*=", token->w_infix).is_found) {
		return true;
	} else {
		*can_be_variable = false;
		return false;
	}
}

bool is_here_doc(const TokenList *list, const size_t *it) {
	return (list->data[*it]->r_type == R_HERE_DOC || list->data[*it]->w_postfix->r_type == R_HERE_DOC);
}

SimpleCommand *parser_parse_current(TokenList *list, Vars *shell_vars) {

	bool error = false;
	bool can_be_variable = true;
	SimpleCommand *command = gc_unique(SimpleCommand, GC_SUBSHELL);
	da_create(redir_list, RedirectionList, sizeof(Redirection *), GC_SUBSHELL);
	da_create(var_list, TokenList, sizeof(Token *), GC_SUBSHELL);
	da_create(arg_list, StringList, sizeof(Str), GC_SUBSHELL);
	
	enum { WORD, VARIABLE, REDIRECTION, HERE_DOC };

	for (size_t it = 0; it < list->size; it++) {
		// tokenToString(list->data[it], 0);
		int type = WORD;
		if (is_variable_declaration(list->data[it], &can_be_variable)) {
			type = VARIABLE;
		}
		if (is_redirection(list, &it)) {
			type = REDIRECTION;
		}
		Token *candidate = get_candidate(list, it);
		if (!candidate)
			continue;
		StrList *string_list = get_range_list(candidate, shell_vars, &error);
		// da_print(string_list);
		if (error) 
			return NULL;
		string_list_consume(string_list, shell_vars);
		if (type == WORD) {
			string_list_split(string_list, shell_vars);
		}
		string_erase_nulls(string_list);
		StringList *result = string_list_merge(string_list);
		if (type == WORD) {
			string_list_push_list(arg_list, result);
		} else if (type == VARIABLE) {
			list->data[it]->w_infix = result->data[0];
			da_push(var_list, list->data[it]);
		} else if (type == REDIRECTION) {
			if (list->data[it]->tag == T_WORD) {
				list->data[it]->w_postfix->r_postfix->w_infix = result->data[0];
			} else {
				list->data[it]->r_postfix->w_infix = result->data[0];
			}
			add_redirection_from_token(redir_list, list->data[it]);
		}
	}

	command->bin = arg_list->data[0];
	command->args = arg_list->data;
	command->redir_list = redir_list;

	// printCommand(command);
	
	if (command->bin == NULL && var_list->size != 0)
		add_vars_to_set(shell_vars, var_list);
	else
		add_vars_to_local(shell_vars->local, var_list);

	return command;
}
