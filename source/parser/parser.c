/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:27:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/14 16:22:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "ft_regex.h"
#include "libft.h"
#include "utils.h"
#include "lexer.h"
#include "exec.h"

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
			case EXP_ARITHMETIC: {result = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL); break;}
			case EXP_VARIABLE: {result = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL); break;}
			default: {}
		}
		gc(GC_FREE, str_list->data[i]->str, GC_SUBSHELL);
		str_list->data[i]->str = gc(GC_ADD, result, GC_SUBSHELL);
	}
}

#include <stdarg.h>

ExpKind identify_exp_begin(char *str, int *flag) {
	if (!ft_strncmp(str, "$((", 3)) {
		return EXP_ARITHMETIC;
	} else if (!ft_strncmp(str, "$(", 2)) {
		return EXP_CMDSUB;
	} else if (!*flag && !ft_strncmp(str, "${", 2)) {
		return EXP_VARIABLE;
	} else if (!ft_strncmp(str, "(", 1)) {
		*flag = 1;
		return EXP_SUB;
	} else {
		return EXP_WORD;
	}
}

ExpKind identify_exp_end(char *str, int *flag) {
	if (*flag == 1) {
		if (!ft_strncmp(str, ")", 1)) {
			*flag = 0;
			return EXP_SUB;
		} else {
			return EXP_WORD;
		}
	}
	if (!ft_strncmp(str, "))", 2)) {
		return EXP_ARITHMETIC;
	} else if (!ft_strncmp(str, ")", 1)) {
		return EXP_CMDSUB;
	} else if (!ft_strncmp(str, "}", 1)) {
		return EXP_VARIABLE;
	} else {
		return EXP_WORD;
	}
}

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

	da_create(exp_stack, StringList, sizeof(char *), GC_SUBSHELL);
	bool can_push = false;
	int flag = 0;
	while (word->size) {
		ExpKind maybe_begin = 0;
		ExpKind maybe_end = 0;

		if ((maybe_begin = identify_exp_begin(word->data, &flag)) != EXP_WORD) {
			if (!exp_stack->size && can_push) {
				Str *res = gc_unique(Str, GC_SUBSHELL);
				res->kind = EXP_WORD;
				da_push(cache_stack, '\0');
				res->str = gc(GC_ADD, ft_strdup(cache_stack->data), GC_SUBSHELL);
				da_clear(cache_stack);
				da_push(self, res);
			}
			da_push(exp_stack, map[maybe_begin].begin);
			for (size_t i = 0; i < ft_strlen(map[maybe_begin].begin); i++) {
				da_push(cache_stack, da_pop_front(word));
			}
		}
		if (exp_stack->size && (maybe_end = identify_exp_end(word->data, &flag)) != EXP_WORD) {
			for (size_t i = 0; i < ft_strlen(map[maybe_end].end); i++) {
				da_push(cache_stack, da_pop_front(word));
			}
			char *stack_back = da_peak_back(exp_stack);
			if (!ft_strncmp(map[maybe_end].begin, stack_back, ft_strlen(map[maybe_end].begin))) {
				da_pop(exp_stack);
				if (!exp_stack->size) {
					da_push(cache_stack, '\0');
					da_push(self, str_init(maybe_end, gc(GC_ADD, ft_strdup(cache_stack->data), GC_SUBSHELL)));
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

#include "debug.h"

void parse_candidate(Token *candidate, Vars *shell_vars) {
	StrList *string_list = get_range_list(candidate);
	// da_print(string_list);
	string_list_consume(string_list, shell_vars);
	// da_print(string_list);
}

SimpleCommand *parser_parse_current(TokenList *tl, Vars *shell_vars) {
	// parser_brace_expansion(); TODO: we'll find time
	
	for (size_t it = 0; it < tl->size; it++) {
		Token *candidate = get_candidate(tl, it);
		if (!candidate)
			continue;
		parse_candidate(candidate, shell_vars);
	}
	TokenList *command_vars = parser_eat_variables(tl);
	RedirectionList *redirs = parser_get_redirection(tl);
	SimpleCommand *command = parser_get_command(tl);
	command->redir_list = redirs;

	if (command->bin == NULL && command_vars->size != 0) {
		add_vars_to_set(shell_vars, command_vars);
    } else {
		add_vars_to_local(shell_vars->local, command_vars);
	}

	return command;
}
