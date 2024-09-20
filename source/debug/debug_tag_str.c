/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tag_str.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 15:49:29 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/04 15:53:18 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"

const char *get_tag_str_separator(type_of_separator type) {
	switch(type) {
		case S_BG:
			return "&";
		case S_OR:
			return "||";
		case S_AND:
			return "&&";
		case S_SEMI_COLUMN:
			return ";";
		case S_PIPE:
			return "|";
		case S_SUB_CLOSE:
			return ")";
		case S_SUB_OPEN:
			return "(";
		case S_CMD_SUB:
			return "$(";
		case S_DQ:
			return "\"";
		case S_EOF:
			return "\\0";
		case S_NEWLINE:
			return "newline";
		case S_DEFAULT:
			return "??????";
		default:
			return "???????????";
	}
}

const char *get_tag_str_redirection(type_of_redirection type) {
	switch(type) {
		case R_INPUT:
			return "<";
		case R_OUTPUT:
			return ">";
		case R_APPEND:
			return ">>";
		case R_HERE_DOC:
			return "<<";
		case R_DUP_IN:
			return "<&";
		case R_DUP_OUT:
			return ">&";
		case R_DUP_BOTH:
			return "&>";
		case R_DUP_BOTH_APPEND:
			return "&>>";
		default:
			return "?????";
	}
}
