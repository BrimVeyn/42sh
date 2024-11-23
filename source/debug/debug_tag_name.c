/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_tag_name.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/28 13:37:54 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/23 22:38:51 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "colors.h"


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

const char *error_handler(uintptr_t __attribute__((unused)) ptr) {
	return C_RED"TAGNAME FORMAT NOT HANDLED"C_RESET;
}
