/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline_main.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:29:59 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/10 09:44:03 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include <stdio.h>

int main(void){
	while(1){
		char *input = ft_readline("42sh> ");
		if (!input){
			break;
		}
		// printf("input: %s", input);
		add_history(input);
	}
	for (int i = 0; i < history->length; i++){
		printf("history %d: %s", i, history->entries[i]->line.data);
	}
	return 0;
}
