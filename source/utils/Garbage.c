/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Garbage.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/09 14:26:28 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

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

static void gc_free(Garbage *gc, void *addr, int n, bool release) {
	for (uint16_t i = 0; i < gc[n].size; i++) {
		if ((uintptr_t) addr == (uintptr_t) gc[n].garbage[i]) {
			if (release)
				free(gc[n].garbage[i]);
			gc[n].garbage[i] = gc[n].garbage[gc[n].size - 1]; 
			gc[n].garbage[gc[n].size - 1] = NULL;
			gc[n].size--;
			break;
		}
	}
}

static void gc_cleanup(Garbage *gc, int n) {
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

static void gc_reset(Garbage *gc, int n) {
	for (size_t i = 0; i < gc[n].size; i++) {
		free(gc[n].garbage[i]);
		gc[n].garbage[i] = NULL;
	}
	gc[n].size = 0;
}


/**
 * @brief A garbage collection manager that allocates, tracks, and frees memory dynamically.
 *
 * This function acts as a central garbage collection (GC) system for managing dynamic memory
 * allocation, deallocation, and cleanup across different "levels" of memory management.
 * It can allocate memory (`GC_ALLOC`, `GC_CALLOC`), add memory pointers to the GC for tracking
 * (`GC_ADD`), free specific memory (`GC_FREE`), or cleanup all memory at a specific level 
 * (`GC_CLEANUP`). The function uses a static garbage collection system `GC` that holds information
 * about allocated memory and the corresponding levels.
 *
 * @param mode The mode of operation, which determines the action:
 *             - `GC_ALLOC`: Allocate memory (equivalent to `malloc`).
 *             - `GC_CALLOC`: Allocate zero-initialized memory (equivalent to `calloc`).
 *             - `GC_ADD`: Add a memory pointer to the garbage collection system for tracking.
 *             - `GC_FREE`: Free a specific memory pointer and remove it from the GC.
 *             - `GC_CLEANUP`: Free all memory in the given GC level.
 * @param ...   Additional arguments depending on the mode:
 *              - For `GC_ALLOC` and `GC_CALLOC`: 
 *                - `size_t nb`: Number of elements to allocate.
 *                - `size_t size`: Size of each element.
 *                - `int level`: The garbage collection level to assign the allocation.
 *              - For `GC_ADD`: 
 *                - `void *ptr`: Pointer to be added to the garbage collection system.
 *                - `int level`: The level at which the pointer should be tracked.
 *              - For `GC_FREE`: 
 *                - `void *ptr`: Pointer to be freed.
 *                - `int level`: The level of the pointer to free.
 *              - For `GC_CLEANUP`: 
 *                - `int level`: The level at which all memory should be cleaned up.
 * 
 * @return 
 * - On `GC_ALLOC` or `GC_CALLOC`, returns the allocated pointer after adding it to the GC.
 * - On `GC_ADD`, returns the added pointer.
 * - On `GC_FREE` or `GC_CLEANUP`, returns `NULL`.
 * 
 * @note The garbage collection system must be initialized on the first call if not already done.
 * 
 * @warning Memory allocations must specify the correct level to ensure proper tracking and cleanup.
 */
void *gc(gc_mode mode, ...) {
	static Garbage GC[GC_LEVELS] = {{NULL, 0, 10}};
	if (GC[0].garbage == NULL) {
		gc_init(GC);
    }
	va_list			args;
	va_start(args, mode);
	switch (mode) {
		case GC_ALLOC: 
			__attribute__((fallthrough));
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
			gc_free(GC, ptr, level, true);
			return NULL;
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
			gc_free(GC, ptr, old_level, false);
			gc_add(GC, ptr, new_level);
			return NULL;
		}
		default:
			return NULL;
			break;
	}
}
