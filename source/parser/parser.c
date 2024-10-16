/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:27:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/16 10:19:08 by bvan-pae         ###   ########.fr       */
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
	const Token *next = (el->tag == T_WORD && el->w_postfix->tag == T_REDIRECTION) ? el->w_postfix : el;
	Redirection *current_redir = (Redirection *) gc(GC_ADD, ft_calloc(1, sizeof(Redirection)), GC_SUBSHELL);

	current_redir->prefix_fd = (next != el) ? ft_atol(el->w_infix) : -1;
	current_redir->r_type = next->r_type;
	if (is_number(next->r_postfix->w_infix) &&
		(next->r_type == R_DUP_OUT ||
		next->r_type == R_DUP_IN)) {
		current_redir->su_type = R_FD;
		current_redir->fd = ft_atol(next->r_postfix->w_infix);
	} else {
		current_redir->su_type = R_FILENAME;
		current_redir->filename = next->r_postfix->w_infix;
	}
	da_push(redir_list, current_redir);
}

RedirectionList *parser_get_redirection(const TokenList *tl) {
	da_create(redir_list, RedirectionList, sizeof(Redirection *), GC_SUBSHELL);
	for (size_t it = 0; it < tl->size; it++) {
		const Token *el = tl->data[it];
		if (is_redirection(tl, &it))
			add_redirection_from_token(redir_list, el);
	}
	return redir_list;
}

SimpleCommand *parser_get_command(const TokenList *tl) {
	SimpleCommand *curr_command = (SimpleCommand *) gc(GC_ADD, ft_calloc(1, sizeof(SimpleCommand)), GC_SUBSHELL);

	size_t count = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->data[it];
		count += (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION);
	}

	curr_command->args = (char **) gc(GC_ADD, ft_calloc(count + 1, sizeof(char *)), GC_SUBSHELL);

	size_t i = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->data[it];
		if (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION) {
			if (i == 0) {
				curr_command->bin = el->w_infix;
			}
			curr_command->args[i++] = el->w_infix;
		}
	}
	curr_command->args[i] = NULL;
	return curr_command;
}

TokenList *parser_eat_variables(TokenList *tokens) {
	da_create(self, TokenList, sizeof(Token *), GC_SUBSHELL);
	bool found_bin = false;
	size_t i = 0;

	while (i < tokens->size && !found_bin) {
		Token *elem = tokens->data[i];
		if (!is_word(tokens, &i) || (is_word(tokens, &i) && is_redirection(tokens, &i))) {
			i += 1;
			continue;
		}

		char *word = elem->w_infix;
		if (!word) {
			i += 1;
			continue;
		}

		if (regex_match("^[_a-zA-Z][a-zA-Z1-9_]*=", word).re_start == -1) {
			found_bin = true;
			i += 1;
			continue;
        }
		da_push(self, elem);
		token_list_remove(tokens, i);
	}
	return self;
}

Token *get_candidate(TokenList *tl, const size_t idx) {
	if (is_word(tl, &idx)) {
		return tl->data[idx];
	} else if (is_redirection_tag(tl, &idx)) {
		return tl->data[idx]->r_postfix;
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
			case EXP_VARIABLE: {result = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL); break;}
			default: {}
		}
		if (result) {
			gc(GC_FREE, str_list->data[i]->str, GC_SUBSHELL);
			str_list->data[i]->str = gc(GC_ADD, result, GC_SUBSHELL);
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

ExpKind identify_exp_end(char *str, ExpKind context) {
	if (str[0] == ')' && (context == EXP_CMDSUB || context == EXP_SUB)) {
		return context;
	} else if (!ft_strncmp(str, "))", 2)) {
		return EXP_ARITHMETIC;
	} else if (str[0] == '}' && (context == EXP_VARIABLE)) {
		return EXP_VARIABLE;
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

StrList *get_range_list(Token *candidate) {
	da_create(self, StrList, sizeof(Str *), GC_SUBSHELL);
	da_create(word, StringStream, sizeof(char), GC_SUBSHELL);
	da_create(cache_stack, StringStream, sizeof(char), GC_SUBSHELL);
	for (size_t i = 0; candidate->w_infix[i]; i++) {
		da_push(word, candidate->w_infix[i]);
	}
	
	static const struct {
		char *begin;
		char *end;
	} map[] = {
		[EXP_ARITHMETIC] = { "$((", "))" },
		[EXP_CMDSUB] = { "$(", ")"},
		[EXP_VARIABLE] = { "${", ")" },
		[EXP_SUB] = { "(", ")" },
	};

	da_create(exp_stack, ExpKindList, sizeof(ExpKind), GC_SUBSHELL);
	bool can_push = false;
	while (word->size) {
		ExpKind maybe_begin = 0;
		ExpKind maybe_end = 0;
		ExpKind context = da_peak_back(exp_stack);

		if ((maybe_begin = identify_exp_begin(word->data)) != EXP_WORD) {
			if (!exp_stack->size && can_push) {
				Str *res = gc_unique(Str, GC_SUBSHELL);
				res->kind = EXP_WORD;
				da_push(cache_stack, '\0');
				res->str = gc(GC_ADD, ft_strdup(cache_stack->data), GC_SUBSHELL);
				da_clear(cache_stack);
				da_push(self, res);
			}
			da_push(exp_stack, maybe_begin);
			for (size_t i = 0; i < ft_strlen(map[maybe_begin].begin); i++) {
				da_push(cache_stack, da_pop_front(word));
			}
			continue;
		}
		if (exp_stack->size && (maybe_end = identify_exp_end(word->data, context)) != EXP_WORD) {
			if (maybe_end == context) {
				da_pop(exp_stack);
				for (size_t i = 0; i < ft_strlen(map[maybe_end].end); i++) {
					da_push(cache_stack, da_pop_front(word));
				}
				if (!exp_stack->size) {
					da_push(cache_stack, '\0');
					da_push(self, str_init(maybe_end, ft_strdup(cache_stack->data)));
					da_clear(cache_stack);
				}
			} 
			can_push = false;
		} else {
			da_push(cache_stack, da_pop_front(word));
			can_push = true;
		}
	}
	if (can_push) {
		Str *res = gc_unique(Str, GC_SUBSHELL);
		res->kind = EXP_WORD;
		da_push(cache_stack, '\0');
		res->str = gc(GC_ADD, ft_strdup(cache_stack->data), GC_SUBSHELL);
		da_push(self, res);
	}
	gc(GC_FREE, cache_stack->data, GC_SUBSHELL);
	gc(GC_FREE, word->data, GC_SUBSHELL);
	return self;
}

void ss_push_string(StringStream *ss, char *str) {
	for (size_t i = 0; str[i]; i++) {
		da_push(ss, str[i]);
	}
}

char	*ft_strchrany(const char *string, const char *searchedChars) {
	if (!searchedChars)
		return NULL;
	while (*string) {
		for (size_t i = 0; searchedChars[i]; i++) {
			if (*string == searchedChars[i]) {
				return ((char *) string);
			}
		}
		string++;
	}
	return (NULL);
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
	char *IFS = string_list_get_value(shell_vars->env, "IFS");
	if (!IFS) {
		IFS = "\n\t ";
	}

	unsigned char map[32] = {[0 ... 31] = 0};

	for (size_t i = 0; IFS[i]; i++) {
		const unsigned char c = IFS[i];
		map[c / 8] |= (1 << (c % 8));
	}

	for (size_t i = 0; i < list->size; i++) {
		if (list->data[i]->kind != EXP_WORD) {
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
					str_add_back(&node, str_init(list->data[i]->kind, ft_strdup("")));
				} else if (start != end) {
					str_add_back(&node, str_init(list->data[i]->kind, ft_substr(curr_str, start, end - start)));
				}
			}
			gc(GC_FREE, list->data[i]->str, GC_SUBSHELL);
			gc(GC_FREE, list->data[i], GC_SUBSHELL);
			list->data[i] = node;
		}
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
				da_push(ss, '\0');
				da_push(new, ft_strdup(ss->data));
				da_clear(ss);
			}
		} else {
			Str *node = curr;
			while (node) {
				ss_push_string(ss, node->str);
				if (!(node->next == NULL && next)) {
					da_push(ss, '\0');
					da_push(new, ft_strdup(ss->data));
					da_clear(ss);
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

SimpleCommand *parser_parse_current(TokenList *tl, Vars *shell_vars) {

	SimpleCommand *command = gc_unique(SimpleCommand, GC_SUBSHELL);
	da_create(arg_list, StringList, sizeof(Str), GC_SUBSHELL);
	for (size_t it = 0; it < tl->size; it++) {
		Token *candidate = get_candidate(tl, it);
		if (!candidate)
			continue;
		StrList *string_list = get_range_list(candidate);
		// da_print(string_list);
		string_list_consume(string_list, shell_vars);
		string_list_split(string_list, shell_vars);
		string_erase_nulls(string_list);
		StringList *final = string_list_merge(string_list);
		// da_print(string_list);
		string_list_push_list(arg_list, final);
	}
	// for (size_t i = 0; i < arg_list->size; i++) {
	// 	dprintf(2, "[%02zu]: %s\n", i, arg_list->data[i]);
	// }
	command->bin = arg_list->data[0];
	command->args = arg_list->data;

	TokenList *command_vars = parser_eat_variables(tl);
	RedirectionList *redirs = parser_get_redirection(tl);
	// SimpleCommand *command = parser_get_command(tl);
	command->redir_list = redirs;

	if (command->bin == NULL && command_vars->size != 0) {
		add_vars_to_set(shell_vars, command_vars);
    } else {
		add_vars_to_local(shell_vars->local, command_vars);
	}

	return command;
}
