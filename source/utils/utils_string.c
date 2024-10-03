/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_string.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:17:39 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/02 12:49:04 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"
#include "libft.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void *ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize) {
	void *new_ptr = ft_calloc(nbEl, elSize);
	if (!new_ptr)
		exit(EXIT_FAILURE);
	ft_memcpy(new_ptr, ptr, oldSize * elSize);
	gc(GC_FREE, ptr, GC_GENERAL);
	return new_ptr;
}

size_t ft_strlenlen(const char **strstr){
	size_t i = 0;
	for (; strstr[i]; i++) {}
	return i;
}

void free_charchar(char **s){
	for (int i = 0; s[i]; i++){
		free(s[i]);
	}
	free(s);
}

// char **ft_strdupdup(const StringList *env){
// 	char **strstr = ft_calloc(ft_strlenlen(env) + 1, sizeof(char *));
// 	if (!strstr){
// 		return NULL;
// 	}
// 	for (int i = 0; env[i]; i++){
// 		strstr[i] = ft_strdup(env[i]);
// 	}
// 	return strstr;
// }


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
			FREE_POINTERS(start, end);
			return replace_char_greedy(gc(GC_ADD, newstr, GC_GENERAL), c, by);
		}
	}
	return str;
}

long	ft_atol(const char *str)
{
	int	i = 0;
	int8_t	sign = 1;
	long	result = 0;

	while ((str[i] >= 9 && str[i] <= 13) || str[i] == 32)
		i++;
	if (str[i] == '+' && str[i + 1] != '-')
		i++;
	if (str[i] == '-')
	{
		i++;
		sign = -1;
	}
	while (str[i] && str[i] >= '0' && str[i] <= '9')
	{
		result *= 10;
		result += str[i] - '0';
		i++;
	}
	return (result * sign);
}

static int	count_digit(long n)
{
	int				cpt;
	unsigned long	nbr;

	cpt = 1;
	if (n < 0)
	{
		nbr = -n;
		cpt++;
	}
	else
		nbr = n;
	while (nbr >= 10)
	{
		nbr /= 10;
		cpt++;
	}
	return (cpt);
}

char	*ft_ltoa(long n)
{
	char			*returnvalue;
	uint8_t				i;
	uint8_t				fin;
	unsigned long	nbr;

	i = count_digit(n) - 1;
	fin = count_digit(n);
	returnvalue = malloc(sizeof(char) * fin + 1);
	if (!returnvalue)
		return (NULL);
	if (n < 0)
	{
		nbr = -n;
		returnvalue[0] = '-';
	}
	else
		nbr = n;
	while (nbr > 9)
	{
		returnvalue[i--] = (nbr % 10) + '0';
		nbr /= 10;
	}
	returnvalue[i] = nbr + '0';
	returnvalue[fin] = '\0';
	return (returnvalue);
}
