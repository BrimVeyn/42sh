/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:40:48 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/27 14:55:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "lexer.h"
#include "utils.h"
#include "libft.h"
#include "ft_readline.h"
#include "ft_regex.h"
#include <sys/time.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

typedef enum {
	FC_E = 0b00001,
	FC_L = 0b00010,
	FC_N = 0b00100,
	FC_R = 0b01000,
	FC_S = 0b10000,
} fc_options;

typedef enum {
    VALID,
	ERROR_HISTORY_EMPTY,
	ERROR_HISTSIZE_0,
	ERROR_INVALID_OPTION,
} fcStatus;

int fc_status = VALID;

void print_fc_error(const fcStatus n, const void ** const arg){
	ft_dprintf(2, "42sh: fc: ");
	switch (n) {
		case ERROR_INVALID_OPTION: {
			char option_str[2] = {(char)(*((int *)arg)), '\0'};
			ft_dprintf(2, "-%s: Invalid option\n", option_str);
			break;
		}
		case ERROR_HISTSIZE_0:
			ft_dprintf(2, "Histsize=0 dumbass\n");
			break;
		case ERROR_HISTORY_EMPTY:
			ft_dprintf(2, "History is empty\n");
			break;
		default:
			break;
	}
}

static int min(const int n1, const int n2){
	return (n1 < n2) ? n1 : n2;
}
static int max(const int n1, const int n2){
	return (n1 > n2) ? n1 : n2;
}

static char *fc_default(void){
	return ft_strdup(history->entries[history->length - CURRENT_LINE]->line.data);
}

char *get_history_value_wd(char *word){
	int word_len = ft_strlen(word);
	for (int i = history->length - 1; i >= 0; i--) {
		char *current_cmd = history->entries[i]->line.data;
		if (!ft_strncmp(current_cmd, word, min(word_len, ft_strlen(current_cmd)))) {
			//do not free that
			return current_cmd; 
		}
	}
    return NULL;
}

char* get_history_value_nb(int first, int last) {
	if (first < 0){
		first = history->length + first;
		if (first < 0) first = 1;
	}
	if (last < 0){
		last = history->length + last;
		if (last < 0) last = 1;
	}

	if (first == 0) first = 1;
	if (last == 0) last = 1;

    if (max(first, last) > history->length) {
        return fc_default();
    }

    char *commands = ft_strdup("");
    if (!commands) return NULL;
	
	//TODO: gerer si first > last
	int inc = (first > last) ? -1 : 1;
    for (int i = first; (first > last) ? (i >= last ): (i <= last); i += inc) {
        const char *line = history->entries[i - 1]->line.data;
        if (!line) continue;

        int new_len = ft_strlen(commands) + ft_strlen(line) + 2; // +1 pour '\n', +1 pour '\0'
        char *tmp = ft_calloc(new_len, sizeof(char));
        if (!tmp) {
            free(commands);
            return NULL; 
        }

        ft_sprintf(tmp, "%s%s\n", commands, line);
        free(commands);
        commands = tmp;
    }

    gc(GC_ADD, commands, GC_SUBSHELL);
    return commands;
}

int get_fc_options(char **args, int *options, StringListL *operands) {
	bool accept_options = true;
	for (int i = 1; args[i]; i++){
		if (accept_options && *args[i] == '-'){
			for (int j = 1; accept_options && args[i][j]; j++){
				switch(args[i][j]){
					case 'e': *options |= FC_E; break;
					case 'l': *options |= FC_L; break;
					case 'n': *options |= FC_N; break;
					case 'r': *options |= FC_R; break;
					case 's': *options |= FC_S; break;
					default: {
						if (j == 1 && ft_isdigit(args[i][j])){ // accept negative numbers
							accept_options = false;
							break;
						}
						fc_status = ERROR_INVALID_OPTION;
						*options = args[i][j];
						return -1;
					}
				}
			}
		}
		else {
			accept_options = false;
			da_push(operands, args[i]);
		}
	}
	return 0;
}

void builtin_fc(const SimpleCommandP *command, Vars *shell_vars) {
	char *c_histsize = string_list_get_value(shell_vars->set, "HISTSIZE");
	int histsize = -1;
	fc_status = VALID;
	if (c_histsize && regex_match("[^0-9]", c_histsize).is_found == false){
		histsize = ft_atoi(c_histsize);
	}

	if (histsize == 0){
		print_fc_error(ERROR_HISTSIZE_0, NULL);
		g_exitno = 1;
		return;
	}

	int options = 0;
	da_create(operands, StringListL, sizeof(char *), GC_SUBSHELL);
	if (get_fc_options(command->word_list->data, &options, operands) == -1){
		print_fc_error(fc_status, (const void **)&options);
	}
	
	char *command_list = NULL;
	if (operands->size == 0){
		command_list = fc_default();
	} else if (ft_isdigit((int) **operands->data)){
		if (operands->size == 1){
			command_list = get_history_value_nb(ft_atoi(*operands->data), ft_atoi(*operands->data));
		} else {
			command_list = get_history_value_nb(ft_atoi(*operands->data), ft_atoi(operands->data[1]));
		}
	} else if (ft_isalpha((int) **operands->data)){
		command_list = get_history_value_wd(*operands->data);
	}
	
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1){
		//print error
		g_exitno = 1;
		return ;
	}
	char filename[21] = {0}; //7 digit max for usec
	ft_sprintf(filename, "/tmp/42sh-fn.%ld", time.tv_usec);
	// printf("command_list: \n%s\n", command_list);
	// printf("filname: %s\n", filename);

	int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
	if (fd == -1){
		//print error
		g_exitno = 1;
		return;
	}
	write(fd, command_list, ft_strlen(command_list));

	
	char *input = gc(GC_ADD, ft_strjoin("${EDITOR} ", filename), GC_SUBSHELL);
	parse_input(input, NULL, shell_vars);
	lseek(fd, 0, SEEK_SET);
	char *buffer = read_whole_file(fd);
	parse_input(buffer, NULL, shell_vars);

	return ;
}

