/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/25 11:37:44 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/02 14:21:28 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c_string.h"
#include "ft_readline.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "ft_regex.h"
#include <linux/limits.h>
#include <string.h>

bool history_defined = false;
HISTORY_STATE *history;

void init_history(Vars *shell_vars) {

	history = gc_unique(HISTORY_STATE, GC_READLINE);
	history->offset = 0;
	history->navigation_offset = 0;
	history->length = 0;
	history->capacity = 10;
	history->entries = gc(GC_CALLOC, 10, sizeof(HIST_ENTRY*), GC_READLINE);

	//config
	history->config = gc_unique(HISTORY_CONFIG, GC_READLINE);
	history->config->histsize = 500;
	history->config->histfilesize = 500;
	string_list_add_or_update(shell_vars->set, "HISTSIZE=500");
	string_list_add_or_update(shell_vars->set, "HISTFILESIZE=500");

	char *home = getenv("HOME");
	char history_filename[PATH_MAX] = {0};
	ft_sprintf(history_filename, "HISTFILE=%s/.42sh_history", home);
	string_list_add_or_update(shell_vars->set, history_filename);
}

void print_history_values(HISTORY_STATE *history) {
    if (!history) {
        printf("HISTORY_STATE is NULL\n");
        return;
    }

    printf("============ HISTORY_STATE DEBUG ============\n");
    printf("Offset: %d\n", history->offset);
	printf("Navigation Offset: %d\n", history->navigation_offset);
    printf("Length: %d\n", history->length);
    printf("Capacity: %d\n", history->capacity);
    printf("--------------------------------------------\n");

    if (!history->entries) {
        printf("Entries pointer is NULL\n");
        return;
    }

    for (int i = 0; i < history->length; i++) {
        if (history->entries[i]) {
            printf("Entry %d: %s\n", i, history->entries[i]->line.data ? history->entries[i]->line.data : "(null)");
        } else {
            printf("Entry %d: (null)\n", i);
        }
    }
    printf("============================================\n");
}

void destroy_history(){
	for (int i = 0; i < history->length; i++){
		gc(GC_FREE, history->entries[i]->line.data, GC_READLINE);
		gc(GC_FREE, history->entries[i], GC_READLINE);
	}
	gc(GC_FREE, history->entries, GC_READLINE);
	gc(GC_FREE, history, GC_READLINE);
}

static void history_realloc() {
    if (history->length + history->offset >= history->capacity) {
        int new_cap = history->capacity;

        while (new_cap <= history->length + history->offset) {
            new_cap *= 2;
        }

        HIST_ENTRY **tmp = gc(GC_CALLOC, new_cap, sizeof(HIST_ENTRY*), GC_READLINE);

        for (int i = 0; i < history->length; i++) {
            tmp[i] = history->entries[i];
        }

        for (int i = history->length; i < new_cap; i++) {
            tmp[i] = NULL;
        }

        gc(GC_FREE, history->entries, GC_READLINE);
        history->entries = tmp;
        history->capacity = new_cap;

    }
}

void add_history(const char *str, Vars *shell_vars) {
    char *chistsize = string_list_get_value(shell_vars->set, "HISTSIZE");
    int histsize = -1;
    if (chistsize) {
        histsize = ft_atoi(chistsize);
    }

    if (!history_defined) {
        init_history(shell_vars);
        history_defined = true;
    }

    history_realloc();

    if (history->length < histsize || histsize == -1) {
        history->entries[history->length] = gc(GC_CALLOC, 1, sizeof(HIST_ENTRY), GC_READLINE);
        history->entries[history->length]->line = string_init_str(str);
        gc(GC_ADD, history->entries[history->length]->line.data, GC_READLINE);
        history->length++;
    } else {
        history->entries++;
        history->capacity--;

        history->entries[history->length - 1] = gc(GC_CALLOC, 1, sizeof(HIST_ENTRY), GC_READLINE);
        history->entries[history->length - 1]->line = string_init_str(str);
        gc(GC_ADD, history->entries[history->length - 1]->line.data, GC_READLINE);
    }
}

void pop_history(){
	if (history->length > 0){
		gc(GC_FREE, history->entries[history->length - 1]->line.data, GC_READLINE);
		gc(GC_FREE, history->entries[history->length - 1], GC_READLINE);
		history->entries[history->length - 1] = NULL;
		history->length--;
	}
}

char *search_in_history(char *str){
	for (int i = history->length - 1; i > 0; i--){
		if (ft_strstr(history->entries[i]->line.data, str) != -1){
			return history->entries[i]->line.data;
		}
	}
	return NULL;
}

void adapt_histsize(int new_histsize, HISTORY_STATE *history){
	if ((new_histsize < history->length && new_histsize != -1) || (history->config->histsize == -1 && new_histsize != -1)){
		history->entries += (history->length - new_histsize - 1);
		history->length = new_histsize + CURRENT_LINE; //+1
	}
	history->config->histsize = new_histsize;
	// printf("after adapt_histsize");
	// print_history_values(history);
}

void handle_history_config(HISTORY_STATE *history, Vars *shell_vars){
	char *c_histsize = string_list_get_value(shell_vars->set, "HISTSIZE");
	int new_histsize = -1;
	if (c_histsize && regex_match("[^0-9]", c_histsize).is_found == false){
		new_histsize = ft_atoi(c_histsize);
	}

	if (new_histsize != history->config->histsize){
		adapt_histsize(new_histsize, history);
	}
	
	return;
}
