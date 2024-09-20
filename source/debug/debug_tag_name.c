/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tag_name.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:37:54 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/19 10:02:00 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "colors.h"

const char *get_tag_name_token(type_of_token type) {
	switch(type) {
		case T_REDIRECTION:
			return "T_REDIRECTION";
		case T_SEPARATOR:
			return "T_SEPARATOR";
		case T_NONE:
			return "T_NONE";
		case T_WORD:
			return "T_WORD";
		default:
			return "FORMAT TYPE NOT HANDLED";
	}
}

const char *get_tag_name_separator(type_of_separator type) {
	switch(type) {
		case S_BG:
			return "S_BG";
		case S_OR:
			return "S_OR";
		case S_AND:
			return "S_AND";
		case S_SEMI_COLUMN:
			return "S_SEMI_COLUMN";
		case S_PIPE:
			return "S_PIPE";
		case S_SUB_CLOSE:
			return "S_SUB_CLOSE";
		case S_SUB_OPEN:
			return "S_SUB_OPEN";
		case S_EOF:
			return "S_EOF";
		case S_NEWLINE:
			return "S_NEWLINE";
		case S_DEFAULT:
			return "S_DEFAULT";
		default:
			return "tagname missing (separator)";
	}
}

const char *get_tag_name_error(type_of_error type) {
	switch(type) {
		case ERROR_NONE:
			return "ERROR_NONE";
		case ERROR_UNCLOSED_SQ:
			return "ERROR_UNCLOSED_SQ";
		case ERROR_UNCLOSED_DQ:
			return "ERROR_UNCLOSED_DQ";
		case ERROR_UNCLOSED_CG:
			return "ERROR_UNCLOSED_CG";
		case ERROR_UNEXPECTED_TOKEN:
			return "ERROR_UNEXPECTED_TOKEN";
		case ERROR_ESCAPED_SQ:
			return "ERROR_ESCAPED_SQ";
		case ERROR_BAD_SUBSTITUTION:
			return "ERROR_BAD_SUBSTITUTION";
		default:
			return "Print not handled (error)";
	}
}

const char *get_tag_name_redirection(type_of_redirection type) {
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

const char *error_handler(uintptr_t __attribute__((unused)) ptr) {
	return C_RED"TAGNAME FORMAT NOT HANDLED"C_RESET;
}

const char *get_tag_name_suffix(type_of_suffix type) {
	switch(type) {
		case R_FILENAME:
			return "R_FILENAME";
		case R_FD:
			return "R_FD";
		default:
			return "R_UNKNOWN";
	}
}
