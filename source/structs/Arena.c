/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 16:34:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/31 13:43:22 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Arena.h"
#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdalign.h>

ArenaAllocator *arena_create(const size_t size, const gc_level garbage_level) {
	ArenaAllocator *arena = gc_unique(ArenaAllocator, garbage_level);

	arena->memory = gc(GC_CALLOC, size, sizeof(char), garbage_level);

	arena->gc_level = garbage_level;
	arena->capacity = size;
	arena->offset = 0;

	return arena;
}

void *arena_alloc(ArenaAllocator * const arena, size_t const size, size_t const alignment) {
	size_t current_offset = (size_t)arena->memory + arena->offset;

	size_t aligned_offset = (current_offset + (alignment - 1)) & ~(alignment - 1);

	size_t next_offset = aligned_offset + size;
	if (next_offset > (size_t)arena->memory + arena->capacity) {
		if (!arena->next)
			arena->next = arena_create(arena->capacity, arena->gc_level);
		return arena_alloc(arena->next, size, alignment);
	}

	arena->offset = next_offset - (size_t)arena->memory;
	return (void *)aligned_offset;
}

void arena_destroy(ArenaAllocator * const arena) {
	FREE_POINTERS(arena->memory, arena);
}

void arena_reset(ArenaAllocator * const arena) {
	ft_memset(arena->memory, 0, arena->capacity);
	arena->offset = 0;
	arena->next = NULL;
}

char *arena_strdup(ArenaAllocator *arena, const char *str) {
    size_t len = ft_strlen(str) + 1; // Include null terminator
    char *copy = arena_alloc(arena, len, alignof(char));
    ft_memcpy(copy, str, len);
    return copy;
}
