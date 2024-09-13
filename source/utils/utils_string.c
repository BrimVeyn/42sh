/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:14:18 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/13 11:09:01 by bvan-pae         ###   ########.fr       */
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

char *gnl(int fd) {
	char buffer[2];
	char *line = NULL;
	size_t len = 0;

	buffer[1] = '\0';

	while (read(fd, buffer, 1) > 0 && buffer[0] != '\n') {
		char *tmp = realloc(line, len + 2);
		if (!tmp) {
			free(line);
			return NULL;
		}
		line = tmp;
		line[len] = buffer[0];
		len++;
	}
	if (line) {
		line[len] = '\0';
	}
	return line;
}
