/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autocomplete.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 14:57:20 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/24 14:50:38 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "ft_readline.h"
#include "ft_regex.h"
#include "c_string.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <termcap.h>
#include <unistd.h>
#include <ncurses.h> 
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dirent.h>
#include <sys/stat.h>

//TODO: set +o vi
//TODO: invalid read attention

// static void get_variable_name_starting_with(char *start, Vars *shell_vars){
//     
// }

// char *filter_name_list(char **name_list, char *match){
//     
// }

typedef struct sugg_infos_s {
    enum { RL_S_ENV, RL_S_FILE, RL_S_BUILTIN, RL_S_NONE} mode;
    char *active_word;
} sugg_info_t;

static bool is_separator(int c){
    return (c == ';' || c == '|');
}

static void get_sugg_infos(readline_state_t *rl_state, string *line, sugg_info_t *ctx){
    int i = rl_get_cursor_pos_on_line(rl_state);
    for (;i > 0 && !is_whitespace(line->data[i]); i--){}
    int word_start = (i == 0) ? i : i + 1;
    ctx->active_word = gc(GC_ADD, ft_substr(line->data, word_start, rl_get_cursor_pos_on_line(rl_state) - word_start), GC_SUBSHELL);

    ctx->mode = RL_S_BUILTIN;

    if (!ft_strncmp("${", ctx->active_word, 2)){
        ctx->mode = RL_S_ENV;
        return;
    }

    for (; i > 0; i--){
        if (is_separator(line->data[i])){
            break;
        }
        else if (!is_whitespace(line->data[i])){
            ctx->mode = RL_S_FILE;
            break;
        }
    }

    return;
}

static char **get_pathnames(char *input) {
    char *last_slash = (input) ? ft_strrchr(input, '/') : '\0';
    char *dirname = NULL;
    char *basename = NULL;
    
    if (last_slash) {
        size_t dirname_len = last_slash - input;
        dirname = gc(GC_ADD, ft_substr(input, 0, dirname_len + 1), GC_SUBSHELL);
        basename = gc(GC_ADD, ft_strdup(last_slash + 1), GC_SUBSHELL);
    } else {
        dirname = gc(GC_ADD, ft_strdup("."), GC_SUBSHELL);
        basename = gc(GC_ADD, ft_strdup(input), GC_SUBSHELL);
    }

    // move_cursor(0, 0);
    // printf("dirname: %s\nbasename: %s\n", dirname, basename);

    DIR *dir = opendir(dirname);
    if (!dir) {
        return NULL;
    }

    size_t match_count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (ft_strncmp(entry->d_name, basename, strlen(basename)) == 0) {
            match_count++;
        }
    }

    // printf("match_count: %zu\n", match_count);

    char **name_list = gc(GC_CALLOC, match_count + 2, sizeof(char *), GC_SUBSHELL);
    if (!name_list) {
        return NULL;
    }

    rewinddir(dir);
    size_t i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, basename, strlen(basename)) == 0 && entry->d_name[0] != '.') {
            char *complete_pathname = NULL;
            if (entry->d_type == DT_DIR){
                complete_pathname = ft_strjoin(entry->d_name, "/");
            } else {
                complete_pathname = ft_strdup(entry->d_name);
            }

            if (ft_strcmp(".", dirname)){
                char *copy = ft_strjoin(dirname, complete_pathname);
                free(complete_pathname); complete_pathname = copy;
            }
            // char *complete_pathname = gc(GC_ADD, ft_strjoin(entry->d_name, "/"), GC_SUBSHELL);
            name_list[i++] = gc(GC_ADD, complete_pathname, GC_SUBSHELL);
        }
    }

    closedir(dir);

    return name_list;
}

static char **get_builtin_name(char *filter){
	static const char *const builtins[] = {
		"alias", "bg", "cd", "echo", "env", "exit",
		"export", "fc", "fg", "hash", "jobs",
		"pwd", "return", "set", "test", "type",
		"unalias", "unset",
	};

    char **name_list = gc(GC_CALLOC, 20, sizeof(char *), GC_SUBSHELL);
    size_t filter_len = (filter) ? ft_strlen(filter) : 0;
        
    size_t i = 0;
    for (size_t j = 0; builtins[j]; j++){
        if (!filter || !ft_strncmp(builtins[j], filter, filter_len)){
            name_list[i] = gc(GC_ADD, ft_strdup(builtins[j]), GC_SUBSHELL);
            i++;
        }
    }
    
    return name_list;
}

void rl_autocomplete(readline_state_t *rl_state, string *line, Vars *shell_vars){
    (void)line; (void)rl_state;

    // char *active_word = get_active_word(rl_state, line);

    char **sugg_list = NULL;
    
    // move_cursor(0, 0);
    // printf("print\n");
    // set_cursor_position(rl_state);
    //
    
    sugg_info_t ctx = {
        .active_word = NULL,
        .mode = RL_S_NONE,
    };

    get_sugg_infos(rl_state, line, &ctx);

    if (ctx.mode == RL_S_ENV){
        sugg_list = string_list_get_all_name(shell_vars, ctx.active_word + 2);
    } else if (ctx.mode == RL_S_BUILTIN){
        sugg_list = get_builtin_name(ctx.active_word);
    } else if (ctx.mode == RL_S_FILE){
        sugg_list = get_pathnames(ctx.active_word);
        set_cursor_position(rl_state);
    }

    if (!sugg_list){
        return;
    }

    if (!sugg_list[0]){
        return;
    }

    if (!sugg_list[1]){
        rl_state->suggestion.active = false;

        int cursor = rl_get_cursor_pos_on_line(rl_state);
        rl_delete_from_n_to_cursor(rl_state, line, cursor - ft_strlen(ctx.active_word));
        str_insert_str(line, sugg_list[0], cursor - ft_strlen(ctx.active_word));
        rl_move_to_next_word_end_sp(rl_state, line);
        rl_move_forward_by_char(rl_state, line);
        return;
    }
    
    rl_state->suggestion.mode = RL_S_NOTFOUND;

    int max = ft_strlen(sugg_list[0]);
    for(int i = 1; sugg_list[i]; i++){
        int len = ft_strlen(sugg_list[i]);
        if (len > max){
            max = len;
        }
    }
    char *format = gc(GC_CALLOC, 32, sizeof(char), GC_SUBSHELL);
    int next_line = get_col() / (max + 3);
    char sugg_list_formated[100000] = {0};
    sprintf(format, "%%%ds", max + 3);
    size_t buffer_len = 0;
    sugg_list_formated[buffer_len++] = '\n';
    for(int i = 0; sugg_list[i]; i++){
        char *copy = ft_strdup(sugg_list_formated);

        buffer_len += sprintf(sugg_list_formated + buffer_len, format, sugg_list[i]);
        free(copy);
        if (!((i + 1) % next_line)){
            char *copy = ft_strdup(sugg_list_formated);
            buffer_len += sprintf(sugg_list_formated + buffer_len, "\n");
            free(copy);
        }
    }

    if (sugg_list_formated[buffer_len - 1] != '\n'){
        sugg_list_formated[buffer_len] = '\n';
        buffer_len++;
    }

    gc(GC_FREE, format, GC_SUBSHELL);

    rl_state->suggestion.active = true;
    rl_state->suggestion.formated_string = sugg_list_formated;
}
