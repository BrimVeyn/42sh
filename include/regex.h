/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/16 15:38:14 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/03 13:14:51 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REGEX_H
#define REGEX_H

typedef struct {
	int start;
	int end;
} match_result;


void regex_test(char *regexp, char *text);
int regex_matchhere(char *regexp, char *text, int *text_pos);
match_result regex_match(char *regexp, char *text);

#endif
