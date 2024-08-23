/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 12:10:41 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/23 10:29:16 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "regex.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>

int regex_matchhere(char *regexp, char *text, int *text_pos);

typedef struct {
    int start;
    int end;
} match_result;

match_result regex_match(char *regexp, char *text) {
    char *old_text = text;
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
    while (*text != '\0' && ((*text == c )|| c == '.')) {
        (*text_pos)++;
        text++;
    }

    if (*regexp == '\0') {
        return 1;
    }

    return regex_matchhere(regexp, text, text_pos);
}

int regex_is_range_match(char **regexp, char c){
    char start = (*regexp)[0];
    char end = (*regexp)[2];
    *regexp += 3;
    return (start <= c && c <= end);
}

int regex_is_range_end(char *regexp){
    int i = 0;
    for (; regexp[i] != ']'; i++){}
    return i + 1;
}

int regex_is_range_start(char *regexp){
    int i = 0;
    for (; regexp[i] != '['; i--){}
    return i;
}

int regex_matchrange(char *regexp, char *text, int *text_pos){
    int has_star = (regexp[regex_is_range_end(regexp)] == '*');

    while(*regexp != ']' && *regexp){
        if (regexp[1] == '-' && regex_is_range_match(&regexp, *text)){
            (*text_pos)++;
            if (has_star){
                return regex_matchrange(regexp + regex_is_range_start(regexp), text + 1, text_pos);
            }
            return regex_matchhere(regexp + regex_is_range_end(regexp), text + 1, text_pos);
        }
        if (regexp[0] == *text){
            (*text_pos)++;
            if (has_star){
                return regex_matchrange(regexp + regex_is_range_start(regexp), text + 1, text_pos);
            }
            return regex_matchhere(regexp + regex_is_range_end(regexp), text + 1, text_pos);
        }
        regexp++;
    }
    (*text_pos)++;
    if (has_star){
        return regex_matchhere(regexp + 1, text + 1, text_pos);
    }
    return 0;
}

int regex_matchhere(char *regexp, char *text, int *text_pos) {
    if (regexp[0] == '\0') {
        return 1;
    }
    if (regexp[0] == '['){
        return regex_matchrange(regexp + 1, text, text_pos);
    }
    if (regexp[1] == '*') {
        return regex_matchstar(*regexp, regexp + 2, text, text_pos);
    }
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text)) {
        (*text_pos)++;
        return regex_matchhere(regexp + 1, text + 1, text_pos);
    }
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

    printf("%s\n", regexp);
    printf("==================\n");
}

int main(void) {
    char *text = "Heeello world!";
    regex_test("Heeello", text);
    regex_test("Hedllo", text);
    regex_test("Hi*", text);
    regex_test("He*", text);
    regex_test("He*l", text);
    regex_test("ee", text);
    regex_test("^ee", text);
    regex_test("^He", text);
    regex_test("H[A-Z]", text);
    regex_test("H[a-z]", text);
    regex_test("Heee[a-z]", text);
    regex_test("H[a-z]*", text);
    printf("Salut\n");

    return 0;
}

/*
TO DO
Checker pourquoi repetition quand range fail
reduire taille de la fonction range
*/
