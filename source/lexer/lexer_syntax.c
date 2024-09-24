/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:12:20 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/24 09:31:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/lexer.h"
#include "../../include/parser.h"
#include "../../include/debug.h"
#include "../../libftprintf/header/libft.h"
#include <stdio.h>

bool is_whitespace_only(TokenList *tokens) {
	for (int i = 0; i < tokens->size; i++) {
		if (!is_newline(tokens, &i) && !is_eof(tokens, &i)) {
			return false;
		}
	}
	return true;
}

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

bool check_cmdgrp_closed(TokenList *tokens, int *it) {
	(*it) -= 2; //pass ';' '}'
	
	while (*it >= 0) {
		if (is_end_cmdgrp(tokens, it)) {
			if (!check_cmdgrp_closed(tokens, it))
				return false;
			(*it) -= 1;
			continue;
		}
		if (is_cmdgrp_start(tokens, it)) {
				return true;
		}
		(*it) -= 1;
	}
	(*it) -= 1;
	return false;
}

bool cmdgrp_parity(TokenList *tokens, int it) {
	size_t total[2] = {0, 0};

	while (it < tokens->size) {
		if (is_cmdgrp_end(tokens, &it))
			total[0]++;
		if (is_cmdgrp_start(tokens, &it))
			total[1]++;
		(it)++;
	}
	return (total[0] != total[1]);
}

bool lexer_syntax_error(TokenList *tokens) {
	if (is_whitespace_only(tokens)) 
		return true;
	for (int it = 0; it < tokens->size; it++) {
		if (is_subshell(tokens, &it)) {
			if (is_end_sub(tokens, &(int){it + 1})) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr((type_of_separator) S_SUB_CLOSE));
				return true;
            }
			int it_save = it;
			if (!is_subshell_closed(tokens, &it)) {
				dprintf(2, UNCLOSED_SUBSHELL_STR"`%s\'\n", tagStr((type_of_separator) S_SUB_CLOSE));
				return true;
			}
			it = it_save;
		}
		if (it == 0 && is_cmdgrp_end(tokens, &it)) {
			dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tokens->t[it]->w_infix);
			return true;
		}
		if (it != 0 && is_end_cmdgrp(tokens, &it)) {
			int it_save = it;
			if (!check_cmdgrp_closed(tokens, &it)) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tokens->t[it_save]->w_infix);
				return true;
            }
			it = it_save;
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
		if (is_redirection(tokens, &it)){
			const Token *redir = is_redirection_tag(tokens, &it) ? tokens->t[it] : tokens->t[it]->w_postfix;
			if (tokens->t[it]->e != ERROR_NONE) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(redir->r_type));
				return true;
			}
		}
	}
	if (cmdgrp_parity(tokens, 0)) {
		dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", "}");
		return true;
	}
	return false;
}

bool is_redirection(const TokenList *tokens, const int *it){
	return is_redirection_tag(tokens, it) || (is_word(tokens, it) && tokens->t[*it]->w_postfix->tag == T_REDIRECTION);
}

bool is_redirection_tag(const TokenList *list, const int *it) {
	return (list->t[*it]->tag == T_REDIRECTION);
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

bool is_newline(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_SEPARATOR && list->t[*i]->s_type == S_NEWLINE);
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

bool is_semi_or_bg(const TokenList *list, const int *it) {
	return is_semi(list, it) || is_bg(list, it);
}

bool is_or_or_and(const TokenList *list, const int *it) {
	return is_or(list, it) || is_and(list, it);
}

bool is_ast_operator(const TokenList *list, const int *it) {
	return is_semi_or_bg(list, it) || is_or_or_and(list, it);
}

bool is_cmdgrp_start(const TokenList *list, const int *i) {
	if (*i == 0 && list->t[*i]->tag == T_WORD && !ft_strcmp(list->t[*i]->w_infix, "{")) {
		return true;
	} else if (*i > 0 && list->t[*i]->tag == T_WORD && !ft_strcmp(list->t[*i]->w_infix, "{")
		&& (is_separator(list, &(int){*i - 1}) || is_cmdgrp_start(list, &(int){*i -1}))) {
		 return true;
	} else {
		return false;
	}
}

bool is_end_cmdgrp(const TokenList *list, const int *it) {
	if (*it == 0) return false;
	return (is_cmdgrp_end(list, it) && is_semi(list, &(int){*it - 1}));
}

bool is_cmdgrp_end(const TokenList *list, const int *i) {
	return (list->t[*i]->tag == T_WORD && !ft_strcmp(list->t[*i]->w_infix, "}"));
}
