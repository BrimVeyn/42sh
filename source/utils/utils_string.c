/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:14:18 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/09 14:30:13 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

void *ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize) {
	void *new_ptr = malloc(nbEl * elSize);
	if (!new_ptr)
		exit(EXIT_FAILURE);
	ft_memcpy(new_ptr, ptr, oldSize * elSize);
	gc_free(ptr, GC_GENERAL);
	return new_ptr;
}

int ft_strlenlen(const char **strstr){
	int i = 0;
	for (; strstr[i]; i++){}
	return i;
}

char **ft_strdupdup(const char **env){
	char **strstr = ft_calloc(ft_strlenlen(env) + 1, sizeof(char *));
	if (!strstr){
		return NULL;
	}
	for (int i = 0; env[i]; i++){
		strstr[i] = ft_strdup(env[i]);
	}
	return strstr;
}
