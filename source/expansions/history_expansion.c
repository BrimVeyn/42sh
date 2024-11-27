/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_expansion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/16 10:17:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/27 10:36:51 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"
#include "ft_regex.h"
#include "ft_readline.h"

#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

void get_history(Vars *shell_vars) {
	char *home = getenv("HOME");
	char history_filename[1024] = {0};
	ft_sprintf(history_filename, "%s/.42sh_history", home);
    int fd = open(history_filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("Can't open history file");
        exit(EXIT_FAILURE);
    }

	struct stat st;
	if (fstat(fd, &st) == -1){
        perror("Can't get history's file stats");
        exit(EXIT_FAILURE);
	}
    size_t file_size = st.st_size;

    char *buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
		close(fd);
		return;
    }

	char *start = buffer;
    char *end = buffer; 

	while(*end){
		if (*end == '\n'){
			char *tmp = (char *)malloc(end - start + 1);
			memcpy(tmp, start, end - start);
			tmp[end - start] = '\0';
			add_history(tmp, shell_vars);
			free(tmp);
			start = end + 1;
		}
		end++;
	}
	munmap(buffer, file_size);
	close(fd);
}

void add_input_to_history(char *input, Vars *shell_vars){
	add_history(input, shell_vars);
	// char *home = getenv("HOME");
	// char history_filename[1024] = {0};
	// ft_sprintf(history_filename, "%s/.42sh_history", home);
	// dprintf(*history_fd, "%s\n", input);
}

char *get_value_wd(char *parameter, char *buffer, size_t history_length){
    char *value = NULL;
    size_t start = 0;
    size_t end = 0;

    for (size_t i = history_length; i > 0; i--) {
        if (buffer[i] == '\n') {
            start = i + 1;
            end = history_length;
            
            char *tmp_cmd = ft_substr(buffer, start, end - start);
			// printf("tmp_cmd: %s parameter: %s", tmp_cmd, parameter);
            if (!ft_strncmp(tmp_cmd, parameter, ft_strlen(parameter))) {
                value = tmp_cmd;
				printf("return\n");
                return value; 
            }
            free(tmp_cmd);
        }
    }
	printf("return null\n");
    return value;
}

//TODO:search in struct history

char *get_value_nb(char *parameter, char *buffer, size_t history_length){
    char *value = NULL;
    bool reverse = (parameter[0] == '-');
    int i = reverse;

    for (; parameter[i] && ft_isdigit(parameter[i]); i++){}

    char *tmp_nbr = ft_substr(parameter, 0, i);  
    int nbr = ft_atoi(tmp_nbr);
    free(tmp_nbr);

    int cpt = 0;
    int tmp = (reverse == true) ? history_length - 1: 0;
    size_t start = 0;
    size_t end = 0;

    if (reverse) {
        for (int i = history_length - 2; i >= 0; i--) {
            if (buffer[i] == '\n' || i == 0) {
				cpt++;
                if (cpt == -nbr) {
                    start = (i == 0) ? 0 : i + 1;
                    end = tmp;
                    value = ft_substr(buffer, start, end - start);
                    break;
                }
                tmp = i;
            }
        }
	} else {
		for (size_t i = 0; buffer[i]; i++) {
			if (buffer[i] == '\n'){
				cpt++;
				if (cpt == nbr){
					start = tmp;
					end = i;
					value = ft_substr(buffer, start, end - start);  
					break;  
				}
				tmp = i + 1;  
			}
		}
	}
    return value;
}

char *stringify_history(HISTORY_STATE *history, int start, int end) {
    size_t capacity = 10;
    size_t size = 0;

    char *buffer = gc(GC_CALLOC, capacity, sizeof(char), GC_SUBSHELL);
    if (!buffer)
        return NULL;

    for (int i = start; i < end; i++) {
        char *current = history->entries[i]->line.data;
        size_t current_len = ft_strlen(current);

        if (size + current_len >= capacity) {
            size_t new_capacity = (size + current_len) * 2;
            buffer = gc(GC_REALLOC, buffer, capacity, new_capacity, sizeof(char), GC_SUBSHELL);
            if (!buffer)
                return NULL;
			capacity = new_capacity;
        }
		ft_strlcat(buffer, "\n", capacity);
        ft_strlcat(buffer, current, capacity);
        size += current_len;
    }

    return buffer;
}

bool history_expansion (char **pstring){
	regex_match_t result = regex_match("[^\\\\]?\\![\\-!a-zA-Z0-9]+", *pstring);
	char *buffer = NULL;

	if (result.is_found){
		buffer = stringify_history(history, 0, history->length - 1);
		if (!buffer){
			char *parameter = ft_substr(*pstring, result.re_start + 1, result.re_end);
			dprintf(2, "event not found: !%s\n", parameter);
			free(parameter);
			return false;
		}

		do {
			result = regex_match("[^\\\\]?\\![\\-!a-zA-Z0-9]+", *pstring);
			if (!result.is_found){
				break;
			}
			char *parameter = ft_substr(*pstring, result.re_start + 1, result.re_end);
			char *value = NULL;
			if (parameter[0] == '!'){
				for (int i = history->length - 2; i >= 0; i--){
					if (buffer[i] == '\n'){
						value = ft_substr(buffer, i + 1, history->length - i - 2);
						break;
					}
				}
			} else if (ft_isalpha(parameter[0])){
				value = get_value_wd(parameter, buffer, history->length);
			} else if (ft_isdigit(parameter[0]) || (parameter[0] == '-' && ft_isdigit(parameter[1]))){
				value = get_value_nb(parameter, buffer, history->length);
			}

			if (value){
				char *start = (result.re_start == 0) ? ft_strdup("") : ft_substr(*pstring, 0, result.re_start - 1);
				char *end = (result.re_end == (int)ft_strlen(*pstring)) ? ft_strdup("") : ft_substr(*pstring, result.re_end, ft_strlen(*pstring) - result.re_end);
				char *new_cmd = malloc(ft_strlen(start) + ft_strlen(value) + ft_strlen(end) + 1);
				sprintf(new_cmd, "%s%s%s", start, value, end);
				*pstring = new_cmd;
			} else {
				dprintf(2, "event not found: !%s\n", parameter);
				return false;
			}
			free(parameter);
		} while(true);

		gc(GC_FREE, buffer, GC_SUBSHELL);
	}
	return true;
}
