/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstadd_back.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/04 17:15:46 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/28 13:53:12 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../header/libft.h"

void	ft_lstadd_back(t_list **lst, t_list *new_value)
{
	t_list	*temp;

	if (*lst == NULL)
		*lst = new_value;
	else
	{
		temp = *lst;
		while (temp->next)
			temp = temp->next;
		temp->next = new_value;
	}
}
