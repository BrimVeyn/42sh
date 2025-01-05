/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pattern_matching.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/29 23:38:11 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/03 11:48:24 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "expansion.h"
#include "ft_regex.h"
#include "dynamic_arrays.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>

static int fill_map_with_character_class(char *character_class_name, char *map, const bool reverse) {
	static const struct {
		char *name;
		char map[256];
	} lookup[] = {
		{ .name = "upper", .map = {['A'...'Z']= 1}, },
		{ .name = "lower", .map = {['a'...'z']= 1}, },
		{ .name = "digit", .map = {['0'...'9']= 1}, },
		{ .name = "space", .map = {['\t'...'\r']= 1, [' ']= 1 }, },
		{ .name = "cntrl", .map = {['\0'...31]= 1, [127]= 1}, },
		{ .name = "punct", .map = {['!'...'/']= 1, [':'...'@']= 1, ['\['...'`']= 1, ['\{'...'~']= 1}, },
		{ .name = "xdigit", .map = {['0'...'9']= 1, ['a'...'f']= 1, ['A'...'F']= 1,}, },
		{ .name = "blank", .map = {[' '] = 1, ['\t'] = 1}, },
		{ .name = "alpha", .map = {['A'...'Z']= 1, ['a'...'z']= 1,}, },
		{ .name = "alnum", .map = {['A'...'Z']= 1, ['a'...'z']= 1, ['0'...'9']= 1,}, },
		{ .name = "graph", .map = {['A'...'Z']= 1, ['a'...'z']= 1, ['0'...'9']= 1,
								   ['!'...'/']= 1, [':'...'@']= 1, ['\['...'`']= 1, ['\{'...'~']= 1}, },
		{ .name = "print", .map = {['A'...'Z']= 1, ['a'...'z']= 1, ['0'...'9']= 1,
								   ['!'...'/']= 1, [':'...'@']= 1, ['\['...'`']= 1, ['\{'...'~']= 1,
								   ['\t'...'\r']= 1, [' ']= 1 }, },
	};

	for (size_t i = 0; i < ARRAY_SIZE(lookup); i++) {
		if (!ft_strcmp(character_class_name, lookup[i].name)) {
			for (size_t j = 0; j < 256; j++) {
				if (reverse) { map[j] ^= lookup[i].map[j]; }
				else { map[j] |= lookup[i].map[j]; }
			}
			return 0;
		}
	}
	return -1;
}

static char *compile_range(char **pattern) {

	char *str = *pattern;
	bool reverse = (*str == '^' || *str == '!');
	char *map = gc(GC_ALLOC, 256, sizeof(char), GC_SUBSHELL);

	if (reverse) ft_memset(map, 1, 256);
	else ft_memset(map, 0, 256);
	str += (reverse);
	// dprintf(2, "str: %s\n", str);
	while(*str) {
		regex_match_t match;
		if ((match = regex_match("^\\[\\:[a-zA-Z0-9]*\\:\\]", str)).is_found) {
			char *const character_class_name = ft_substr(str, 2, match.re_end - 2 - 2);
			fill_map_with_character_class(character_class_name, map, reverse);
			str += (match.re_end); //reposition pointer after the closing ]
			free(character_class_name);
			continue;
		}
		if ((*str) == '-') { str++; continue; }
		if ((*str) == ']')
			break;
		if ((*(str + 1)) != 0 && *(str + 1) == '-') {
			unsigned char start = (*str);
			str++;
			while (*str == '-') str++;
			if (*str) {
				unsigned char end = (*str);
				if (start < end)
					ft_memset(&map[start], !(reverse), end - start + 1);
				// dprintf(2, "s: %c | e: %c\n", start, end);
			}
		} else {
			map[(unsigned char)(*str)] = !(reverse);
		}
		str++;
	}
	*pattern = str;
	// for (size_t i = 0; reverse && i < 256; i++)
	// 	map[i] = !map[i];
	return map;
}

PatternNodeL *compile_pattern(char *pattern) {
    da_create(list, PatternNodeL, sizeof(PatternNode), GC_SUBSHELL);

    static bool dquote = false, squote = false;
    const char *special = "*?[";

	while(*pattern) {
		if		(*pattern == '\"' && !squote) { dquote = !dquote; pattern++; continue; }
		else if (*pattern == '\'' && !dquote) { squote = !squote; pattern++; continue; }

		if (!squote && *pattern == '\\') {
			pattern++; //skip backslash
			PatternNode node = { .type = P_CHAR, .c = *pattern };
			da_push(list, node);
			pattern++; //skip escaped char
			continue;
		}

        if (!ft_strchr(special, *pattern) || squote || dquote) {
            PatternNode node = { .type = P_CHAR, .c = *pattern };
            da_push(list, node);
        } else {
            if (*pattern == '*') {
                PatternNode node = { .type = P_STAR, .c = 0 };
                da_push(list, node);
            } else if (*pattern == '?') {
				PatternNode node = { .type = P_QMARK, .c = 0 };
                da_push(list, node);
            } else { //pattern[i] == [
				if (ft_strchr(pattern, ']')) {
					PatternNode node = { .type = P_RANGE, .c = 0};
					pattern++; //skip [
					// dprintf(2, "pattern: %s\n", pattern);
					node.map = compile_range(&pattern);
					// dprintf(2, "pattern: %s\n", pattern);
					da_push(list, node);
				} else {
					PatternNode node = { .type = P_CHAR, .c = '[' };
					da_push(list, node);
				}
            }
        }
		pattern++;
    }
    return list;
}

typedef struct {
	const char *str;
	size_t size;
} String;

static bool match_pattern(int **dp, const String str, const PatternNodeL *pattern, size_t i, size_t j, int flag) {
	// dprintf(2, "INIT: i: %zu, j: %zu\n", i, j);
	if (dp[i][j] != -1) {
		// dprintf(2, "RETURN -1: i: %zu, j: %zu\n", i, j);
		return dp[i][j];
    }

	if (flag != 0 && j >= pattern->size) {
		return true;
	}

	if (i >= str.size && j >= pattern->size) {
		// dprintf(2, "RETURN MATCH: i: %zu, j: %zu\n", i, j);
		return true;
    }

	if (j >= pattern->size) {
		// dprintf(2, "RETURN EOP: i: %zu, j: %zu\n", i, j);
		return false;
	}

	bool match = ( 
		(i < str.size) &&
		((pattern->data[j].type == P_RANGE && pattern->data[j].map[(unsigned int)str.str[i]] == true) ||
		(pattern->data[j].type == P_CHAR && pattern->data[j].c == str.str[i]) ||
        (pattern->data[j].type == P_QMARK))
	);

	if (pattern->data[j].type == P_STAR) {
		// dprintf(2, "STAR: i: %zu, j: %zu\n", i, j);
		bool i_1 = (i < str.size && match_pattern(dp, str, pattern, i + 1, j, flag));
		bool j_1 = (match_pattern(dp, str, pattern, i, j + 1, flag));
		dp[i][j] = (i_1 || j_1);
		return dp[i][j];
	}

	if (match) {
		dp[i][j] = match_pattern(dp, str, pattern, i + 1, j + 1, flag);
		return dp[i][j];
	}

	dp[i][j] = false;
    return dp[i][j];
}

int match_string(const char *str, const PatternNodeL *pattern_nodes, int flag) {

	String string = {
		.str = str,
		.size = ft_strlen(str),
	};

    int **dp = calloc(string.size + 1, sizeof(int *));
    for (size_t i = 0; i < string.size + 1; i++) {
        dp[i] = malloc((pattern_nodes->size + 1) * sizeof(int));
		ft_memset(dp[i], -1, (pattern_nodes->size + 1) * sizeof(int));
    }

    bool match = match_pattern(dp, string, pattern_nodes, 0, 0, flag);
	size_t ret = match;

	if (flag != 0) {
		for (size_t i = 0; i < string.size; i++) {
			const size_t p_size = pattern_nodes->size - 1;

			if ((flag == SMAL_PREFIX || flag == SMAL_SUFFIX) && dp[i][p_size] == true) {
				ret = (i + 1);
				break;
			} else if ((flag == LONG_SUFFIX || flag == LONG_PREFIX) && dp[i][p_size] == true) {
				ret = (i + 1 > ret) ? (i + 1) : ret;
			}
		}
	}

    for(size_t i = 0; i < string.size + 1; i++) {
        free(dp[i]);
    }
    free(dp);

    return ret;
}
