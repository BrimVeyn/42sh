/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 12:10:41 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/03 13:14:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "regex.h"
#include <stdint.h>
#include <stdio.h>

static int matchdigit(char c){
    return (0 <= c && c <= 9);
}

static int matchalpha(char c){
    return (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'));
}

static int matchalphanum(char c){
    return matchdigit(c) || matchalpha(c);
}

static int matchwhitespace(char c){
    return (('\a' <= c && c <= '\r') || c == ' ');
}

int matchmetachar(char c, const char* str)
{
  switch (str[0])
  {
    case 'd': return  matchdigit(c);
    case 'D': return !matchdigit(c);
    case 'w': return  matchalphanum(c);
    case 'W': return !matchalphanum(c);
    case 's': return  matchwhitespace(c);
    case 'S': return !matchwhitespace(c);
    default:  return (c == str[0]);
  }
}

match_result regex_match(char *regexp, char *text) {
    match_result result = { -1, -1 };
    int text_pos = 0;

    if (regexp[0] == '^') {
        if (regex_matchhere(regexp + 1, text, &text_pos)) {
            result.start = 0;
            result.end = text_pos;
            return result;
        }
    }

    do {
        int start_text_pos = text_pos;
        if (regex_matchhere(regexp, text, &text_pos)) {
            result.start = start_text_pos;
            result.end = text_pos;
            return result;
        }
    } while (*text++ != '\0' && ++text_pos);

    return result;
}

int regex_matchstar(char c, char *regexp, char *text, int *text_pos) {
 //    printf("Regex matchstar\n");
	// printf("comparing regexp: %c\ntext: %c\n", c, *text);
	while (*text != '\0' && ((*text == c )|| c == '.')) {
		// printf("comparing regexp: %c\ntext: %c\n", c, *text);
        (*text_pos)++;
        text++;
    }

    if (*regexp == '\0') {
        return 1;
    }

    return regex_matchhere(regexp, text, text_pos);
}

int regex_is_range_match(char *regexp, char c) {
    char start = regexp[0];
    char end = regexp[2];
    return (start <= c && c <= end);
}

int regex_find_range_start(char *regexp){
    int i = 0;
    for (; regexp[i] != '['; i--){}
    return i;
}

int regex_find_range_end(char *regexp){
    int i = 0;
    for (; regexp[i] != ']'; i++){}
    return i;
}

int regex_matchrange(char *regexp, char *text, int *text_pos, int previous_found) {
    int has_star = (regexp[regex_find_range_end(regexp) + 1] == '*');
    int matched = 0;
	int has_caret = (*regexp == '^');

	if (has_caret){
		regexp++;
	}

    while(*regexp != ']') {
        // implemente litteral charactere here (&& regexp[0] != '//')
        // +2 to skip current char and -
        if (regexp[1] == '-' && (regexp += 2) && regex_is_range_match(regexp - 2, *text)){
            matched = 1;
        } else if (regexp[0] == '\\' && matchmetachar(*text, regexp + 1)){
            matched = 1;
        } else if (*regexp == *text){
            matched = 1;
        }

        if ((matched && !has_caret) || (!matched && has_caret && *regexp)){
            previous_found = 1;
            (*text_pos)++;
            text++;
            if (has_star){
                return regex_matchrange(regexp + regex_find_range_start(regexp) + 1, text, text_pos, previous_found);
            }
            return regex_matchhere(regexp + regex_find_range_end(regexp) + 1, text, text_pos);
        }

        regexp++;
    }
    
    if (has_star) {
        /*printf("Not found but there is a star\n");*/
        if (previous_found)
            return regex_matchhere(regexp + 2, text, text_pos);
        return regex_matchhere(regexp + 1, text, text_pos);
        // if text_pos
    }

    return 0;
}

int regex_matchhere(char *regexp, char *text, int *text_pos) {
	// printf("regexp char: %c\ntext char: %c\n", *regexp, *text);
    if (regexp[0] == '\0') {
        return 1;
    }
    // if (regexp[0] == '$'){
    //     return (*text == '\0');
    // }
    if (regexp[0] == '['){
        return regex_matchrange(regexp + 1, text, text_pos, 0);
    }
    if (regexp[1] == '*') {
        return regex_matchstar(*regexp, regexp + 2, text, text_pos);
    }
    if (regexp[0] == '\\' && matchmetachar(*text, regexp + 1)){
        (*text_pos)++;
        return regex_matchhere(regexp + 2, text + 1, text_pos);
    }
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text)) {
        (*text_pos)++;
        return regex_matchhere(regexp + 1, text + 1, text_pos);
    }

    /*printf("regex_matchhere return 0\n");*/
    return 0;
}

void regex_test(char *regexp, char *text){
    match_result result = regex_match(regexp, text);
    printf("start: %d\nend: %d\n", result.start, result.end);

    for (int i = 0; text[i]; i++) {
        if (i == result.start)
            printf("\033[0;32m");
        else if (i == result.end)
            printf("\033[0m");
        printf("%c", text[i]);
    }
    printf("\n");
    printf("\033[0m");

    printf("%s\n", regexp);
    printf("==================\n");
}

