/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_tagname.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/20 10:09:03 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/22 16:07:46 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include <stdint.h>

const char *get_tagName_token(type_of_token type) {
	switch(type) {
		case T_REDIRECTION:
			return "T_REDIRECTION";
		case T_EXPRESSION:
			return "T_EXPRESSION";
		case T_COMMAND_GROUPING:
			return "T_COMMAND_GROUPING";
		case T_SEPARATOR:
			return "T_SEPARATOR";
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

const char *get_tagName_separator(type_of_separator type) {
	switch(type) {
		case S_BG:
			return "S_BG";
		case S_OR:
			return "S_OR";
		case S_AND:
			return "S_AND";
		case S_SEMI:
			return "S_SEMI";
		case S_PIPE:
			return "S_PIPE";
		default:
			return "tagname missing (separator)";
	}
}

const char *get_tagName_error(type_of_error type) {
	switch(type) {
		case ERROR_NONE:
			return "ERROR_NONE";
		case ERROR_UNCLOSED_SQ:
			return "ERROR_UNCLOSED_SQ";
		case ERROR_UNCLOSED_CG:
			return "ERROR_UNCLOSED_CG";
		case ERROR_UNEXPCTED_TOKEN:
			return "ERROR_UNEXPCTED_TOKEN";
		case ERROR_ESCAPED_SQ:
			return "ERROR_ESCAPED_SQ";
		case ERROR_BAD_SUBSTITUTION:
			return "ERROR_BAD_SUBSTITUTION";
		default:
			return "Print not handled (error)";
	}
}

const char *get_tagName_redirection(type_of_redirection type) {
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

const char *get_tagName_command_grouping(type_of_command_grouping type) {
	switch(type) {
		case CG_SUBSHELL:
			return "CG_SUBSHELL";
		case CG_CONTROL_GROUP:
			return "CG_CONTROL_GROUP";
		case CG_CONTROL_SUBSTITUTION:
			return "CG_CONTROL_SUBSTITUTION";
		case CG_INHIBITOR_DQUOTE:
			return "CG_INHIBITOR_DQUOTE";
		default:
			return "Not handled ! (control sub)";
	}
}

const char *get_tagName_expression(type_of_expression type) {
	switch(type) {
		case EX_WORD:
			return "EX_WORD";
		case EX_VARIABLE_ASSIGNMENT:
			return "EX_VARIABLE_ASSIGNMENT";
		case EX_VARIABLE_EXPANSION:
			return "EX_VARIABLE_EXPANSION";
		case EX_BRACES_EXPANSION:
			return "EX_BRACES_EXPANSION";
		case EX_PATTERN_MATCHING:
			return "EX_PATTERN_MATCHING";
		case EX_INHIBITOR_SQUOTE:
			return "EX_INHIBITOR_SQUOTE";
		case EX_ERROR:
			return "EX_ERROR";
		default:
			return "Not handled ! (expression)";
	}
}

const char *error_handler(uintptr_t __attribute__((unused)) ptr) {
	return C_RED"TAGNAME FORMAT NOT HANDLED"C_RESET;
}
