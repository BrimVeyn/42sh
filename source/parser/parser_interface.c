/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_interface.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 16:28:44 by nbardavi          #+#    #+#             */
/*   Updated: 2024/08/29 11:48:40 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"
#include "../../include/42sh.h"

type_of_separator interface_separator(type_of_separator new_separator, int mode){
	static type_of_separator separator;
	if (mode == I_WRITE){
		separator = new_separator;
	}
	return separator;
}
