/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 13:16:23 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <stdint.h>

//garbage collector
static Garbage gc = {NULL, 0, 0};

void gc_init(void) {
	gc.size = 0;
	gc.capacity = 10;
	gc.garbage = ft_calloc(10, sizeof(void *));
	if (!gc.garbage)
		exit(EXIT_FAILURE);
}

void *gc_add(void *ptr) {
	if (gc.size >= gc.capacity) {
		gc.capacity *= 2;
		void **tmp = gc.garbage;
		gc.garbage = ft_realloc(gc.garbage, gc.size, gc.capacity, sizeof(void *));
		free(tmp);
		if (!gc.garbage)
			exit(EXIT_FAILURE);
	}
	gc.garbage[gc.size] = ptr;
	gc.size += 1;

	return ptr;
}

void gc_cleanup(void) {
	for (uint16_t i = 0; i < gc.size; i++) {
		free(gc.garbage[i]);
	}
	free(gc.garbage);
	gc.garbage = NULL;
	gc.size = 0;
	gc.capacity = 0;
}

void gc_free(void *addr) {
	for (uint16_t i = 0; i < gc.size; i++) {
		if ((uintptr_t) addr == (uintptr_t) gc.garbage[i]) {
			free(gc.garbage[i]);
			gc.garbage[i] = NULL;
			break;
		}
	}
}


