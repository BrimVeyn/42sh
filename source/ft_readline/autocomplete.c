/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   autocomplete.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/22 14:57:20 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/23 16:20:30 by nbardavi         ###   ########.fr       */
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

//TODO: set +o vi

// static void get_variable_name_starting_with(char *start, Vars *shell_vars){
//     
// }

// char *filter_name_list(char **name_list, char *match){
//     
// }

static char *get_active_word(readline_state_t *rl_state, string *line){
    int i = rl_get_cursor_pos_on_line(rl_state);
    for (;i > 0 && !is_whitespace(line->data[i]); i--){}
    return (gc(GC_ADD, ft_substr(line->data, i, rl_get_cursor_pos_on_line(rl_state) - i), GC_SUBSHELL));
}

void rl_autocomplete(readline_state_t *rl_state, string *line, Vars *shell_vars){
    (void)line; (void)rl_state;
    
    static char *previous_line = NULL;

    if (previous_line && !ft_strcmp(previous_line, line->data)){
        rl_state->suggestion.active = true;
        return;
    }

    if (previous_line){
        gc(GC_FREE, previous_line, GC_SUBSHELL);
    }
    previous_line = ft_strdup(line->data);
    gc(GC_ADD, previous_line, GC_SUBSHELL);

    char *active_word = get_active_word(rl_state, line);
    

    char **sugg_list = NULL;
    

    // move_cursor(0, 0);
    // printf("print\n");
    // set_cursor_position(rl_state);
    if (!ft_strncmp(active_word, "${", 2)){
        active_word += 2;
        sugg_list = string_list_get_all_name(shell_vars, active_word);
    } else {
        return;
    }

    if (!sugg_list[0]){
        return;
    }

    // printf("len: %ld\n", ft_strlenlen((const char **)sugg_list));
    // move_cursor(0, 0);
    // printf("0: %s\n", sugg_list[0]);
    // printf("1: %s\n", sugg_list[1]);
    // printf("active word: %s\n", active_word);
    // set_cursor_position(rl_state);

    if (!sugg_list[1]){
        rl_state->suggestion.active = false;

        int cursor = rl_get_cursor_pos_on_line(rl_state);
        rl_delete_from_n_to_cursor(rl_state, line, cursor - ft_strlen(active_word) - 2);
        str_insert_str(line, sugg_list[0], cursor - ft_strlen(active_word) - 2);
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
    int next_line = get_col() / max;
    char sugg_list_formated[100000] = {0};
    sprintf(format, "%%%ds", max + 1);
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
