# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/29 13:20:27 by nbardavi          #+#    #+#              #
#    Updated: 2024/08/30 11:32:09 by bvan-pae         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= 42sh

LIBFT			:= libftprintf/libftprintf.a
CC 				:= gcc
LDFLAGS			:= -lreadline -lncurses
CFLAGS 			:= -Wall -Werror -Wextra -g3 #-fsanitize=address

LEXER_SRC 		:= $(filter-out source/lexer/lexer_main.c, $(wildcard source/lexer/*.c))
EXEC_SRC 		:= $(filter-out source/exec/exec_main.c, $(wildcard source/exec/*.c))
DEBUG_SRC		:= $(wildcard source/debug/*.c)
REGEX_SRC		:= $(wildcard source/regex/*.c)
PARSER_SRC		:= $(wildcard source/parser/*.c)
AST_SRC			:= $(wildcard source/ast/*.c)
STRING_SRC		:= $(wildcard source/string/*.c)
UTILS_SRC		:= $(wildcard source/utils/*.c)


SRC 			:= source/main.c $(LEXER_SRC) $(DEBUG_SRC) $(UTILS_SRC) $(PARSER_SRC) $(EXEC_SRC)

OBJ 			:= $(SRC:source/%.c=objects/%.o)
# TEST

LEXER_TEST		:= lexer_test
LEXER_TEST_SRC	:= source/lexer/lexer_main.c $(LEXER_SRC) $(DEBUG_SRC) $(UTILS_SRC) $(PARSER_SRC) $(EXEC_SRC)
LEXER_TEST_OBJ	:= $(LEXER_TEST_SRC:source/%.c=objects/%.o)

REGEX_TEST		:= regex_test
REGEX_TEST_SRC	:= source/regex/regex_main.c $(REGEX_SRC) $(UTILS_SRC)
REGEX_TEST_OBJ	:= $(REGEX_TEST_SRC:source/%.c=objects/%.o)

EXEC_TEST		:= exec_test
EXEC_TEST_SRC	:= source/exec/exec_main.c $(DEBUG_SRC) $(EXEC_SRC) $(UTILS_SRC)
EXEC_TEST_OBJ	:= $(EXEC_TEST_SRC:source/%.c=objects/%.o)

OBJDIR 			:= objects
LEXER_DIR		:= $(OBJDIR)/lexer
PARSER_DIR		:= $(OBJDIR)/parser
EXEC_DIR		:= $(OBJDIR)/exec
DEBUG_DIR		:= $(OBJDIR)/debug
UTILS_DIR		:= $(OBJDIR)/utils

DIRS			:= $(OBJDIR) $(LEXER_DIR) $(PARSER_DIR) $(EXEC_DIR) $(DEBUG_DIR) $(UTILS_DIR)

DEF_COLOR		:= \033[0;39m
GRAY			:= \033[0;90m
RED				:= \033[0;91m
GREEN			:= \033[0;92m
YELLOW			:= \033[0;93m
BLUE			:= \033[0;94m
MAGENTA			:= \033[0;95m
CYAN			:= \033[0;96m
WHITE			:= \033[0;97m

all: $(NAME)

$(LEXER_TEST): $(LIBFT) $(LEXER_OBJ)
	@echo "$(RED)Making test binary: $(LEXER_TEST)"
	@printf "$(MAGENTA)"
	$(CC) $(LEXER_TEST_SRC) $(LIBFT) $(CFLAGS) $(LDFLAGS) -o $(LEXER_TEST)
	@printf "$(LEXER_TEST) done !$(DEF_COLOR)\n"

$(EXEC_TEST): $(LIBFT) $(EXEC_OBJ)
	@echo "$(RED)Making test binary: $(EXEC_TEST)"
	@printf "$(MAGENTA)"
	$(CC) $(EXEC_TEST_SRC) $(LIBFT) $(CFLAGS) $(LDFLAGS) -o $(EXEC_TEST)
	@printf "$(EXEC_TEST) done !$(DEF_COLOR)\n"

$(REGEX_TEST): $(LIBFT) $(REGEX_OBJ)
	@echo "$(RED)Making test binary: $(REGEX_TEST)"
	@printf "$(MAGENTA)"
	$(CC) $(REGEX_TEST_SRC) $(LIBFT) $(CFLAGS) $(LDFLAGS) -o $(REGEX_TEST)
	@printf "$(REGEX_TEST) done !$(DEF_COLOR)\n"

$(NAME): $(LIBFT) $(OBJDIR) $(OBJ)
	@echo "$(GREEN)Making binary: $(NAME)"
	@printf "$(MAGENTA)"
	@$(CC) $(OBJ) $(LIBFT) $(CFLAGS) $(LDFLAGS) -o $(NAME)
	@printf "Done !$(DEF_COLOR)\n"

$(OBJDIR)/%.o: source/%.c
	@printf '$(YELLOW)Compiling : %-45s $(CYAN)-->	$(YELLOW)%-30s\n' "$<" "$@";
	@printf "$(BLUE)"
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf "$(DEF_COLOR)"

clean:
	@rm -rf $(OBJDIR)
	@make --no-print-directory -C libftprintf/ clean 
	@printf "$(RED)Objects deleted !$(DEF_COLOR)\n"

fclean: clean
	@rm -rf $(NAME)
	@make --no-print-directory -C libftprintf/ fclean
	@printf "$(RED)Binary deleted !$(DEF_COLOR)\n"

$(OBJDIR):
	@mkdir -p $(DIRS)

$(LIBFT) :
	@make --no-print-directory -C libftprintf/

re: fclean all
	
.PHONY: all clean fclean re regex_test lexer_test exec_test
