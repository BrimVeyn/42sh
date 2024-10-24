/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_lexer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/04 12:53:31 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/10/24 14:53:20 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug.h"
#include "colors.h"
#include "lexer.h"
#include "libft.h"
#include "utils.h"

#include <stdio.h>

void ifStructPrint(IfStruct *cf) {
	dprintf(2, "IF:{\n");
	for (size_t i = 0; i < cf->conditions->size; i++) {
		dprintf(2, ".condition[%zu]: ", i);
		expr_array_print(cf->conditions->data[i]);
		dprintf(2, "\n");
	}
	for (size_t i = 0; i < cf->bodies->size; i++) {
		dprintf(2, ".body[%zu]: ", i);
		expr_array_print(cf->bodies->data[i]);
		dprintf(2, "\n");
	}
	if (cf->else_body->size) {
		expr_array_print(cf->else_body);
		dprintf(2, "\n");
    }
	dprintf(2, "}");
}

void expr_print(Expr *expr) {
	dprintf(2, "Expr(");
	switch (expr->type) {
		case EXPR_WORD: {
			dprintf(2, "WORD:{%s}", expr->word->litteral);
			break;
		}
		case EXPR_SEP: {
			dprintf(2, "SEP:{%s}", expr->sep->litteral);
			break;
		}
		case EXPR_IF: {
			ifStructPrint(expr->ifstruct);
			break;
		}
		default: break;
	}
	dprintf(2, ") ");
}

void expr_array_print(ExprArray *array) {
	for (size_t i = 0; i < array->size; i++) {
		expr_print(array->data[i]);
	}
}

char *get_line_x(char *in, const size_t n) {
	char *line_ptr = in;
	for (size_t i = 0; i < n; i++) {
		line_ptr = ft_strchr(line_ptr, '\n');
	}

	char *end = ft_strchr(line_ptr, '\n');
	char *return_line = ft_substr(in, (line_ptr - in), (end - line_ptr));
	return return_line;
}

void printContextStack(LexemContextStack *stack) {
	dprintf(2, C_LIGHT_RED"--------Lexem Stack--------"C_RESET"\n");
	for (size_t i = 0; i < stack->size; i++) {
		dprintf(2, "[%zu]: %s\n",i, LexemStr(stack->data[i]->lexem));
	}
	dprintf(2, C_LIGHT_RED"---------------------------"C_RESET"\n");
}

char *LexemStr(Lexem lexem) {

	static const struct {
		Lexem lexem;
		char  *value;
	} litterals[] = {
		{LNONE, "stack_bottom"}, {IF, "if"}, {THEN, "then"}, {ELSE, "else"},
	    {ELIF, "elif"}, {FI, "fi"}, {CASE, "case"},
		{ESAC, "esac"}, {WHILE, "while"}, {FOR, "for"},
		{SELECT, "select"}, {UNTIL, "until"}, {DO, "do"},
		{DONE, "done"}, {IN, "in"}, {FUNCTION, "function"},
		{TIME, "time"}, {L_CURLY_BRACKET, "{"}, {R_CURLY_BRACKET, "}"},
		{L_SQUARE_BRACKET, "["}, {R_SQUARE_BRACKET, "]"},
		{SEMI_COLUMN, ";"}, {COMMAND, "CMD"}, {NEWLINE, "CR"},
		{ENDOFFILE, "EOF"}, {ARGS, "bin/arg"},
	};

	for (size_t i = 0; i < ARRAY_SIZE(litterals); i++) {
		if (lexem == litterals[i].lexem) {
			return litterals[i].value;
		}
	}
	return "INVALID";
}

void printLexemState(LexemInfos *infos) {
	if (!infos) 
		return ;
	char buffer[MAX_WORD_LEN] = {0};
	ft_sprintf(buffer, C_BRIGHT_CYAN"%s"C_RESET": "C_BRIGHT_YELLOW, LexemStr(infos->lexem));
	for (size_t i = 0; i < 31; i++) {
		if (infos->ctx_mandatory && (infos->ctx_mandatory->value & (1 << i)) > 0) {
			ft_sprintf(buffer, "%s ", LexemStr(1 << i));
		}
	}
	ft_sprintf(buffer, C_RESET"\n");
	ft_putstr_fd(buffer, 2);
}

void printStackStates(LexemContextStack *stack, char *color) {
	dprintf(2, "%s------STATES------"C_RESET"\n", color);
	for (size_t i = 0; i < stack->size; i++) {
		printLexemState(stack->data[i]);
	}
	dprintf(2, "%s------------------"C_RESET"\n", color);
}
void lexer_debug(Lexer_p lexer) {
	dprintf(2, "--- Lexer state ---\n");
	dprintf(2, "Input = |%s|\n", lexer->input);
	dprintf(2, "Position = %zu\n", lexer->position);
	dprintf(2, "Read_position = %zu\n", lexer->read_position);
	dprintf(2, "Ch = %c | %d\n", lexer->ch, lexer->ch);
	dprintf(2, "-------------------\n");
}

void tokenListToString(TokenList *t) {
	for (uint16_t i = 0; i < t->size; i++) {
		tokenToString(t->data[i], 0);
	}
}

static void printOffset(const size_t offset) {
	for (size_t i = 0; i < offset; i++) {
		dprintf(2, " ");
	}
}

void tokenToString(Token *t, size_t offset) {
	printOffset(offset);
	if (t->e != ERROR_NONE) {
		dprintf(2, C_RED"ERROR: %s\n"C_RESET, tagName(t->e));
	}
	switch(t->tag) {
		case T_REDIRECTION:
			dprintf(2, C_LIGHT_YELLOW"T_REDIRECTION"C_RESET" {\n");
			printOffset(offset + 4);
			dprintf(2, "Type: "C_MEDIUM_YELLOW"%s\n"C_RESET, tagName(t->r_type));
			tokenToString(t->r_postfix, offset + 4);
			printOffset(offset);
			dprintf(2, "}\n");
			break;
		case T_NONE:
			dprintf(2, C_MEDIUM_GRAY"T_NONE"C_RESET" {}\n");
			break;
		case T_WORD:
			dprintf(2, C_MEDIUM_BLUE"T_WORD"C_RESET" {\n");
			printOffset(offset + 4);
			dprintf(2, C_LIGHT_BLUE"w_infix: %s\n"C_RESET, t->w_infix);
			printOffset(offset + 4);
			dprintf(2, C_LIGHT_BLUE"w_postfix: "C_RESET);
			tokenToString(t->w_postfix, offset + 4);
			printOffset(offset);
			dprintf(2, "}\n");
			break;
		case T_SEPARATOR:
			dprintf(2, C_PURPLE"T_SEPARATOR"C_RESET" {\n");
			printOffset(offset + 4);
			dprintf(2, "Type: "C_VIOLET"%s\n"C_RESET, tagName(t->s_type));
			printOffset(offset);
			dprintf(2, "}\n");
			break;
		default:
			dprintf(2, "Format not handled\n");
	}
}
