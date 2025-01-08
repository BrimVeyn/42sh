/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   rl_undo.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/08 14:41:46 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/08 16:02:23 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "ft_regex.h"
#include "c_string.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"
#include "dynamic_arrays.h"
#include <string.h>

void undo_stack_pop_front(undo_state_stack_t *stack){
    if (stack->size > 0) {
        ft_memmove(stack->data, stack->data + 1, (stack->size - 1) * sizeof(undo_state_t *));
        stack->size--;
    }
}

void rl_save_undo_state(string *line, readline_state_t *rl_state){
    if (rl_state->undo_stack->size >512){
        undo_stack_pop_front(rl_state->undo_stack);
    }
    undo_state_t *state = gc_unique(undo_state_t, GC_SUBSHELL);

    state->cursor = rl_state->cursor;
    state->cursor.x = rl_state->cursor.x;
    state->cursor.y = rl_state->cursor.y;
    state->line = str_strdup(line);
    gc(GC_ADD, state->line.data, GC_SUBSHELL);
    
    da_push(rl_state->undo_stack, state);
}

void rl_load_previous_state(string *line, readline_state_t *rl_state){
    if (rl_state->undo_stack->size > 0){
        undo_state_t *state = da_pop(rl_state->undo_stack);

        rl_state->cursor.x = state->cursor.x;
        rl_state->cursor.y = state->cursor.y;
        *line = state->line;
    }
}
