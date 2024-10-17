/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_syntax.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/12 10:12:20 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/17 13:20:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "parser.h"
#include "debug.h"
#include "libft.h"

#include <stdio.h>

bool is_whitespace_only(TokenList *tokens) {
	for (size_t i = 0; i < tokens->size; i++) {
		if (!is_newline(tokens, &i) && !is_eof(tokens, &i)) {
			return false;
		}
	}
	return true;
}

bool is_subshell_closed(TokenList *tokens, size_t *it) {
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

bool check_cmdgrp_closed(TokenList *tokens, size_t *it) {
	(*it) -= 2; //pass ';' '}'
	
	size_t it_cpy = *it;
	while (*it <= it_cpy) {
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

bool cmdgrp_parity(TokenList *tokens, size_t it) {
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
	for (size_t it = 0; it < tokens->size; it++) {
		if (is_subshell(tokens, &it)) {
			size_t it_1 = it + 1;
			if (is_end_sub(tokens, &it_1)) {
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
			dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tokens->data[it]->w_infix);
			return true;
		}
		if (it != 0 && is_end_cmdgrp(tokens, &it)) {
			int it_save = it;
			if (!check_cmdgrp_closed(tokens, &it)) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tokens->data[it_save]->w_infix);
				return true;
            }
			it = it_save;
		}
		if (is_separator(tokens, &it)) {
			if (it == 0) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(tokens->data[it]->s_type));
				return true;
			}
			if ((is_logical_operator(tokens, &it) || is_pipe(tokens, &it)) && it == tokens->size - 2) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(tokens->data[it]->s_type));
				return true;
			}
			if (is_separator(tokens, &it) && is_separator(tokens, &(size_t){it - 1})) {
				dprintf(2, UNEXPECTED_TOKEN_STR"`%s\'\n", tagStr(tokens->data[it]->s_type));
				return true;
			}
		}
		if (is_redirection(tokens, &it)){
			const Token *redir = is_redirection_tag(tokens, &it) ? tokens->data[it] : tokens->data[it]->w_postfix;
			if (tokens->data[it]->e != ERROR_NONE) {
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

bool is_redirection(const TokenList *tokens, const size_t *it){
	return is_redirection_tag(tokens, it) || (is_word(tokens, it) && tokens->data[*it]->w_postfix->tag == T_REDIRECTION);
}

bool is_redirection_tag(const TokenList *list, const size_t *it) {
	return (list->data[*it]->tag == T_REDIRECTION);
}

bool is_separator(const TokenList *tokens, const size_t *it) {
	return is_logical_operator(tokens, it) || is_break_seperator(tokens, it) || is_pipe(tokens, it);
}

bool is_logical_operator(const TokenList *tokens, const size_t *it) {
	return is_and(tokens, it) || is_or(tokens, it);
}

bool is_break_seperator(const TokenList *tokens, const size_t *it) {
	return is_semi(tokens, it) || is_bg(tokens, it); 
}

bool is_word(const TokenList *list, const size_t *it) {
	return (list->data[*it]->tag == T_WORD);
}

bool is_newline(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_NEWLINE);
}

bool is_pipe(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_PIPE);
}

bool is_eof(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_EOF);
}

bool is_semi(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_SEMI_COLUMN);
}

bool is_or(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_OR);
}

bool is_bg(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_BG);
}

bool is_and(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_AND);
}

bool is_subshell(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_SUB_OPEN);
}

bool is_end_sub(const TokenList *list, const size_t *i) {
	return list->data[*i]->tag == T_SEPARATOR && list->data[*i]->s_type == S_SUB_CLOSE;
}

bool is_semi_or_bg(const TokenList *list, const size_t *it) {
	return is_semi(list, it) || is_bg(list, it);
}

bool is_or_or_and(const TokenList *list, const size_t *it) {
	return is_or(list, it) || is_and(list, it);
}

bool is_ast_operator(const TokenList *list, const size_t *it) {
	return is_semi_or_bg(list, it) || is_or_or_and(list, it);
}

bool is_cmdgrp_start(const TokenList *list, const size_t *i) {
	if (*i == 0 && list->data[*i]->tag == T_WORD && !ft_strcmp(list->data[*i]->w_infix, "{")) {
		return true;
	} else if (*i > 0 && list->data[*i]->tag == T_WORD && !ft_strcmp(list->data[*i]->w_infix, "{")
		&& (is_separator(list, &(size_t){*i - 1}) || is_cmdgrp_start(list, &(size_t){*i -1}))) {
		 return true;
	} else {
		return false;
	}
}

bool is_end_cmdgrp(const TokenList *list, const size_t *it) {
	if (*it == 0) return false;
	return (is_cmdgrp_end(list, it) && is_semi(list, &(size_t){*it - 1}));
}

bool is_cmdgrp_end(const TokenList *list, const size_t *i) {
	return (list->data[*i]->tag == T_WORD && !ft_strcmp(list->data[*i]->w_infix, "}"));
}
