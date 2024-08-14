/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fd_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/27 16:37:56 by bvan-pae          #+#    #+#             */
/*   Updated: 2023/12/27 17:11:39 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char		*exec_flags(const char *txt, int i, va_list args);

static int	print_and_free3(char *str, int fd)
{
	int	len;

	if (str)
	{
		len = (int)ft_strlen(str);
		ft_putstr_fd(str, fd);
		free(str);
		return (len);
	}
	return (0);
}

static int	handle_format3(const char *txt, unsigned int *i, va_list args,
		int fd)
{
	int		nbf;
	char	*temp;

	nbf = c_flags(txt, *i + 1);
	if (nbf == 0)
		temp = check_p(txt, ++(*i), args);
	else
	{
		temp = exec_flags(txt, ++(*i), args);
		if (temp)
			*i += nbf;
	}
	return (print_and_free3(temp, fd));
}

int	fd_printf(int fd, const char *txt, ...)
{
	va_list			args;
	unsigned int	i;
	int				count;

	if (txt == NULL)
		return (-1);
	va_start(args, txt);
	count = 0;
	i = 0;
	while (txt[i])
	{
		if (txt[i] == '%')
			count += handle_format3(txt, &i, args, fd);
		else
		{
			ft_putchar_fd(txt[i], fd);
			count++;
		}
		i++;
	}
	va_end(args);
	return (count);
}
