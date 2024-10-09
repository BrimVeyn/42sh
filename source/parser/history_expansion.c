/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history_expansion.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/03 12:28:06 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/09 14:32:31 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "42sh.h"
#include "regex.h"
#include <stdio.h>
#include <sys/mman.h>

int get_history(void) {
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
		fd = open(history_filename, O_APPEND | O_RDWR | O_CREAT, 0644);
		return fd;
    }

	char *start = buffer;
    char *end = buffer; 

	while(*end){
		if (*end == '\n'){
			char *tmp = (char *)malloc(end - start + 1);
			memcpy(tmp, start, end - start);
			tmp[end - start] = '\0';
			// add_history(tmp);
			free(tmp);
			start = end + 1;
		}
		end++;
	}
	munmap(buffer, file_size);
	close(fd);
	fd = open(history_filename, O_APPEND | O_RDWR | O_CREAT, 0644);
	return fd;
}

void add_input_to_history(char *input, int *history_fd){
	// add_history(input);
	char *home = getenv("HOME");
	char history_filename[1024] = {0};
	ft_sprintf(history_filename, "%s/.42sh_history", home);
	// if (*history_fd == -1){
	// 	*history_fd = open(history_filename, O_APPEND | O_WRONLY | O_CREAT, 0644);
	// 	if (*history_fd == -1) {
	// 		perror("Can't open history file");
	// 		exit(EXIT_FAILURE);
	// 	}
	// }
	dprintf(*history_fd, "%s\n", input);
}

char *get_value_wd(char *parameter, char *buffer, size_t file_size){
    char *value = NULL;
    size_t start = 0;
    size_t end = 0;

    for (size_t i = file_size; i > 0; i--) {
        if (buffer[i] == '\n') {
            start = i + 1;
            end = file_size;
            
            char *tmp_cmd = ft_substr(buffer, start, end - start);

            if (!ft_strncmp(tmp_cmd, parameter, ft_strlen(parameter))) {
                value = tmp_cmd;
                return value; 
            }
            free(tmp_cmd);
        }
    }

    return value;
}

char *get_value_nb(char *parameter, char *buffer, size_t file_size){
    char *value = NULL;
    bool reverse = (parameter[0] == '-');
    int i = reverse;

    for (; parameter[i] && ft_isdigit(parameter[i]); i++){}

    char *tmp_nbr = ft_substr(parameter, 0, i);  
    int nbr = ft_atoi(tmp_nbr);
    free(tmp_nbr);

    int cpt = 0;
    int tmp = (reverse == true) ? file_size - 1: 0;
    size_t start = 0;
    size_t end = 0;

    if (reverse) {
        for (int i = file_size - 2; i >= 0; i--) {
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

bool history_expansion (char **pstring, int history_fd){

	regex_match_t result = regex_match("[^\\\\]?\\![\\-!a-zA-Z0-9]*", *pstring);
	size_t file_size = 0;
	char *buffer = NULL;

	if (result.is_found){
		struct stat st;
		if (fstat(history_fd, &st) == -1){
			perror("Can't get history's file stats");
			return false;
		}
		file_size = st.st_size;

		buffer = mmap(NULL, file_size, PROT_READ | PROT_WRITE, MAP_SHARED, history_fd, 0);
		if (buffer == MAP_FAILED) {
			char *parameter = ft_substr(*pstring, result.re_start + 1, result.re_end);
			dprintf(2, "event not found: !%s\n", parameter);
			free(parameter);
			return false;
		}

		do {
			result = regex_match("[^\\\\]?\\![\\-!a-zA-Z0-9]*", *pstring);
			if (!result.is_found){
				break;
			}
			char *parameter = ft_substr(*pstring, result.re_start + 1, result.re_end);
			char *value = NULL;
			if (parameter[0] == '!'){
				for (int i = file_size - 2; i >= 0; i--){
					if (buffer[i] == '\n'){
						value = ft_substr(buffer, i + 1, file_size - i - 2);
						break;
					}
				}
			} else if (ft_isalpha(parameter[0])){
				value = get_value_wd(parameter, buffer, file_size);
			} else if (ft_isdigit(parameter[0]) || (parameter[0] == '-' && ft_isdigit(parameter[1]))){
				value = get_value_nb(parameter, buffer, file_size);
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
	}
	return true;
}
