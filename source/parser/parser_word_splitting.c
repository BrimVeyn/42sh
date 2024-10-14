/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_word_splitting.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 10:08:43 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/14 13:50:17 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "parser.h"
#include "lexer.h"
#include "utils.h"

static TokenList *ft_token_split(const char *str, const char *ifs) {
	da_create(tl, TokenList, sizeof(Token *), GC_SUBSHELL);
	const uint16_t str_len = ft_strlen(str);

	for (uint16_t i = 0; i < str_len; i++) {
		Token *word = gc(GC_ADD, token_empty_init(), GC_SUBSHELL);
		word->tag = T_WORD;
		token_word_init(word);

		const int start = i;
		while (i < str_len && !ft_strchr(ifs, str[i])) {
			i++;
		}
		const int end = i;

		if (start == end)
			word->w_infix = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL);
		else
			word->w_infix = gc(GC_ADD, ft_substr(str, start, end - start), GC_SUBSHELL);

		da_push(tl, word);
	}
	return tl;
}

static TokenList *word_splitter(char *str, Vars *shell_vars) {
	char *IFS_value = string_list_get_value(shell_vars->env, "IFS");
	if (!IFS_value) {
		IFS_value = "\n\t ";
	}

	TokenList *list = ft_token_split(str, IFS_value);
	if (list->size == 0) {
		Token *word = gc(GC_ADD, token_empty_init(), GC_SUBSHELL);
		word->tag = T_WORD;
		token_word_init(word);
		word->w_infix = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL); //must be NULL
		da_push(list, word);
	}
	return list;
}

bool parser_word_split(TokenList *dest, Vars *shell_vars, char *prefix, char *infix, char *postfix, int index) {
	TokenList *words = word_splitter(infix, shell_vars);
	token_list_remove(dest, index);
	token_list_insert_list(dest, words, index);

	Range range = {
		.start = index,
		.end = index + words->size - 1,
	};

	Token *first = dest->data[range.start];
	Token *last = dest->data[range.end];

	char *old_first_word = first->w_infix;
	char *old_last_word = last->w_infix;

	first->w_infix = gc(GC_ADD, ft_strjoin(prefix, first->w_infix), GC_SUBSHELL);
	last->w_infix = gc(GC_ADD, ft_strjoin(last->w_infix, postfix), GC_SUBSHELL);

	gc(GC_FREE, old_first_word, GC_SUBSHELL);
	gc(GC_FREE, old_last_word, GC_SUBSHELL);

	return true;
}
