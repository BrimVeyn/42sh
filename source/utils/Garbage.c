/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Garbage.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/14 16:40:02 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

//garbage collector
static Garbage gc[10] = {{NULL, 0, 0}};

void gc_init(int n) {
	gc[n].size = 0;
	gc[n].capacity = 10;
	gc[n].garbage = ft_calloc(10, sizeof(void *));
	if (!gc[n].garbage)
		exit(EXIT_FAILURE);
}

Garbage *gc_get(void) {
	return gc;
}

void *gc_add(void *ptr, int n) {
	if (gc[n].size >= gc[n].capacity) {
		gc[n].capacity *= 2;
		void **tmp = gc[n].garbage;
		gc[n].garbage = ft_realloc(gc[n].garbage, gc[n].size, gc[n].capacity, sizeof(void *));
		free(tmp);
		if (!gc[n].garbage)
			exit(EXIT_FAILURE);
	}
	gc[n].garbage[gc[n].size] = ptr;
	gc[n].size += 1;

	return ptr;
}

void gc_cleanup(int n) {
	if (n == GC_ALL){
		for (int i = 0; i < GC_ALL; i++){
			gc_cleanup(i);
		}
		return;
	}
	for (uint16_t i = 0; i < gc[n].size; i++) {
		free(gc[n].garbage[i]);
		gc[n].garbage[i] = NULL;
	}
	free(gc[n].garbage);
	gc[n].garbage = NULL;
	gc[n].size = 0;
	gc[n].capacity = 0;
}

void gc_free(void *addr, int n) {
	for (uint16_t i = 0; i < gc[n].size; i++) {
		if ((uintptr_t) addr == (uintptr_t) gc[n].garbage[i]) {
			free(gc[n].garbage[i]);
			gc[n].garbage[i] = NULL;
			break;
		}
	}
}

void gc_addcharchar(char **str, int n) {
	for (uint16_t i = 0; str[i]; i++){
		gc_add(str[i], n);
	}
	gc_add(str, n);
}

