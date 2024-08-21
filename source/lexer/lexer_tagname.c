/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_tagname.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:09:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/21 14:56:34 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"

char *get_tagName_token(type_of_token type) {
	switch(type) {
		case T_REDIRECTION:
			return "T_REDIRECTION";
		case T_EXPRESSION:
			return "T_EXPRESSION";
		case T_PATTERN_MATCHING:
			return "T_PATTERN_MATCHING";
		case T_CONTROL_GROUP:
			return "T_CONTROL_GROUP";
		case T_SEPARATOR:
			return "T_SEPARATOR";
		case T_UNKNOWN_TYPE:
			return "T_UNKNOWN_TYPE";
		case T_END_OF_FILE:
			return "T_END_OF_FILE";
		case T_NONE:
			return "T_NONE";
		case T_COMMAND:
			return "T_COMMAND";
		default:
			return "FORMAT TYPE NOT HANDLED";
	}
}

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

char *get_tagName_command_grouping(type_of_command_grouping type) {
	switch(type) {
		case CG_SUBSHELL:
			return "CG_SUBSHELL";
		case CG_CONTROL_GROUP:
			return "CG_CONTROL_GROUP";
		case CG_CONTROL_SUBSTITUTION:
			return "CG_CONTROL_SUBSTITUTION";
		default:
			return "Not handled !";
	}
}

char *get_tagName_expression(type_of_expression type) {
	switch(type) {
		case EX_WORD:
			return "EX_WORD";
		case EX_VARIABLE_EXPANSION:
			return "EX_VARIABLE_EXPANSION";
		case EX_BRACES_EXPANSION:
			return "EX_BRACES_EXPANSION";
		case EX_PATTERN_MATCHING:
			return "EX_PATTERN_MATCHING";
		case EX_ERROR:
			return "EX_ERROR";
		default:
			return "EX_ERROR";
	}
}
