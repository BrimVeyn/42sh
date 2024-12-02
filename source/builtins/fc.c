/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fc.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:40:48 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/02 14:23:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "lexer.h"
#include "utils.h"
#include "libft.h"
#include "ft_readline.h"
#include "ft_regex.h"
#include <stdio.h>
#include <sys/time.h>
#include <linux/limits.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

//TODO: flag -R

typedef enum {
	FC_E = 0b00001,
	FC_L = 0b00010,
	FC_N = 0b00100,
	FC_R = 0b01000,
	FC_S = 0b10000,
} FcFlags;

typedef enum {
    VALID,
	ERROR_HISTORY_EMPTY,
	ERROR_HISTSIZE_0,
	ERROR_INVALID_OPTION,
	ERROR_FC_FATAL,
	ERROR_COMMAND_NOT_FOUND,
	ERROR_OPTION_E_REQUIRES_ARGUMENT,
} FcStatus;

typedef struct {
	int flags;
	char *old;
	char *new;
	char *editor;
	char *first;
	char *last;
} FcOptions;

static int fc_status = VALID;

#define COLOR_RESET   "\033[0m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_RED     "\033[31m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_BOLD    "\033[1m"

void print_options(const FcOptions *options) {
    if (!options) {
        printf(COLOR_RED "Error: Options structure is NULL" COLOR_RESET "\n");
        return;
    }

    printf(COLOR_BOLD COLOR_CYAN "=== FcOptions ===" COLOR_RESET "\n");

    printf(COLOR_BOLD "Flags: " COLOR_RESET);
    if (options->flags & FC_E) printf(COLOR_GREEN "FC_E " COLOR_RESET);
    if (options->flags & FC_L) printf(COLOR_YELLOW "FC_L " COLOR_RESET);
    if (options->flags & FC_N) printf(COLOR_BLUE "FC_N " COLOR_RESET);
    if (options->flags & FC_R) printf(COLOR_MAGENTA "FC_R " COLOR_RESET);
    if (options->flags & FC_S) printf(COLOR_CYAN "FC_S " COLOR_RESET);
    if (options->flags == 0) printf(COLOR_RED "None" COLOR_RESET);
    printf("\n");

    printf(COLOR_BOLD "Old command: " COLOR_RESET "%s\n", options->old ? options->old : COLOR_RED "(null)" COLOR_RESET);
    printf(COLOR_BOLD "New command: " COLOR_RESET "%s\n", options->new ? options->new : COLOR_RED "(null)" COLOR_RESET);
    printf(COLOR_BOLD "Editor: " COLOR_RESET "%s\n", options->editor ? options->editor : COLOR_RED "(null)" COLOR_RESET);
    printf(COLOR_BOLD "First: " COLOR_RESET "%s\n", options->first ? options->first : COLOR_RED "(null)" COLOR_RESET);
    printf(COLOR_BOLD "Last: " COLOR_RESET "%s\n", options->last ? options->last : COLOR_RED "(null)" COLOR_RESET);

    printf(COLOR_BOLD COLOR_CYAN "=================" COLOR_RESET "\n");
}

void print_fc_usage(){
	ft_dprintf(2, "fc: usage: fc [-e ename] [-lnr] [first] [last] or fc -s [old=new] [command]\n");
}

void print_fc_error(const FcStatus n, const void ** const arg){
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
		case ERROR_FC_FATAL:
			ft_dprintf(2, "FATAL\n");
			break;
		case ERROR_COMMAND_NOT_FOUND:
			ft_dprintf(2, "no command found\n");
			break;
		case ERROR_OPTION_E_REQUIRES_ARGUMENT:
			ft_dprintf(2, "-e: option requires an argument\n");
			print_fc_usage();
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

static char *fc_default(){
	return ft_strdup(history->entries[history->length - CURRENT_LINE]->line.data);
}

int cmp_both(const char *const first, const char *const end, const char *const current_cmd) {
	size_t first_len = ft_strlen(first);
	size_t end_len = ft_strlen(end);
	if (!ft_strncmp(current_cmd, first, min(first_len, ft_strlen(current_cmd)))) {
		return 2;
	}
	if (!ft_strncmp(current_cmd, end, min(end_len, ft_strlen(current_cmd)))) {
		return 1;
	}
	return -1;
}

StringListL *get_history_value_wd(StringListL *command_list, char *first, char *end){
	int first_len = ft_strlen(first);
	int end_len = ft_strlen(end);
	for (int i = history->length - 1; i >= 0; i--) {
		char * current_cmd = history->entries[i]->line.data;
		int match = cmp_both(first, end, current_cmd);
		if (match != -1) {
			const char * const to_cmp = (match == 1) ? first : end;
			const size_t to_cmp_len = (match == 1) ? first_len : end_len;
			do {
				current_cmd = history->entries[i]->line.data;
				
				if (match == 2) {
					da_push(command_list, current_cmd);
				} else {
					da_push_front(command_list, current_cmd);
				}

				i--;
			} while(i >= 0 && ft_strncmp(current_cmd, to_cmp, min(to_cmp_len, ft_strlen(current_cmd))));
			break;
		}
	}
	return command_list;
}

void get_history_value_nb(StringListL *command_list, int first, int last) {
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
		da_push(command_list, fc_default());
        return;
    }

	
	//TODO: gerer si first > last
	int inc = (first > last) ? -1 : 1;
    for (int i = first; (first > last) ? (i >= last ): (i <= last); i += inc) {
        char *line = history->entries[i - 1]->line.data;
        if (!line) continue;

		da_push(command_list, line);
    }

    return;
}

int get_fc_options(char **args, FcOptions *options) {
	bool accept_options = true;
	for (int i = 1; args[i]; i++){
		if (accept_options && *args[i] == '-'){
			for (int j = 1; args[i][j]; j++){
				char current_char = args[i][j];
				if (current_char == 'e'){
					options->flags |= FC_E;
					if (args[i + 1] == NULL){
						print_fc_error(ERROR_OPTION_E_REQUIRES_ARGUMENT, NULL);
					}
					options->editor = args[++i];
					//si NULL segfault
					break;
				} else if (current_char == 'l'){
					options->flags |= FC_L;
				} else if (current_char == 'n'){
					options->flags |= FC_N;
				} else if (current_char == 'r'){
					options->flags |= FC_R;
				} else if (current_char == 's'){
					options->flags |= FC_S;
					if (args[i + 1]){
						char *arg = args[i + 1];
						char *ppos = ft_strchr(arg, '=');
						if (ppos){
							options->old = ft_substr(arg, 0, ppos - arg);
							options->new = ft_strdup(ppos + 1);
							i++;
							break;
						}
					}
					// printf("%s\n", ft_strchr(args[++i], '='));
					
				} else {
					if (j == 1 && ft_isdigit(args[i][j])){ // accept negative numbers
						options->first = args[i];
						accept_options = false;
						break;
					}
					print_fc_error(ERROR_INVALID_OPTION, (const void **)&args[i][j]);
					return -1;
				}
			}
		}
		else {
			if (options->first){
				options->last = args[i];
			} else {
				accept_options = false;
				options->first = args[i];
			}
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

	FcOptions options;
	options.first = NULL;
	options.last = NULL;
	options.editor = NULL;
	options.new = NULL;
	options.old = NULL;
	options.flags = 0;

	if (get_fc_options(command->word_list->data, &options) == -1){
		print_fc_error(fc_status, (const void **)&options);
	}

	print_options(&options);
	if (options.last == NULL){
		options.last = options.first;
	}
	
	da_create(command_list, StringListL, sizeof(char *), GC_SUBSHELL);
	if (options.first == NULL){
		da_push(command_list, fc_default());
	} else if (*options.first == '-' || ft_isdigit((int)*options.first)){
		int first_number = ft_atoi(options.first);
		int last_number = ft_atoi(options.last);
		get_history_value_nb(command_list, first_number, last_number);
	} else {
		get_history_value_wd(command_list, options.first, options.last);
	}

	if (!command_list->size){
		print_fc_error(ERROR_COMMAND_NOT_FOUND, NULL);
		return;
	}

	if (options.flags & FC_S){
		char *buffer = ft_strdup("");
		for (size_t i = 0; i < command_list->size; i++){
			char new_buffer[MAX_WORD_LEN] = {0};
			ft_snprintf(new_buffer, MAX_WORD_LEN, "%s\n%s", buffer, command_list->data[i]);
			buffer = gc(GC_ADD, ft_strdup(new_buffer), GC_SUBSHELL);
			// char *tmp = ft_strjoin(buffer, command_list->data[i]);
			// buffer = tmp;
			// gc(GC_ADD, tmp, GC_SUBSHELL);
		}

		if (options.old){
			size_t old_len = ft_strlen(options.old);
			da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);

			char *ptr = buffer;

			do {
				int pos = ft_strstr(ptr, options.old);
				if (pos == -1) break;

				for (int i = 0; i < pos; i++)
					da_push(ss, ptr[i]);

				ss_push_string(ss, options.new);
				ptr += (pos + old_len);

			} while (true);
			ss_push_string(ss, ptr);
			buffer = ss_get_owned_slice(ss);
		}

		parse_input(buffer, NULL, shell_vars);
	} else if (options.flags & FC_L){
		for (size_t i = 0; i < command_list->size; i++){
			if (!(options.flags & FC_N))
				ft_dprintf(STDOUT_FILENO, "%d\t", i);
			ft_dprintf(STDOUT_FILENO, "%s\n", command_list->data[i]);
		}
	} else {
		struct timeval	time;

		if (gettimeofday(&time, NULL) == -1){
			print_fc_error(ERROR_FC_FATAL, NULL);
			g_exitno = 1;
			return ;
		}
		char filename[] = "/tmp/42sh_fc.XXXXXX"; //7 digit max for usec
		int fd = mkstemp(filename);
		if (fd == -1){
			print_fc_error(ERROR_FC_FATAL, NULL);
			g_exitno = 1;
			return;
		}
		for (size_t i = 0; i < command_list->size; i++){
			ft_dprintf(fd, "%s\n", command_list->data[i]);
		}

		char *input = NULL;
		if (options.flags & FC_E){
			input = gc(GC_CALLOC, ft_strlen(filename) + ft_strlen(options.editor) + 2, sizeof(char), GC_SUBSHELL); // +2 for \0 and ' '
			ft_sprintf(input, "%s %s", options.editor, filename);
		} else {
			input = gc(GC_CALLOC, ft_strlen(filename) + sizeof("${EDITOR} ") + 1, sizeof(char), GC_SUBSHELL);
			ft_sprintf(input, "${EDITOR} %s", filename);
		}
		parse_input(input, NULL, shell_vars);
		lseek(fd, 0, SEEK_SET);
		char *buffer = read_whole_file(fd);
		parse_input(buffer, NULL, shell_vars);
		free(buffer);
	}

	return ;
}
