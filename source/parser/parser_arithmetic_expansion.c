/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_arithmetic_expansion.c                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/18 15:46:07 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/19 15:20:37 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include "parser.h"

int get_arithmetic_exp_range_end(char *str, int *i) {
	(*i) += 3;

	while (str[*i] && ft_strncmp("))", &str[*i], 2)) {
		if (!ft_strncmp("$((", &str[*i], 3)) {
			get_arithmetic_exp_range_end(str, i);
			if (*i == -1) return -1;
		}
		(*i)++;
	}
	if (ft_strncmp(&str[*i], "))", 2)) return -1;
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
		self.end = get_arithmetic_exp_range_end(str, &start_copy);
	}
	return self;
}

bool parser_arithmetic_expansion(TokenList *tokens, StringList *env) {
	(void) tokens;
	(void) env;

	int i = 0;

	while (i < tokens->size) {
		Token *elem = tokens->t[i];

		if (!is_word(tokens, &i)) {
			i += 1;
			continue;
		}

		const Range range = get_arithmetic_exp_range(elem->w_infix);
		if (is_a_match(range)) {
			if (!is_range_valid(range, UNCLOSED_ARITMETIC_EXP_STR)) {
				g_exitno = 126;
				return false;
			} else i += 1; continue;
		}
		dprintf(2, "r.start: %d, r.end: %d\n", range.start, range.end);

		char infix[MAX_WORD_LEN] = {0};
		ft_memcpy(infix, &elem->w_infix[range.start + 3], (range.end - range.start) - 3);

		dprintf(2, "infix = %s\n", infix);

		i++;
	}


	return true;
}
