/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/29 13:51:53 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lexer/lexer.h"
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
	if (gc.size >= gc.capacity) {
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

void *ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize) {
	void *new_ptr = malloc(nbEl * elSize);
	if (!new_ptr)
		exit(EXIT_FAILURE);
	ft_memcpy(new_ptr, ptr, oldSize * elSize);
	free(ptr);
	return new_ptr;
}

bool is_whitespace(char c) {
	return (c == ' ' || c == '\t');
}

bool is_redirection_char(char c) {
	return (c == '>' || c == '<');
}

bool is_number(char *str) {
	for (uint16_t i = 0; str[i]; i++) {
		if (!ft_isdigit(str[i])) return false;
	}
	return true;
}

void free_charchar(char **s){
	for (int i = 0; s[i]; i++){
		free(s[i]);
	}
	free(s);
}
