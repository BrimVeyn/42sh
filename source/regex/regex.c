/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 12:10:41 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/24 16:32:35 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "regex.h"
#include "../../include/42sh.h"
#include <stdint.h>
#include <stdio.h>

const char *get_regex_type(regex_pattern_type type);

void regex_append_node(regex_compiled_t *regexp, regex_node_t *node) {
	if (regexp->size >= regexp->capacity - 1) {
		regexp->capacity *= 2;
		regexp->pattern = (regex_node_t **)ft_realloc(regexp->pattern, regexp->size, regexp->capacity, sizeof(regex_node_t *));
	}
	regexp->pattern[regexp->size] = node;
	regexp->size += 1;
	regexp->pattern[regexp->size] = NULL;
}

void regex_compile_pattern(regex_compiled_t *regexp, char *pattern){
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
			default:
				set_single_char(regexp, char_in_pattern);
				i++;
		}
	}
	set_end_of_pattern(regexp);
}

int regex_exec_pattern(regex_compiled_t *regexp, char *string, regex_match_t *pmatch){
	pmatch->re_start = -1;
	pmatch->re_end = -1;

	int i = 0;
	while(true){
		regex_node_t *node = regexp->pattern[i];
		switch(node->type){
			case REGEX_SINGLE_CHAR:
				
				break;
			default:
				;
		}
	}
}

const char *get_quantifier_type(regex_quantifier_type type) {
	switch(type) {
		case REGEX_STAR:
			return "*";
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
		case REGEX_SINGLE_CHAR:
			return "char";
		default:
			return "??????";
	}
}

void print_regex_pattern(regex_compiled_t *regexp){

	for (int i = 0; i < regexp->size; i++){
		int type = regexp->pattern[i]->type;
		printf("==========\ntype: %s\n", get_regex_type(type));
		if (type == REGEX_SINGLE_CHAR){
			printf("	char: '%c'\n", regexp->pattern[i]->c);
			
		}
		if (type == REGEX_RANGE){
			for (int j = 0; regexp->pattern[i]->range[j][0] != '\0'; j++){
				printf("	range pattern : [%s]\n", regexp->pattern[i]->range[j]);
			}
		}
		if (regexp->pattern[i]->quantifier != REGEX_NO_QUANTIFIER){
			printf("quatified by %s\n", get_quantifier_type(regexp->pattern[i]->quantifier));
		}
	}
}

int main (void){
	regex_compiled_t regexp;
	regex_compile_pattern(&regexp, "^S[a-z\\-]*.*$");
	print_regex_pattern(&regexp);

	return 0;
}

#include <regex.h>
// void regex_test(char *regexp, char *text) {
//     match_result my_result = regex_match(regexp, text);
//
//     regex_t regex;
//     regmatch_t matches[1];
//     int result = regcomp(&regex, regexp, REG_EXTENDED);
//     if (result != 0) {
//         char errbuf[128];
//         regerror(result, &regex, errbuf, sizeof(errbuf));
//         printf("Erreur de compilation du regex POSIX: %s\n", errbuf);
//         return;
//     }
//
//     result = regexec(&regex, text, 1, matches, 0);
//     int posix_start = -1, posix_end = -1;
//     if (result == 0) {
//         posix_start = matches[0].rm_so;
//         posix_end = matches[0].rm_eo;
//     } else if (result == REG_NOMATCH) {
//         posix_start = posix_end = -1;
//     }
//
//     if (my_result.start != posix_start || my_result.end != posix_end) {
//
//         printf("My regex: start = %d, end = %d\n", my_result.start, my_result.end);
//         printf("POSIX regex: start = %d, end = %d\n", posix_start, posix_end);
//
//         printf("My regex:\n");
//         for (int i = 0; text[i]; i++) {
//             if (i == my_result.start)
//                 printf("\033[0;32m");
//             else if (i == my_result.end)
//                 printf("\033[0m");
//             printf("%c", text[i]);
//         }
//         printf("\033[0m\n");
//
//         printf("POSIX regex:\n");
//         for (int i = 0; text[i]; i++) {
//             if (i == posix_start)
//                 printf("\033[0;31m");
//             else if (i == posix_end)
//                 printf("\033[0m");
//             printf("%c", text[i]);
//         }
//         printf("\033[0m\n");
//     } else {
//         printf("Valid\n");
//     }
//
//     regfree(&regex);
//     printf("==================\n");
// }
