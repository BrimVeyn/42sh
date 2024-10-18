/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 10:48:08 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/18 17:15:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

size_t	ft_strlen(const char *theString)
{
	size_t	i;

	i = 0;
	if (theString == NULL)
		return (0);
	while (theString[i])
		i++;
	return (i);
}
