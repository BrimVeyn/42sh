/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Arena.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 16:42:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/09 13:20:52 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <unistd.h>

#include "final_parser.h"

typedef struct {
	void *memory;
	size_t capacity;
	size_t offset;
} ArenaAllocator;

ArenaAllocator *arena_create(const size_t size, const gc_level garbage_level);
void			*arena_alloc(ArenaAllocator * const arena, size_t const size, size_t const alignment);
char			*arena_strdup(ArenaAllocator *arena, const char *str);
void			arena_destroy(ArenaAllocator * const arena);
void			arena_reset(ArenaAllocator * const arena);

FunctionP			*arena_dup_function(ArenaAllocator *arena, FunctionP *func);
ListP				*arena_dup_list(ArenaAllocator *arena, ListP *list);
CompleteCommandP	*arena_dup_complete_command(ArenaAllocator *arena, CompleteCommandP *cc);

#include  <stdalign.h>

#define arena_unique(arena_ptr, type) arena_alloc(arena_ptr, sizeof(type), alignof(type));
#define _arena_alloc(arena_ptr, num, type) arena_alloc(arena_ptr, num * sizeof(type), alignof(type));

#endif // !ARENA_H
