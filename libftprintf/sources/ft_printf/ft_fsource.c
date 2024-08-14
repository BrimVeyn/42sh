/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_fsource.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbardavi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/16 13:36:20 by nbardavi          #+#    #+#             */
/*   Updated: 2023/12/24 12:34:31 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/ft_printf.h"

char	*fzero(char *str, t_trigger trigger, size_t lenstr)
{
	if (str[0] == '-' && trigger.dot == 0)
		str = ft_strjoin_s2("-", joinflag(0, ft_substr_free(str, 1, lenstr),
					'0', trigger.number1 - lenstr));
	else if (trigger.dot == 1)
		str = joinflag(0, str, ' ', trigger.number1 - lenstr);
	else if (trigger.dot == 0)
		str = joinflag(0, str, '0', trigger.number1 - lenstr);
	return (str);
}

char	*fdot(char *str, t_trigger trigger, size_t lenstr)
{
	char	*temp;

	if (str[0] == '-' && trigger.origine != 0)
		str = ft_strjoin_s2("-", joinflag(0, ft_substr_free(str, 1, lenstr),
					'0', trigger.number2 - lenstr + 1));
	else if (trigger.origine > 0 && trigger.number2 <= 0)
		str = ft_strdup_free(str);
	else if (trigger.origine == 0 && trigger.number2 <= 0)
	{
		free((void *)str);
		temp = ft_strdup("");
		return (temp);
	}
	else if (trigger.origine != 0 || trigger.number2 >= 0)
		str = joinflag(0, str, '0', trigger.number2 - lenstr);
	return (str);
}

char	*sdot(char *str, t_trigger trigger)
{
	if (trigger.s_origine == NULL && trigger.number2 <= 5)
	{
		free(str);
		return (ft_strdup(""));
	}
	return (str = ft_substr_free(str, 0, trigger.number2));
}
