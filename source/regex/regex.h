/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 15:38:14 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/24 15:38:38 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REGEX_H
#define REGEX_H

#include <stdbool.h>
#include <stdint.h>

typedef enum{
	REGEX_NO_QUANTIFIER,
	REGEX_PLUS,
	REGEX_STAR,
} regex_quantifier_type;

typedef enum{
	REGEX_END_OF_PATTERN,
	REGEX_SINGLE_CHAR,
	REGEX_DOT,
	REGEX_RANGE,
	REGEX_BEGIN,
	REGEX_END,
}regex_pattern_type;

typedef struct {
	uint16_t re_start;
	uint16_t re_end;
} regex_match_t;

typedef struct {
	regex_pattern_type type;
	union {
		char c;
		char range[256][4];
	};
	regex_quantifier_type quantifier;
} regex_node_t;

typedef struct {
	regex_node_t **pattern;
	int size;
	int capacity;
	char *err;
	bool is_pattern_valid;
} regex_compiled_t;

void regex_test(char *regexp, char *text);
void regex_append_node(regex_compiled_t *regexp, regex_node_t *node);
void set_single_char(regex_compiled_t *regexp, char c);
void set_quantifier(regex_compiled_t *regexp, regex_quantifier_type quantifier);
void set_end_of_pattern(regex_compiled_t *regexp);
void set_dot(regex_compiled_t *regexp);
void set_begin(regex_compiled_t *regexp);
void set_range(regex_compiled_t *regexp, char *pattern, int *index);
void set_end(regex_compiled_t *regexp);

#endif
