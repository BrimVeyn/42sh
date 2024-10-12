/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Garbage.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/12 20:49:12 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

static void gc_init(Garbage *gc) {
	for (int n = 0; n < GC_ALL; n++) {
		gc[n].size = 0;
		gc[n].capacity = 10;
		gc[n].garbage = ft_calloc(10, sizeof(void *));
		if (!gc[n].garbage)
			exit(EXIT_FAILURE);
	}
}

static void *gc_add(Garbage *gc, void *ptr, const int n) {
	if (gc[n].size >= gc[n].capacity) {
		gc[n].capacity *= 2;
		void **tmp = gc[n].garbage;
		gc[n].garbage = (void **) ft_realloc(gc[n].garbage, gc[n].size, gc[n].capacity, sizeof(void *));
		free(tmp);
		if (!gc[n].garbage)
			exit(EXIT_FAILURE);
	}
	gc[n].garbage[gc[n].size] = ptr;
	gc[n].size += 1;

	return ptr;
}

static void *gc_free(Garbage *gc, void *addr, const int n, const bool release) {
	for (size_t i = 0; i < gc[n].size; i++) {
		if ((uintptr_t) addr == (uintptr_t) gc[n].garbage[i]) {
			if (release) {
				free(gc[n].garbage[i]);
            }
			const size_t last_index = gc[n].size - 1;
			gc[n].garbage[i] = gc[n].garbage[last_index]; 
			gc[n].garbage[last_index] = NULL;
			gc[n].size--;
			return (void *) 0;
		}
	}
	return (void *) 1;
}

static void gc_cleanup(Garbage *gc, const int n) {
	if (n == GC_ALL){
		for (int i = 0; i < GC_ALL; i++){
			gc_cleanup(gc, i);
		}
		return;
	}
	for (size_t i = 0; i < gc[n].size; i++) {
		free(gc[n].garbage[i]);
		gc[n].garbage[i] = NULL;
	}
	free(gc[n].garbage);
	gc[n].garbage = NULL;
	gc[n].size = 0;
	gc[n].capacity = 0;
}

static void gc_reset(Garbage *gc, const int n) {
	for (size_t i = 0; i < gc[n].size; i++) {
		free(gc[n].garbage[i]);
		gc[n].garbage[i] = NULL;
	}
	gc[n].size = 0;
}

void *gc(gc_mode mode, ...) {
	static Garbage GC[GC_LEVELS] = {{NULL, 0, 10}};
	if (GC[0].garbage == NULL) {
		gc_init(GC);
    }
	va_list			args;
	va_start(args, mode);
	switch (mode) {
		case GC_ALLOC: 
		case GC_CALLOC: {
			const size_t nb = va_arg(args, size_t);
			const size_t size = va_arg(args, size_t);
			const int level = va_arg(args, int);
			if (mode == GC_ALLOC)
				return gc(GC_ADD, malloc(nb * size), level);
			if (mode == GC_CALLOC)
				return gc(GC_ADD, ft_calloc(nb, size), level);
			return NULL;
		}
		case GC_REALLOC: {
			void *ptr = va_arg(args, void *);
			const size_t old_count = va_arg(args, size_t);
			const size_t new_count = va_arg(args, size_t);
			const size_t element_size = va_arg(args, size_t);
			const int level = va_arg(args, int);

			void *new_ptr = gc(GC_CALLOC, new_count, element_size, level);
			//TODO: add a fatal call in GC_ADD;
			ft_memcpy(new_ptr, ptr, old_count * element_size);
			gc_free(GC, ptr, level, true);
			return new_ptr;
		}
		case GC_ADD: {
			void *ptr = va_arg(args, void *);
			const int level = va_arg(args, int);
			return gc_add(GC, ptr, level);
		}
		case GC_GET: {
			return GC;
		}
		case GC_RESET: {
			const int level = va_arg(args, int);
			gc_reset(GC, level);
			return NULL;
		}
		case GC_FREE: {
			void *ptr = va_arg(args, void *);
			const int level = va_arg(args, int);
			return gc_free(GC, ptr, level, true);
		}
		case GC_CLEANUP: {
			const int level = va_arg(args, int);
			gc_cleanup(GC, level);
			return NULL;
		}
		case GC_MOVE: {
			void *ptr = va_arg(args, void *);
			const int old_level = va_arg(args, int);
			const int new_level = va_arg(args, int);
			assert(old_level != new_level);
			if (gc_free(GC, ptr, old_level, false) == 0) {
				gc_add(GC, ptr, new_level);
			}
			return NULL;
		}
		default:
			return NULL;
			break;
	}
}
