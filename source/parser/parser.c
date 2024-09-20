/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:27:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/20 16:04:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

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
	return gc_add(ft_strdup(filename), GC_GENERAL);
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
	Redirection *current_redir = (Redirection *) gc_add(ft_calloc(1, sizeof(Redirection)), GC_SUBSHELL);

	current_redir->prefix_fd = (next != el) ? ft_atoi(el->w_infix) : -1;
	current_redir->r_type = next->r_type;
	if (is_number(next->r_postfix->w_infix) &&
		(next->r_type == R_DUP_OUT ||
		next->r_type == R_DUP_IN)) {
		current_redir->su_type = R_FD;
		current_redir->fd = ft_atoi(next->r_postfix->w_infix);
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
	SimpleCommand *curr_command = (SimpleCommand *) gc_add(ft_calloc(1, sizeof(SimpleCommand)), GC_SUBSHELL);

	size_t count = 0;
	for (uint16_t it = 0; it < tl->size; it++) {
		const Token *el = tl->t[it];
		count += (el->tag == T_WORD && el->w_postfix->tag != T_REDIRECTION);
	}

	curr_command->args = (char **) gc_add(ft_calloc(count + 1, sizeof(char *)), GC_SUBSHELL);

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
	int i = 0;

	while (i < tokens->size) {
		Token *elem = tokens->t[i];
		if (!is_word(tokens, &i) || (is_word(tokens, &i) && is_redirection(tokens, &i))) {
			i += 1;
			continue;
		}

		char *word = elem->w_infix;

		if (regex_match("^[_a-zA-Z][a-zA-Z1-9_]*=", word).start == -1) {
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
	for (int i = 0; i < vars->size; i++) {
		string_list_add_or_update(set, vars->t[i]->w_infix);
	}
}

SimpleCommand *parser_parse_current(TokenList *tl, Vars *shell_vars) {
	// parser_brace_expansion();
	// parser_tilde_expansion();

	//AU BOULOT
	if (!parser_parameter_expansion(tl, shell_vars)){
		return NULL;
	}
	if (!parser_command_substitution(tl, shell_vars)) {
		return NULL;
	}
	// if (!parser_arithmetic_expansion(tl, shell_vars)) {
	// 	return NULL;
	// }
	// if (!parser_filename_expansion(tl)){
	// 	return NULL;
	// }
	TokenList *command_vars = parser_eat_variables(tl);
	RedirectionList *redirs = parser_get_redirection(tl);
	SimpleCommand *command = parser_get_command(tl);

	if (command->bin == NULL)
		add_vars_to_set(shell_vars, command_vars);

	command->redir_list = redirs;
	return command;
}
