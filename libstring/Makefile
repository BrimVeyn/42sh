GCC=cc
CFLAGS=-Wall -Wextra -Werror -g3 -mtune=native -march=native -O3 -fomit-frame-pointer -Iinclude -mavx -mavx2 -fPIE

SRCDIR=srcs
OBJDIR=objs
SRCS=$(wildcard $(SRCDIR)/**/*.c $(SRCDIR)/*.c)
OBJS=$(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET=libstring.a

GREEN = \033[38;2;4;231;98m
BLUE = \033[38;2;119;166;182m
RED = \033[0;31m
RESET = \033[0m

all: $(TARGET)

$(TARGET): $(OBJS)
	@ar rcs $(TARGET) $(OBJS)
	@printf "$(GREEN)╭────────────────────────────────────────────╮\n"
	@printf "│        LibString Created Successfully      │\n"
	@printf "╰────────────────────────────────────────────╯\n$(RESET)"

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@$(GCC) $(CFLAGS) -c $< -o $@

clean:
	@rm -frd $(OBJDIR)

fclean: clean
	@rm -f $(TARGET)

re: fclean all

.PHONY: all clean fclean re

# Rule to generate compile_commands.json using Bear
bear: clean
	@bear -- make $(TARGET)
