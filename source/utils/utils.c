/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:40:25 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/12 09:33:19 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"
#include <stdint.h>

//garbage collector
static Garbage gc[10] = {{NULL, 0, 0}};

void gc_init(int n) {
	gc[n].size = 0;
	gc[n].capacity = 10;
	gc[n].garbage = ft_calloc(10, sizeof(void *));
	if (!gc[n].garbage)
		exit(EXIT_FAILURE);
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

char *replace_char_greedy(char *str, char c, char by){
	int j = 0;
	
	for (int i = 0; str[i]; i++){
		while(str[i] && str[i] == c){
			i++;
			j++;
		}
		if (j != 0){
			char *start = ft_substr(str, 0, i - j);
			char *end = ft_substr(str, i, ft_strlen(str));
			char *newstr = ft_calloc(ft_strlen(start) + ft_strlen(end) + 2, sizeof(char));
			if (str[i - j - 1] == ';') // if there is already semi
				sprintf(newstr, "%s%s", start, end);
			else
				sprintf(newstr, "%s;%s", start, end);
			free(start); free(end);
			return replace_char_greedy(gc_add(newstr, GC_GENERAL), c, by);
		}
	}
	return str;
}
