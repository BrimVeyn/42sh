/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Arena.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 16:34:04 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/07 11:48:10 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Arena.h"
#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdalign.h>

ArenaAllocator *arena_create(size_t size) {
	ArenaAllocator *arena = malloc(sizeof(ArenaAllocator));
	if (!arena)
		_fatal("arena: failed to allocate arena pointer", 1);

	arena->memory = malloc(size);
	if (!arena->memory) {
		free(arena);
		_fatal("arena: failed to allocated arena memory", 1);
	}

	arena->capacity = size;
	arena->offset = 0;

	return arena;
}

void *arena_alloc(ArenaAllocator *arena, size_t size, size_t alignment) {
	size_t current_offset = (size_t)arena->memory + arena->offset;

	size_t aligned_offset = (current_offset + (alignment - 1)) & ~(alignment - 1);
    ft_memset((void *)aligned_offset, 0, size); // Ensure memory is initialized to zero

	size_t next_offset = aligned_offset + size;
	if (next_offset > (size_t)arena->memory + arena->capacity) {
		_fatal("arena: max capacity exceeded", 1);
	}

	arena->offset = next_offset - (size_t)arena->memory;
	return (void *)aligned_offset;
}

void arena_destroy(ArenaAllocator *arena) {
	FREE_POINTERS(arena->memory, arena);
}

void arena_reset(ArenaAllocator *arena) {
	arena->offset = 0;
}

char *arena_strdup(ArenaAllocator *arena, const char *str) {
    size_t len = ft_strlen(str) + 1; // Include null terminator
    char *copy = arena_alloc(arena, len, alignof(char));
    ft_memcpy(copy, str, len);
    return copy;
}
