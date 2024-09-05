/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_ast.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/05 09:40:01 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/05 09:40:40 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/42sh.h"

char *getKind(Node *node) {
	switch(node->tag) {
		case N_OPERAND:
			return "HEY";
			break;
		case N_OPERATOR:
			switch (node->value.operator) {
				case S_AND:
					return "AND";
					break;
				case S_OR:
					return "OR";
					break;
				default:
					return "";
			}
			break;
		default:
			return "";
	}
}


void printAST(Node *node, char *padding, bool has_rhs) {
    if (node != NULL) {
        printf("\n");
        printf("%s", padding);

        if (has_rhs) {
            printf("├──");
        } else {
            printf("└──");
        }

        printf("%s", getKind(node));

        char *new_padding = (char *)malloc(strlen(padding) + 4);
        strcpy(new_padding, padding);

        if (has_rhs) {
            strcat(new_padding, "│  ");
        } else {
            strcat(new_padding, "   ");
        }

        if (node->left != NULL) {
            printAST(node->left, new_padding, node->right != NULL);
        }
        if (node->right != NULL) {
            printAST(node->right, new_padding, false);
        }

        free(new_padding);
    }
}

void printTree(Node *self) {
    printf("%s", getKind(self));

    // const char *pointerRight = "└──";
    // const char *pointerLeft = (self->right != NULL) ? "├──" : "└──";

    char *ss = (char *)malloc(1);
    ss[0] = '\0';

    if (self->left != NULL) {
        printAST(self->left, ss, self->right != NULL);
    }
    if (self->right != NULL) {
        printAST(self->right, ss, false);
    }

    printf("\n");
    free(ss);
}
