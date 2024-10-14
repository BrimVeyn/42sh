/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:20:27 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/14 13:24:16 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "42sh.h"

bool history_defined = false;
HISTORY_STATE *history;

void init_history(){
	history = malloc(sizeof(HISTORY_STATE));
	history->offset = 0;
	history->length = 0;
	history->capacity = 10;

	history->entries = ft_calloc(10, sizeof(HIST_ENTRY*));
}

void destroy_history(){
	for (int i = 0; i < history->length; i++){
		str_destroy(&history->entries[i]->line);
		free(history->entries[i]);
	}
	free(history->entries);
	free(history);
}

static void history_realloc() {
    if (history->length + 1 >= history->capacity) {
        size_t new_cap = history->capacity * 2;
        HIST_ENTRY **tmp = ft_calloc(new_cap, sizeof(HIST_ENTRY*));

        memcpy(tmp, history->entries, sizeof(HIST_ENTRY*) * history->length);

        free(history->entries);
        history->entries = tmp;
        history->capacity = new_cap;
    }
}

void add_history(const char *str) {
    if (!history_defined) {
        init_history();
        history_defined = true;
    }
	history_realloc();

    if (!history->entries[history->length]) {
        history->entries[history->length] = malloc(sizeof(HIST_ENTRY));
        if (!history->entries[history->length]) {
            return;
        }
    }

    history->entries[history->length]->line = STRING_L(str);
    history->length++;
}

void pop_history(){
	if (history->length > 0){
		str_destroy(&history->entries[history->length - 1]->line);
		free(history->entries[history->length - 1]);
		history->entries[history->length - 1] = NULL;
		history->length--;
	}
}
