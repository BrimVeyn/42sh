/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_arithmetic_expansion.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:46:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/20 16:00:16 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

int get_arithmetic_exp_range_end(char *str, int *i, Range *range) {
	(*i) += 3; //skip '$(('

	while (str[*i] && ft_strncmp("))", &str[*i], 2)) {
		if (!ft_strncmp("$((", &str[*i], 3)) {
			range->start = *i;
			return get_arithmetic_exp_range_end(str, i, range);
			if (*i == -1) return -1;
		}
		(*i)++;
	}
	if (ft_strncmp(&str[*i], "))", 2)) return -1;
	(*i) += 1;
	return *i;
}

Range get_arithmetic_exp_range(char *str) {
	Range self = {
		.start = -1,
		.end = -1,
	};

	self.start = ft_strstr(str, "$((");
	if (self.start != -1) {
		int start_copy = self.start;
		self.end = get_arithmetic_exp_range_end(str, &start_copy, &self);
	}
	return self;
}

bool parser_arithmetic_expansion(TokenList *tokens, Vars *shell_vars) {
	(void) tokens;
	(void) shell_vars;

	int i = 0;

	while (i < tokens->size) {
		Token *elem = tokens->t[i];

		if (!is_word(tokens, &i)) {
			i += 1;
			continue;
		}

		const Range range = get_arithmetic_exp_range(elem->w_infix);
		// dprintf(2, "r.start: %d, r.end: %d\n", range.start, range.end);
		if (is_a_match(range)) {
			if (!is_range_valid(range, UNCLOSED_ARITMETIC_EXP_STR)) {
				g_exitno = 126;
				return false;
			} else i += 1; continue;
		}

		char infix[MAX_WORD_LEN] = {0};
		ft_memcpy(infix, &elem->w_infix[range.start + 3], (range.end - range.start) - 4);

		Lexer_p lexer = lexer_init(infix);
		ATokenList *tokens = lexer_arithmetic_exp_lex_all(lexer);
		aTokenListToString(tokens);

		// dprintf(2, "infix = %s\n", infix);

		i++;
	}


	return true;
}
