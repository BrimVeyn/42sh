/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_front.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 17:01:28 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/28 13:52:56 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

void	ft_lstadd_front(t_list **lst, t_list *new_value)
{
	if (new_value != NULL)
	{
		new_value->next = *lst;
		*lst = new_value;
	}
}
