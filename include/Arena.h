/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Arena.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 16:42:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/08 12:47:19 by bvan-pae         ###   ########.fr       */
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

ArenaAllocator	*arena_create(size_t size);
void			*arena_alloc(ArenaAllocator *arena, size_t size, size_t alignment);
char			*arena_strdup(ArenaAllocator *arena, const char *str);
void			arena_destroy(ArenaAllocator *arena);
void			arena_reset(ArenaAllocator *arena);

FunctionP			*arena_dup_function(ArenaAllocator *arena, FunctionP *func);
ListP				*arena_dup_list(ArenaAllocator *arena, ListP *list);
CompleteCommandP	*arena_dup_complete_command(ArenaAllocator *arena, CompleteCommandP *cc);

#include  <stdalign.h>

#define arena_unique(ptr, type) arena_alloc(ptr, sizeof(type), alignof(type));


#endif // !ARENA_H
