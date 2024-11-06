/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   history.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 13:20:27 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/01 16:21:26 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "c_string.h"
#include "ft_readline.h"
#include "42sh.h"
#include "utils.h"

bool history_defined = false;
HISTORY_STATE *history;


void init_history(){
	history = gc(GC_ALLOC, 1, sizeof(HISTORY_STATE), GC_READLINE);
	history->offset = 0;
	history->length = 0;
	history->capacity = 10;

	history->entries = gc(GC_CALLOC, 10, sizeof(HIST_ENTRY*), GC_READLINE);
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
    if (history->length + 1 >= history->capacity) {
        size_t new_cap = history->capacity * 2;
		HIST_ENTRY **tmp = gc(GC_CALLOC, new_cap, sizeof(HIST_ENTRY*), GC_READLINE);
		
        memcpy(tmp, history->entries, sizeof(HIST_ENTRY*) * history->length);

		gc(GC_FREE, history->entries, GC_READLINE);
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

 //    if (!history->entries[history->length]) {
 //        if (!history->entries[history->length]) {
 //            return;
 //        }
	// }

	history->entries[history->length] = gc(GC_CALLOC, 1, sizeof(HIST_ENTRY), GC_READLINE);
    history->entries[history->length]->line = string_init_str(str);
	gc(GC_ADD, history->entries[history->length]->line.data, GC_READLINE);
    history->length++;
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
