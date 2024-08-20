/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   garbage.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 11:08:14 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include <stdint.h>

//garbage collector
static Garbage gc = {NULL, 0, 0};

void gc_init(void) {
	gc.garbage = ft_calloc(10, sizeof(void *));
	if (!gc.garbage)
		exit(EXIT_FAILURE);
	gc.size = 0;
	gc.capacity = 10;
}

void *gc_add(void *ptr) {
	if (gc.size == gc.capacity) {
		gc.capacity *= 2;
		gc.garbage = ft_realloc(gc.garbage, gc.size, gc.capacity, sizeof(void *));
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
