/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_tools.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 14:03:35 by nbardavi          #+#    #+#             */
/*   Updated: 2024/01/09 11:53:15 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

// char *interdup_f(char *str)
// {
//   if (str == NULL)
//     return (ft_strdup(""));
//   else
//     return (ft_strdup(str));
// }

// char	*itc_f(int c)
// {
// 	char *str;

// 	str = malloc(sizeof(char) * 2);
// 	if (str == NULL)
// 		return (NULL);
// 	if (c != '\0')
// 	{
// 		str[0] = c;
// 		str[1] = '\0';
// 	}
// 	else
// 	{
// 		str[0] = -9;
// 		str[1] = '\0';
// 	}
// 	return(str);
// }

t_trigger	get_nbr(const char *txt, int i, t_trigger trigger)
{
	char	*nbr;
	int		j;

	j = 0;
	while (ft_isdigit(txt[i++]))
		j++;
	nbr = malloc(sizeof(char) * (j + 1));
	if (nbr == NULL)
		return (trigger);
	i -= (j + 1);
	j = 0;
	while (ft_isdigit(txt[i]) == 1)
		nbr[j++] = txt[i++];
	nbr[j] = '\0';
	if (txt[i - j - 1] == '.')
		trigger.number2 = ft_atoi(nbr);
	else if (txt[i - j - 1] == 't')
		trigger.tr_nbr = ft_atoi(nbr);
	else
		trigger.number1 = ft_atoi(nbr);
	trigger.index = j;
	free(nbr);
	return (trigger);
}

int	c_flags(const char *txt, unsigned int i)
{
	char	*lst;
	int		cpt;

	cpt = 0;
	lst = "dicspuxX%";
	while (ft_instr(txt[i], lst) == 0 && txt[i] != '\0')
	{
		cpt++;
		i++;
	}
	return (cpt);
}

// int	instr(char c, char *str)
// {
// 	while (*str)
// 	{
// 		if (*str == c)
// 			return (1);
// 		++str;
// 	}
// 	return (0);
// }

char	*itc(int c)
{
	char	*str;

	str = malloc(sizeof(char) * 2);
	if (str == NULL)
		return (NULL);
	if (c != '\0')
	{
		str[0] = c;
		str[1] = '\0';
	}
	else
	{
		str[0] = -9;
		str[1] = '\0';
	}
	return (str);
}

char	*interdup(char *str)
{
	if (str == NULL)
		return (ft_strdup(""));
	else
		return (ft_strdup_free(str));
}
