/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:27:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/11 17:04:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/parser.h"
#include "../../include/ft_regex.h"
#include "../../libftprintf/header/libft.h"
#include "../../include/utils.h"
#include "lexer.h"
#include "colors.h"

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
		const Token *curr = data->t[it];
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

void add_redirection_from_token(RedirectionList **redir_list, const Token *el) {
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
	redirection_list_add(*redir_list, current_redir);
}

RedirectionList *parser_get_redirection(const TokenList *tl) {
	RedirectionList *redir_list = redirection_list_init();
	for (int it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		if (is_redirection(tl, &it))
			add_redirection_from_token(&redir_list, el);
	}
	return redir_list;
}

SimpleCommand *parser_get_command(const TokenList *tl) {
	SimpleCommand *curr_command = (SimpleCommand *) gc(GC_ADD, ft_calloc(1, sizeof(SimpleCommand)), GC_SUBSHELL);

	size_t count = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		count += (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION);
	}

	curr_command->args = (char **) gc(GC_ADD, ft_calloc(count + 1, sizeof(char *)), GC_SUBSHELL);

	size_t i = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
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
	TokenList *self = token_list_init();
	bool found_bin = false;
	int i = 0;

	while (i < tokens->size && !found_bin) {
		Token *elem = tokens->t[i];
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
		token_list_add(self, elem);
		token_list_remove(tokens, i);
	}
	return self;
}

void add_vars_to_set(Vars *shell_vars, TokenList *vars) {
	StringList *set = shell_vars->set;
	StringList *env = shell_vars->env;
	for (int i = 0; i < vars->size; i++) {
		string_list_update(env, vars->t[i]->w_infix);
		string_list_add_or_update(set, vars->t[i]->w_infix);
	}
}

void add_vars_to_local(StringList *list, TokenList *vars) {
	for (int i = 0; i < vars->size; i++) {
		string_list_add_or_update(list, vars->t[i]->w_infix);
	}
}

Token *get_candidate(TokenList *tl, const int idx) {
	if (is_word(tl, &idx)) {
		return tl->t[idx];
	} else if (is_redirection_tag(tl, &idx)) {
		return tl->t[idx]->r_postfix;
	} else {
		return NULL;
	}
}

ExpRangeList *exp_range_list_init(void) {
	ExpRangeList *self = gc(GC_ALLOC, 1, sizeof(ExpRangeList), GC_SUBSHELL);
	self->data = gc(GC_CALLOC, 10, sizeof(ExpRange *), GC_SUBSHELL);
	self->capacity = 10;
	self->size = 0;
	return self;
}

ExpRange *exp_range_init(void) {
	ExpRange *self = gc(GC_CALLOC, 1, sizeof(ExpRange), GC_SUBSHELL);
	self->end = -1;
	self->start = -1;
	return self;
}

typedef enum {
	VALID,
	ERROR,
	DONE,
} ErrorCode;

uint8_t find_command_sub_ranges(ExpRangeList *ptr, Token *candidate) {
	ssize_t idx = 0;

	int i = 0;
	ssize_t candidate_len = ft_strlen(candidate->w_infix);
	//TODO: remove debug mode i < 10 by true
	while (i < 10) {
		if (idx >= candidate_len - 1) {
			return DONE;
		}
		char *candidat_str = &candidate->w_infix[idx];
		ExpRange *range = get_command_sub_range(candidat_str);
		// dprintf(2, "s: %ld e: %ld\n", range->start, range->end);
		if (is_a_match(range)) {
			if (!is_range_valid(range, UNCLOSED_COMMAND_SUB_STR)) {
				g_exitno = 126;
				return ERROR;
			}
		} else {
			return DONE;
		}
		range->kind = EXP_CMDSUB;
		range->start += idx;
		range->end += idx;
		idx = range->end;
		da_add(ptr, range);
		i++;
	}
	return DONE;
}

void exp_range_print(ExpRangeList *list) {
	const char *map[3] = {
		[EXP_ARITHMETIC] = "ARITHMETIC",
		[EXP_CMDSUB] = "CMD_SUB",
		[EXP_VARIABLE] = "VARIABLE",
	};

	for (size_t i = 0; i < list->size; i++) {
		dprintf(2, "["C_LIGHT_GREEN"%zu"C_RESET"]: "C_MAGENTA"%s"C_RESET": {%ld, %ld}\n", i, map[list->data[i]->kind], list->data[i]->start, list->data[i]->end);
	}
}

StrList *post_exp_list_init(ExpRangeList *ranges, Token *candidate) {
	StrList *self = str_list_init();
	char *word = candidate->w_infix;
	const ssize_t word_size = ft_strlen(word);
	ssize_t i = 0;
	ssize_t range_it = 0;

	while (i < word_size) {
		const ExpRange *curr = ((size_t)range_it < ranges->size) ? ranges->data[range_it] : NULL;
		if (curr && curr->start <= i) {
			char *el = gc(GC_ADD, ft_substr(word, curr->start + 2, curr->end - curr->start - 2), GC_SUBSHELL);
			da_add(self, str_init(curr->kind, el));
			range_it++;
			i = curr->end + 1;
			continue;
		} else if (curr) {
			char *el = gc(GC_ADD, ft_substr(word, i, curr->start - i), GC_SUBSHELL);
			da_add(self, str_init(EXP_WORD, el));
			i = curr->start;
			continue;
		} else {
			char *el = gc(GC_ADD, ft_substr(word, i, word_size), GC_SUBSHELL);
			da_add(self, str_init(EXP_WORD, el));
			i = word_size;
			continue;
		}
	}
	//DEBUG
	str_list_print(self);
	return self;
}

void exp_list_consume(StrList *str_list, Vars *shell_vars) {
	for (size_t i = 0; i < str_list->size; i++) {
		char *result = NULL;
		Str *curr = str_list->data[i];
		ExpKind kind = curr->kind;

		if (curr->kind == EXP_WORD) 
			continue;

		switch (kind) {
			case EXP_CMDSUB: { result = parser_command_substitution(curr->str, shell_vars); break;}
			case EXP_ARITHMETIC: {break;}
			case EXP_VARIABLE: {break;}
			default: {}
		}
		gc(GC_FREE, str_list->data[i]->str, GC_SUBSHELL);
		str_list->data[i]->str = gc(GC_ADD, result, GC_SUBSHELL);
	}
}

ExpRangeList *get_range_list(Token *candidate) {
	ExpRangeList *self = exp_range_list_init();
	char *str = candidate->w_infix;
	const size_t word_len = ft_strlen(str);

	size_t i = 0;
	size_t si = i;
	(void)si;
	while (i < word_len) {
		if (ft_strncmp("$((", &str[i], 3)) {
			// ExpRange *el = eat_arithmetic(candidate, i);
		} else if (ft_strncmp("$(", &str[i], 2)) {
			// ExpRange *el = eat_arithmetic(candidate, i);
		} else if (ft_strncmp("${", &str[i], 2)) {
			// ExpRange *el = eat_arithmetic(candidate, i);
		}
	}
	return self;
}

TokenList *get_exp_ranges(Token *candidate, Vars *shell_vars) {
	// TODO: Fill up get_range_list so it can consume any kind of expansion
	// ExpRangeList *range_list = get_range_list(candidate);

	ExpRangeList *range_list = exp_range_list_init();

	if (find_command_sub_ranges(range_list, candidate) == ERROR) {
		//TODO: free range_list;
		return NULL;
	}
	//TODO: find variable ranges
	//TODO: find arithmetic ranges
	if (range_list->size == 0) {
		return NULL;
	}
	exp_range_print(range_list);
	StrList *list = post_exp_list_init(range_list, candidate);
	//TODO: free range_list
	exp_list_consume(list, shell_vars);
	str_list_print(list);
	//TODO: join list
	//TODO: apply word split based on ifs, if KIND != WORD --> new token
	// exp_range_consume(range_list);
	return token_list_init();
}

SimpleCommand *parser_parse_current(TokenList *tl, Vars *shell_vars) {
	// parser_brace_expansion(); TODO: we'll find time
	
	for (int it = 0; it < tl->size; it++) {
		Token *candidate = get_candidate(tl, it);
		if (!candidate)
			continue;
		TokenList *new_token = get_exp_ranges(candidate, shell_vars);
		(void) new_token;
		// parser_arithmetic_expansion(tl, it, 0, shell_vars);
	}


	if (!parser_parameter_expansion(tl, shell_vars)){
		return NULL;
	}
	// if (!parser_command_substitution(tl, shell_vars)) {
	// 	return NULL;
	// }
	// if (!parser_arithmetic_expansion(tl, shell_vars)) {
	// 	return NULL;
	// }
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
