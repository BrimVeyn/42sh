/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putstr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 14:46:26 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/17 10:49:41 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"
#include <unistd.h>

void	ft_putstr_fd(const char *s, int fd)
{
	unsigned int	i;

	i = 0;
	while (s[i])
	{
		if (s[i] == -9)
		{
			write(fd, "\0", 1);
			i++;
		}
		else
			write(fd, &s[i++], 1);
	}
}
