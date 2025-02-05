/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_set_pattern.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/24 14:45:26 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/11 11:14:12 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_regex.h"
#include "libft.h"
#include "utils.h"

void set_single_char(regex_compiled_t *regexp, char c){
	regex_node_t *node = gc_unique(regex_node_t, GC_SUBSHELL);
	node->type = REGEX_SINGLE_CHAR;
	node->c = c;
	node->quantifier = REGEX_NO_QUANTIFIER;
	regex_append_node(regexp, node);
}

void set_quantifier(regex_compiled_t *regexp, regex_quantifier_type quantifier){
	regexp->pattern[regexp->size - 1]->quantifier = quantifier;
}

void set_end_of_pattern(regex_compiled_t *regexp){
	regex_node_t *node = gc_unique(regex_node_t, GC_SUBSHELL);
	node->type = REGEX_END_OF_PATTERN;
	node->quantifier = REGEX_NO_QUANTIFIER;
	regex_append_node(regexp, node);
}

void set_dot(regex_compiled_t *regexp){
	regex_node_t *node = gc_unique(regex_node_t, GC_SUBSHELL);
	node->type = REGEX_DOT;
	node->quantifier = REGEX_NO_QUANTIFIER;
	regex_append_node(regexp, node);
}

void set_end(regex_compiled_t *regexp){
	regex_node_t *node = gc_unique(regex_node_t, GC_SUBSHELL);
	node->type = REGEX_END;
	node->quantifier = REGEX_NO_QUANTIFIER;
	regex_append_node(regexp, node);
}

void set_begin(regex_compiled_t *regexp){
	regex_node_t *node = gc_unique(regex_node_t, GC_SUBSHELL);
	node->type = REGEX_BEGIN;
	node->quantifier = REGEX_NO_QUANTIFIER;
	regex_append_node(regexp, node);
}

void set_range(regex_compiled_t *regexp, const char *pattern, int *index){
	char range_string_list[256][4];
	int i = 0;
	int pattern_number = 0;
	
	ft_memset(range_string_list, 0, sizeof(range_string_list));
	if (pattern[0] == '^')
		i++;
	while(pattern[i] && pattern[i] != ']'){
		if (pattern[i] == '\\'){
			ft_memcpy(range_string_list[pattern_number], &pattern[i + 1], sizeof(char) * 2);
 			range_string_list[pattern_number][1] = '\0';
			i += 2;
			pattern_number++;
		}
		else if (pattern[i + 1] == '-'){
			ft_memcpy(range_string_list[pattern_number], &pattern[i], sizeof(char) * 4);
			range_string_list[pattern_number][3] = '\0';
			i += 3;
			pattern_number++;
		}
		else {
			ft_memcpy(range_string_list[pattern_number], &pattern[i], sizeof(char) * 2);
			range_string_list[pattern_number][1] = '\0';
			i++;
			pattern_number++;
		}
	}

	*index += i + 2;

	regex_node_t *node = gc_unique(regex_node_t, GC_SUBSHELL);

	node->type = REGEX_RANGE;
	if (pattern[0] == '^')
		node->type = REGEX_INVERT_RANGE;
	node->quantifier = REGEX_NO_QUANTIFIER;
	ft_memcpy(node->range, range_string_list, sizeof(range_string_list));
	regex_append_node(regexp, node);
}
