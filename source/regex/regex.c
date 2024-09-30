/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 12:10:41 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/30 15:07:00 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_regex.h"
#include "utils.h"
#include "libft.h"
#include "colors.h"
#include <stdint.h>
#include <stdio.h>

void print_regex_pattern(regex_compiled_t *regexp);
void print_regex_node(regex_node_t *pattern);
const char *get_regex_type(regex_pattern_type type);
int matchhere(regex_node_t **pattern, char *string, int *end_pos);
regex_match_t regex_exec_pattern(regex_compiled_t *regexp, char *string);

void regex_append_node(regex_compiled_t *regexp, regex_node_t *node) {
	if (regexp->size >= regexp->capacity - 1) {
		regexp->capacity *= 2;
		regexp->pattern = (regex_node_t **)ft_realloc(regexp->pattern, regexp->size, regexp->capacity, sizeof(regex_node_t *));
	}
	regexp->pattern[regexp->size] = node;
	regexp->size += 1;
	regexp->pattern[regexp->size] = NULL;
}

void regex_compile_pattern(regex_compiled_t *regexp, const char *pattern){
	regexp->size = 0;
	regexp->capacity = 10;
	regexp->pattern = ft_calloc(10, sizeof(regex_node_t *));
	regexp->err = "Success";
	regexp->is_pattern_valid = true;
	
	if (pattern == NULL){
		regexp->is_pattern_valid = false;
		regexp->err = "NULL pattern string";
		return;
	}
	
	int i = 0;
	while(pattern[i] != '\0'){
		char char_in_pattern = pattern[i];
		// printf("pattern left: %s\nchar: %c\n", pattern + i, char_in_pattern);
		switch(char_in_pattern){
			case '^':
				set_begin(regexp);
				i++;
				break;
			case '$':
				set_end(regexp);
				i++;
				break;
			case '.':
				set_dot(regexp);
				i++;
				break;
			case '[':
				set_range(regexp, pattern + i + 1, &i);
				break;
			case '*':
				set_quantifier(regexp, REGEX_STAR);
				i++;
				break;
			case '?':
				set_quantifier(regexp, REGEX_QUESTION);
				i++;
				break;
			case '+':
				set_quantifier(regexp, REGEX_PLUS);
				i++;
				break;
			case '\\':
				i++;
				char_in_pattern = pattern[i];
			// fall through
			default:
				set_single_char(regexp, char_in_pattern);
				i++;
		}
	}
	set_end_of_pattern(regexp);
}


int matchchar(regex_node_t *pattern, char c){
	if (pattern->type == REGEX_DOT)
		return 1;
	if (pattern->type != REGEX_SINGLE_CHAR)
		return 0;
	return c == pattern->c;
}

int matchrange(regex_node_t *pattern, char c) {
    if (pattern->type != REGEX_RANGE && pattern->type != REGEX_INVERT_RANGE)
        return 0;
    int invert = pattern->type == REGEX_INVERT_RANGE;
    for (int i = 0; pattern->range[i][0]; i++) {
        if (pattern->range[i][1] == '-' && pattern->range[i][0] <= c && c <= pattern->range[i][2]) {
            return !invert;
        } else if (pattern->range[i][1] != '-' && c == pattern->range[i][0]) {
            return !invert;
        }
    }
    return invert;
}
const char *get_regex_type(regex_pattern_type type);

int matchquestion(regex_node_t *search, regex_node_t **pattern, char *string, int *end_pos) {
    char *saved_pos = string;
    int temp_end = *end_pos;

    if (*string != '\0' && (matchchar(search, *string) || matchrange(search, *string))) {
        string++;
        temp_end++;
        if (matchhere(pattern, string, &temp_end)) {
            *end_pos = temp_end;
            return 1;
        }
        string = saved_pos;
    }

    return matchhere(pattern, string, end_pos);
}


int matchplus(regex_node_t *search, regex_node_t **pattern, char *string, int *end_pos) {
    char *saved_pos = string;
    int matched = 0;

    while (*string != '\0' && (matchchar(search, *string) || matchrange(search, *string))) {
        string++;
        (*end_pos)++;
        matched = 1;
    }

    if (!matched) return 0;

    while (string > saved_pos) {
        if (matchhere(pattern, string, end_pos))
            return 1;
        string--;
        (*end_pos)--;
    }

    return 0;
}

int matchstar(regex_node_t *search, regex_node_t **pattern, char *string, int *end_pos) {
    char *saved_pos = string;
    while (*string != '\0' && (matchchar(search, *string) || matchrange(search, *string))) {
        string++;
        (*end_pos)++;
    }
	
    while (string >= saved_pos) {
        if (matchhere(pattern, string, end_pos))
            return 1;
        string--;
        (*end_pos)--;
    }
    return 0;
}

int matchhere(regex_node_t **pattern, char *string, int *end_pos) {
    // print_regex_node(*pattern);
    if ((*pattern)->type == REGEX_END_OF_PATTERN)
        return 1;
    if ((*pattern)->quantifier == REGEX_PLUS)
		return matchplus(*pattern, pattern + 1, string, end_pos);
    if ((*pattern)->quantifier == REGEX_QUESTION)
		return matchquestion(*pattern, pattern + 1, string, end_pos);
    if ((*pattern)->quantifier == REGEX_STAR)
		return matchstar(*pattern, pattern + 1, string, end_pos);
    if ((*pattern)->type == REGEX_END && pattern[1]->type == REGEX_END_OF_PATTERN)
        return *string == '\0';
    if (*string != '\0' && (matchchar(*pattern, *string) || matchrange(*pattern, *string))) {
        (*end_pos)++;
        return matchhere(pattern + 1, string + 1, end_pos);
    }
    return 0;
}

void regex_free_pattern(regex_compiled_t *regexp){
	for (int i = 0; regexp->pattern[i]; i++){
		free(regexp->pattern[i]);
	}
	free(regexp->pattern);
}

regex_match_t regex_match(const char *pattern, char *string){
	regex_compiled_t regexp;
	regex_compile_pattern(&regexp, pattern);
	regex_match_t result = regex_exec_pattern(&regexp, string);
	regex_free_pattern(&regexp);
	return result;
}

regex_match_t regex_exec_pattern(regex_compiled_t *regexp, char *string){
	regex_match_t match;

	match.re_start = 0;
	match.re_end = 0;
	match.is_found = false;
	
	regex_node_t **pattern = regexp->pattern;

	if (regexp->pattern[0]->type == REGEX_BEGIN){
		if (matchhere(pattern+1, string, &match.re_end))
			match.is_found = true;
		else{
			match.re_end = -1;
			match.re_start = -1;
		}
		return match;
	}
	do {
		if (matchhere(pattern, string, &match.re_end)){
			match.is_found = true;
			return match;
		}
		match.re_start++;
		match.re_end = match.re_start;
	} while (*string++ != '\0');
	match.re_end = -1;
	match.re_start = -1;
	return match;
}

const char *get_quantifier_type(regex_quantifier_type type) {
	switch(type) {
		case REGEX_STAR:
			return "*";
		case REGEX_QUESTION:
			return "?";
		case REGEX_PLUS:
			return "+";
		default:
			return "??????";
	}
}

const char *get_regex_type(regex_pattern_type type) {
	switch(type) {
		case REGEX_END_OF_PATTERN:
			return "END OF PATTERN";
		case REGEX_DOT:
			return ".";
		case REGEX_BEGIN:
			return "^";
		case REGEX_END:
			return "$";
		case REGEX_RANGE:
			return "[]";
		case REGEX_INVERT_RANGE:
			return "[^]";
		case REGEX_SINGLE_CHAR:
			return "char";
		default:
			return "??????";
	}
}

void print_regex_node(regex_node_t *pattern){
	int type = pattern->type;
	printf("==========\ntype: %s\n", get_regex_type(type));
	if (type == REGEX_SINGLE_CHAR){
		printf("	char: '%c'\n", pattern->c);
		
	}
	if (type == REGEX_RANGE){
		for (int j = 0; pattern->range[j][0] != '\0'; j++){
			printf("	range pattern : [%s]\n", pattern->range[j]);
		}
	}
	if (pattern->quantifier != REGEX_NO_QUANTIFIER){
		printf("quantified by %s\n", get_quantifier_type(pattern->quantifier));
	}
}

void print_regex_pattern(regex_compiled_t *regexp){

	for (int i = 0; i < regexp->size; i++){
		print_regex_node(regexp->pattern[i]);
	}
}

#include <ft_regex.h>

void regex_test(char *pattern, char *text) {
	// regex_compiled_t regexp;
	// regex_compile_pattern(&regexp, pattern);
    regex_match_t my_result = regex_match(pattern, text);

    regex_t regex;
    regmatch_t matches[1];
    int result = regcomp(&regex, pattern, REG_EXTENDED);
    if (result != 0) {
        char errbuf[128];
        regerror(result, &regex, errbuf, sizeof(errbuf));
        printf("Erreur de compilation du regex POSIX: %s\n", errbuf);
        return;
    }

    result = regexec(&regex, text, 1, matches, 0);
    int posix_re_start = -1, posix_re_end = -1;
    if (result == 0) {
        posix_re_start = matches[0].rm_so;
        posix_re_end = matches[0].rm_eo;
    } else if (result == REG_NOMATCH) {
        posix_re_start = posix_re_end = -1;
    }
	printf("My regex:    re_start = %d, re_end = %d\n", my_result.re_start, my_result.re_end);
	for (int i = 0; text[i]; i++) {
		if (i == my_result.re_start)
			printf("\033[0;32m");
		else if (i == my_result.re_end)
			printf("\033[0m");
		printf("%c", text[i]);
	}
	printf("\033[0m\n");
	
    if (my_result.re_start != posix_re_start || my_result.re_end != posix_re_end) {
		printf("pattern: %s\n", pattern);
        printf("POSIX regex: re_start = %d, re_end = %d\n", posix_re_start, posix_re_end);


        printf("POSIX regex:\n");
        for (int i = 0; text[i]; i++) {
            if (i == posix_re_start)
                printf("\033[0;31m");
            else if (i == posix_re_end)
                printf("\033[0m");
            printf("%c", text[i]);
        }
        printf("\033[0m\n");
    } else {
        printf("✅  %s%s%s\n", C_LIGHT_GRAY, pattern, C_RESET);
    }
    regfree(&regex);
    printf("==================\n");
}
