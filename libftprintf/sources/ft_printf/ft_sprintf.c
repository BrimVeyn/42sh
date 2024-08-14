/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sprintf.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/09 13:32:26 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/27 17:12:52 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char		*exec_flags(const char *txt, int i, va_list args);

static char	*print_and_free2(char *str, char *returnchar)
{
	char	*returnvalue;

	if (str)
	{
		returnvalue = ft_strjoin(returnchar, str);
		free(returnchar);
		free(str);
		return (returnvalue);
	}
	return (0);
}

static char	*handle_format2(const char *txt, unsigned int *i, va_list args,
		char *returnchar)
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
	return (print_and_free2(temp, returnchar));
}

char	*ft_sprintf(char *txt, ...)
{
	va_list			args;
	unsigned int	i;
	int				count;
	char			*returnchar;

	returnchar = ft_strdup("");
	va_start(args, txt);
	count = 0;
	i = 0;
	while (txt[i])
	{
		if (txt[i] == '%')
			returnchar = handle_format2(txt, &i, args, returnchar);
		else
		{
			returnchar = ft_strjoin_free(returnchar, ft_strdup_char(txt[i]));
			count++;
		}
		i++;
	}
	va_end(args);
	return (returnchar);
}

// int	main(void)
// {
// 	char *count2 = ft_sprintf("%05d %s", 232, "Coucou");
// 	printf("%s", count2);
// 	free(count2);
// 	return (0);
// }
