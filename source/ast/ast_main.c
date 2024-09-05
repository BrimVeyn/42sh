/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast_main.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/02 11:47:59 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 12:52:54 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

int main(void) {
	gc_init();

	Lexer_p l1 = lexer_init("echo nathan | (echo bryan && echo yoann) | rev && cat");
	TokenList *tokens = lexer_lex_all_test(l1, S_EOF, DEFAULT);
	TokenListStack *branch_list = split_operator(tokens);
	tokenListToStringAll(branch_list); //Debug
	branch_list_to_rpn(branch_list);
	Node *AST = generateTree(branch_list);
	printTree(AST);

	gc_cleanup();
}
