/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 19:21:44 by nbardavi          #+#    #+#             */
/*   Updated: 2024/02/27 10:57:43 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

static int	ft_instr(char const *str, char const c)
{
	while (*str)
	{
		if (c == *str)
			return (1);
		str++;
	}
	return (0);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*end;

	while (*s1 && ft_instr(set, *s1))
		s1++;
	if (!*s1)
		return (ft_strdup(""));
	end = (char *)(s1 + ft_strlen(s1) - 1);
	while (end > s1 && ft_instr(set, *end))
		end--;
	return (ft_substr(s1, 0, end - s1 + 1));
}
