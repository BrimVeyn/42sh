NAME			:= minishell

LIBFT			:= libftprintf/libftprintf.a
CC 				:= gcc
LDFLAGS			:= -lreadline -lncurses
CFLAGS 			:= -Wall -Werror -Wextra -g3 -fsanitize=address
SRC 			:= source/main.c

LEXER_TEST		:= lexer_test
LEXER_TEST_SRC	:= $(wildcard source/lexer/*.c)
LEXER_SRC 		:= $(filter-out lexer_main.c, $(wildcard *.c))
LEXER_TEST_OBJ	:= $(LEXER_TEST_SRC:source/%.c=objects/%.o)

OBJ 			:= $(SRC:source/%.c=objects/%.o)


OBJDIR 			:= objects
LEXER_DIR		:= lexer

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
	@printf "$(LEXER_TEST) done !$(DEF_COLOR)"

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
	@mkdir -p $(OBJDIR) $(OBJDIR)/$(LEXER_DIR) 

$(LIBFT) :
	@make --no-print-directory -C libftprintf/

re: fclean all

.PHONY: all clean fclean re lexer_test

