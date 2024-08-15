/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ast.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/15 14:06:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/08/15 16:51:07 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AST_H
#define AST_H

#include "../lexer/lexer.h"
#include "../lexer/lexer_struct.h"

typedef struct Node {
	Token token;
	struct Node *left;
	struct Node *right;
} Node;

typedef struct Utils {
	//ptr to utils AST functions;
	//build;
	//print;
	//deinit;
} Utils;

typedef struct AST {
	Node		*root;
	char			*input;
	Utils	*func;
} AST;

#endif // !AST_H
