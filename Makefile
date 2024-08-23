NAME			:= minishell

LIBFT			:= libftprintf/libftprintf.a
CC 				:= cc
LDFLAGS			:= -lreadline -lncurses
CFLAGS 			:= -Wall -Werror -Wextra
DEBUG 			:= -DDEBUG
SRC 			:= source/main.c

OBJ 			:= $(SRC:source/%.c=objects/%.o)

REGEX_TEST		:= regex_test
REGEX_TEST_SRC	:= $(wildcard source/regex/*.c)
REGEX_TEST_OBJ	:= $(REGEX_TEST_SRC:source/%.c=objects/%.o)

OBJDIR 			:= objects
DOUBLE_DIR		:= double_linked_list
STRING_DIR		:= string_utils
STAR_DIR		:= star_list
MISC_DIR		:= misc
BUILTINS		:= builtins
PARSING_DIR		:= parsing
EXEC			:= exec
UTILS_EXEC      := utils_exec

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
	@mkdir -p $(OBJDIR) $(OBJDIR)/$(STAR_DIR) $(OBJDIR)/$(DOUBLE_DIR) $(OBJDIR)/$(STRING_DIR) $(OBJDIR)/$(MISC_DIR) $(OBJDIR)/$(BUILTINS)  $(OBJDIR)/$(PARSING_DIR) $(OBJDIR)/$(EXEC) $(OBJDIR)/$(UTILS_EXEC)

$(LIBFT) :
	@make --no-print-directory -C libftprintf/

re: fclean all

.PHONY: all clean fclean re regex_test
