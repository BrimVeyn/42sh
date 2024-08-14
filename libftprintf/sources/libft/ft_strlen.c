/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlen.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/01 10:48:08 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:38:01 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

size_t	ft_strlen(const char *theString)
{
	size_t	i;

	i = 0;
	if (theString == NULL)
		return (0);
	if (theString[0] == -9)
		i++;
	while (theString[i])
		i++;
	return (i);
}
