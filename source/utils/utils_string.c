/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 13:14:18 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/14 16:39:56 by bvan-pae         ###   ########.fr       */
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


int ft_strstr(char *haystack, char *needle) {
	if (!haystack || !needle) {
		return -1;
	}
	
	const int needle_len = ft_strlen(needle);
	int i = 0;

	while (haystack[i]) {
		if (!ft_strncmp(&haystack[i], needle, needle_len)) {
			return i;
		}
		i += 1;
	}
	return -1;
}

int ft_strrstr(char *haystack, char *needle) {
	if (!haystack || !needle) {
		return -1;
	}
	
	const int needle_len = ft_strlen(needle);
	int i = ft_strlen(haystack) - 1;

	while (i >= 0) {
		if (!ft_strncmp(&haystack[i], needle, needle_len)) {
			return i;
		}
		i -= 1;
	}
	return -1;
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
