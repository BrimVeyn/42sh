# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/06 10:05:24 by bvan-pae          #+#    #+#              #
#    Updated: 2024/12/18 14:45:44 by bvan-pae         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME			:= 42sh

LIBFT			:= libftprintf/libftprintf.a
STRING 			:= libstring/libstring.a
CC 				:= gcc
LDFLAGS			:= -lreadline -lncurses

WWFLAGS = $(WFLAGS) -Wpedantic -Wshadow -Wconversion -Wcast-align \
  -Wstrict-prototypes -Wmissing-prototypes -Wunreachable-code -Winit-self \
  -Wmissing-declarations -Wfloat-equal -Wbad-function-cast -Wundef \
  -Waggregate-return -Wstrict-overflow=5 -Wold-style-definition -Wpadded \
  -Wredundant-decls -Wall -Werror -Wextra

CFLAGS 			:= -Wall -Wextra -Werror -Wunused-function -g3
SANFLAGS		:= -fsanitize=address -fno-omit-frame-pointer -fsanitize=undefined -fstack-protector-strong -fno-optimize-sibling-calls

LEXER_SRC 		:= $(wildcard source/lexer/*.c)
FT_READLINE_SRC := $(wildcard source/ft_readline/*.c)
EXEC_SRC 		:= $(wildcard source/exec/*.c)
AST_SRC			:= $(wildcard source/ast/*.c)
REGEX_SRC		:= $(wildcard source/regex/*.c)
UTILS_SRC		:= $(wildcard source/utils/*.c)
SIGNALS_SRC		:= $(wildcard source/signals/*.c)
BUILTINS_SRC	:= $(wildcard source/builtins/*.c)
EXPANSIONS_SRC	:= $(wildcard source/expansions/*.c)

INC       := -I./include -I./libftprintf/header -I./libstring/include

SRC 			:= source/main.c $(LEXER_SRC) $(UTILS_SRC) \
				   $(EXPANSIONS_SRC) $(EXEC_SRC) $(SIGNALS_SRC) $(AST_SRC) \
				   $(REGEX_SRC) $(BUILTINS_SRC) $(FT_READLINE_SRC) \

OBJ 			:= $(SRC:source/%.c=objects/%.o)
DEPS 			:= $(OBJ:%.o=%.d)

SAN 			:= san

OBJDIR 			:= objects
DIRS			:= $(patsubst source/%,objects/%,$(dir $(wildcard source/*/*)))

BOLD			:= \e[1m
DEF_COLOR		:= \033[0;39m
GRAY			:= \033[0;90m
RED				:= \033[0;91m
GREEN			:= \033[0;92m
YELLOW			:= \033[0;93m
BLUE			:= \033[0;94m
MAGENTA			:= \033[0;95m
CYAN			:= \033[0;96m
WHITE			:= \033[0;97m
PASTEL_BLUE     := \033[0;38;2;130;135;206m
PRUSSIAN_BLUE   := \033[0;38;2;28;49;68m
JASPER   		:= \033[0;38;2;213;87;59m
OLIVINE   		:= \033[0;38;2;154;184;122m
HIDE_CURSOR		:= \033[?25l
RESET_CURSOR	:= \033[?25h

TOTAL_FILES := $(shell echo $(SRC) | wc -w)
COMPILE_COUNT := 0

# Define a function that prints a progress bar based on the current count
define update_compile_count
    $(eval COMPILE_COUNT := $(shell echo $$(( $(COMPILE_COUNT) + 1 ))))
    $(call print_progress_bar,$(COMPILE_COUNT),$(TOTAL_FILES))
endef


# Shell function to print a progress bar
define print_progress_bar
    @CURRENT=$(1); \
    TOTAL=$(2); \
    if [ "$$TOTAL" -eq 0 ]; then \
        echo "No files to compile."; \
    else \
        PERCENT=$$((CURRENT * 100 / TOTAL)); \
        BAR_WIDTH=50; \
        EMPTY_BLOCK=" "; \
        FILLED_BLOCK="#"; \
        FILLED=$$((BAR_WIDTH * PERCENT / 100)); \
        EMPTY=$$((BAR_WIDTH - FILLED)); \
        FILLED_BAR=$$(printf "%$${FILLED}s" | sed "s/ /$${FILLED_BLOCK}/g") \
        EMPTY_BAR=$$(printf "%$${EMPTY}s" | sed "s/ /$${EMPTY_BLOCK}/g"); \
		printf "\033[?25l"; \
		printf "\033[1B"; \
        printf "\rProgress: [$(PASTEL_BLUE)$$FILLED_BAR$(DEF_COLOR)$$EMPTY_BAR] %d%% (%d/%d)\n" $$PERCENT $$CURRENT $$TOTAL; \
        if [ "$$CURRENT" -eq "$$TOTAL" ]; then printf "\033[?25h"; else printf "\033[2A"; fi; \
    fi
endef

# Wrapper to exec command in order to handle the reset color if an error occured
define cmd_wrapper
	$1 || { printf "\033[?25h"; echo -n "$(DEF_COLOR)$(RESET_CURSOR)"; exit 1; }
endef

all: compute_total $(NAME)

test: $(NAME)
	@echo "$(CYAN)Copying binary to test environment...$(DEF_COLOR)"
	@{ cp $(NAME) ./vm/backend/; }
	@echo "$(CYAN)Running tests in the test environment...$(DEF_COLOR)"
	@{ cd vm/backend/ && ./tester; }
	@echo "$(GREEN)Tests and results update complete!$(DEF_COLOR)"
	@echo "$(BLUE)Open the test results at:$(DEF_COLOR) http://localhost:3000"

compute_total:
	$(eval TOTAL_FILES := $(shell var=$$(./progression_bar.sh); if [ $${var} -ne 0 ]; then echo $${var} ; else echo $(TOTAL_FILES); fi))

$(SAN): $(LIBFT) $(STRING) $(STRING) $(OBJDIR) $(OBJ)
	@echo "$(GREEN)Making binary with sanitizer: $(NAME)"
	@printf "$(MAGENTA)"
	@$(call cmd_wrapper, $(CC) $(OBJ) $(LIBFT) $(STRING) $(CFLAGS) $(LDFLAGS) $(SANFLAGS) -o $(NAME))
	@printf "Done with sanitizer !$(DEF_COLOR)\n"

$(NAME): $(LIBFT) $(STRING) $(OBJDIR) $(OBJ)
	@echo "$(OLIVINE)Making binary: $(NAME)"
	@printf "$(JASPER)"
	@$(call cmd_wrapper, $(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) $(LIBFT) $(STRING) -o $(NAME))
	@printf "Done !$(DEF_COLOR)\n"

-include $(DEPS)
$(OBJDIR)/%.o: source/%.c
	@$(call update_compile_count)
	@printf "$(BLUE)"
	@$(call cmd_wrapper, $(CC) $(CFLAGS) $(INC) -MMD -c $< -o $@)
	@printf "$(DEF_COLOR)"

clean:
	@rm -rf $(OBJDIR)
	@make --no-print-directory -C libftprintf/ clean 
	@make --no-print-directory -C libstring/ clean 
	@printf "$(JASPER)Objects deleted !$(DEF_COLOR)\n"
fclean: clean
	@rm -rf $(NAME) $(AST_TEST) $(REGEX_TEST) $(EXEC_TEST) $(LEXER_TEST) $(FT_READLINE_TEST)
	@make --no-print-directory -C libftprintf/ fclean
	@make --no-print-directory -C libstring/ fclean
	@printf "$(JASPER)Binary deleted !$(DEF_COLOR)\n"

bear:
	bear -- make re

$(OBJDIR):
	@mkdir -p $(DIRS)

$(LIBFT):
	@make --no-print-directory -C libftprintf/

$(STRING):
	@make --no-print-directory -C libstring/

re: fclean all
	
.PHONY: all clean fclean re regex_test lexer_test exec_test san ft_readline_test
