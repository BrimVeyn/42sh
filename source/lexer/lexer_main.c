/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:08:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/19 17:28:47 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "lexer_enum.h"
#include "lexer_struct.h"
#include <stdint.h>
#include <assert.h>

char *get_tagName_redirection(type_of_redirection type) {
	switch(type) {
		case R_HERE_DOC:
			return "HERE_DOC";
		case R_INPUT:
			return "INPUT_FILE";
		case R_OUTPUT:
			return "OUTPUT_FILE";
		case R_APPEND:
			return "OUTPUT_FILE_APPEND";
		default:
			return "UNKNOWN_TYPE";
	}
}

Token oneTokenTest(char *input) {
	Lexer_p l = lexer_init(input);

	Token_or_Error tok = lexer_get_next_token(l);
	Token maybe_token = tok.token;
	assert(tok.tag == TOKEN);
	lexer_deinit(l);

	return maybe_token;
}

int main(void) {
	//Basic redirection test
	char *test_basic_redir_1 = ft_strdup("10<file");
	Token result_1 = oneTokenTest(test_basic_redir_1);
	assert((result_1.fd_prefix == 10));
	assert(!strcmp(result_1.filename, "file"));
	assert(result_1.r_type == R_INPUT);
	free(result_1.filename);

	char *test_basic_redir_2 = ft_strdup("10>file");
	Token result_2 = oneTokenTest(test_basic_redir_2);
	assert((result_2.fd_prefix == 10));
	assert(!strcmp(result_2.filename, "file"));
	assert(result_2.r_type == R_OUTPUT);
	free(result_2.filename);
	
	char *test_basic_redir_3 = ft_strdup("10>>file");
	Token result_3 = oneTokenTest(test_basic_redir_3);
	assert((result_3.fd_prefix == 10));
	assert(!strcmp(result_3.filename, "file"));
	assert(result_3.r_type == R_APPEND);
	free(result_3.filename);
	
	char *test_basic_redir_4 = ft_strdup("10<<file");
	Token result_4 = oneTokenTest(test_basic_redir_4);
	assert((result_4.fd_prefix == 10));
	assert(!strcmp(result_4.filename, "file"));
	assert(result_4.r_type == R_HERE_DOC);
	free(result_4.filename);
	
	// char *test_basic_redir_5 = ft_strdup("10<&11");
	// Token result_5 = oneTokenTest(test_basic_redir_5);
	// assert((result_5.fd_prefix == 10) && strcmp(result_5.filename, "11") && (result_5.r_type == R_INPUT));
	// 
	// char *test_basic_redir_6 = ft_strdup("10>&11");
	// Token result_1 = oneTokenTest(test_basic_redir_1);
	// assert((result_1.fd_prefix == 10) && strcmp(result_1.filename, "file") && (result_1.r_type == R_INPUT));
	// char *test_basic_redir_7 = ft_strdup("&>file");
	// char *test_basic_redir_8 = ft_strdup(">&file");
	// char *test_basic_redir_9 = ft_strdup("&>>file");
}
