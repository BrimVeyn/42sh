/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_tagname.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:09:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/20 18:17:36 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"

char *get_tagName_redirection(type_of_redirection type) {
	switch(type) {
		case R_INPUT:
			return "R_INPUT";
		case R_OUTPUT:
			return "R_OUTPUT";
		case R_APPEND:
			return "R_APPEND";
		case R_HERE_DOC:
			return "R_HERE_DOC";
		case R_DUP_IN:
			return "R_DUP_IN";
		case R_DUP_OUT:
			return "R_DUP_OUT";
		case R_DUP_BOTH:
			return "R_DUP_BOTH";
		case R_DUP_BOTH_APPEND:
			return "R_DUP_BOTH_APPEND";
		default:
			return "R_UNKNOWN";
	}
}
char *get_tagName_expression(type_of_expression type) {
	switch(type) {
		case EX_WORD:
			return "EX_WORD";
		case EX_CONTROL_SUBSTITUTION:
			return "EX_CONTROL_SUBSTITUTION";
		case EX_VARIABLE_EXPANSION:
			return "EX_VARIABLE_EXPANSION";
		case EX_BRACES_EXPANSION:
			return "EX_BRACES_EXPANSION";
		case EX_PATTERN_MATCHING:
			return "EX_PATTERN_MATCHING";
		default:
			return "EX_ERROR";
	}
}
