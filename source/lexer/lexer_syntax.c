/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:28:52 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/12 09:23:39 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include "lexer.h"
#include "lexer_enum.h"

bool is_subshell_closed(TokenList *tokens, int *it) {
	(*it)++;
	while ((*it) < tokens->size && !is_end_sub(tokens, it)) {
		if (is_subshell(tokens, it)) {
			if (!is_subshell_closed(tokens, it)) {
				return false;
			}
		}
		(*it)++;
	}
	if ((*it) >= tokens->size || !is_end_sub(tokens, it)) {
		return false;
	}
	return true;
}

bool lexer_syntax_error(TokenList *tokens) {
	for (int it = 0; it < tokens->size; it++) {
		if (is_subshell(tokens, &it) && !is_subshell_closed(tokens, &it)) {
			dprintf(2, UNCLOSED_SUBSHELL_STR"`%s\'\n", tagStr((type_of_separator) S_SUB_CLOSE));
			return true;
		}
		if (is_separator(tokens, &it)) {
			if (it == 0) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(tokens->t[it]->s_type));
				return true;
			}
			if ((is_logical_operator(tokens, &it) || is_pipe(tokens, &it)) && it == tokens->size - 2) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(tokens->t[it]->s_type));
				return true;
			}
			if (is_separator(tokens, &it) && is_separator(tokens, &(int){it - 1})) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(tokens->t[it]->s_type));
				return true;
			}
		}
		if (is_redirection(tokens, &it)) {
			const Token *redir = is_redirection_tag(tokens, &it) ? tokens->t[it] : tokens->t[it]->w_postfix;
			if (tokens->t[it]->e != ERROR_NONE) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(redir->r_type));
				return true;
			}
		}
	}
	return false;
}

bool is_redirection(const TokenList *tokens, const int *it) {
	return is_redirection_tag(tokens, it) || (is_word(tokens, it) && tokens->t[*it]->w_postfix->tag == T_REDIRECTION);
}

bool is_separator(const TokenList *tokens, const int *it) {
	return is_logical_operator(tokens, it) || is_break_seperator(tokens, it) || is_pipe(tokens, it);
}

bool is_logical_operator(const TokenList *tokens, const int *it) {
	return is_and(tokens, it) || is_or(tokens, it);
}

bool is_break_seperator(const TokenList *tokens, const int *it) {
	return is_semi(tokens, it) || is_bg(tokens, it); 
}

bool is_word(const TokenList *list, const int *it) {
	return (list->t[*it]->tag == T_WORD);
}

bool is_redirection_tag(const TokenList *list, const int *it) {
	return (list->t[*it]->tag == T_REDIRECTION);
}

bool is_pipe(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_PIPE);
}

bool is_eof(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_EOF);
}

bool is_semi(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_SEMI_COLUMN);
}

bool is_or(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_OR);
}

bool is_bg(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_BG);
}

bool is_and(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_AND);
}

bool is_subshell(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_SUB_OPEN);
}

bool is_end_sub(const TokenList *list, const int *i) {
	return list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_SUB_CLOSE;
}
