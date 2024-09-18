NAME = libftprintf.a
CC = cc
CFLAGS = -Wall -Wextra -Werror -g3
SRCS = sources/custom_utils/ft_strcmp.c \
sources/custom_utils/ft_strjoin_free.c \
sources/libft/ft_atoi.c \
sources/libft/ft_bzero.c \
sources/libft/ft_calloc.c \
sources/libft/ft_isalnum.c \
sources/libft/ft_isalpha.c \
sources/libft/ft_isascii.c \
sources/libft/ft_isdigit.c \
sources/libft/ft_isprint.c \
sources/libft/ft_itoa.c \
sources/libft/ft_litoa.c \
sources/libft/ft_lstadd_back.c \
sources/libft/ft_lstadd_front.c \
sources/libft/ft_lstclear.c \
sources/libft/ft_lstdelone.c \
sources/libft/ft_lstiter.c \
sources/libft/ft_lstlast.c \
sources/libft/ft_lstmap.c \
sources/libft/ft_lstnew.c \
sources/libft/ft_lstsize.c \
sources/libft/ft_memchr.c \
sources/libft/ft_memcmp.c \
sources/libft/ft_memcpy.c \
sources/libft/ft_memmove.c \
sources/libft/ft_memset.c \
sources/libft/ft_putchar_fd.c \
sources/libft/ft_putendl_fd.c \
sources/libft/ft_putnbr_fd.c \
sources/libft/ft_putstr_fd.c \
sources/libft/ft_split.c \
sources/libft/ft_strchr.c \
sources/libft/ft_strdup.c \
sources/libft/ft_striteri.c \
sources/libft/ft_strjoin.c \
sources/libft/ft_strlcat.c \
sources/libft/ft_strlcpy.c \
sources/libft/ft_strlen.c \
sources/libft/ft_strmapi.c \
sources/libft/ft_strncmp.c \
sources/libft/ft_strnstr.c \
sources/libft/ft_strrchr.c \
sources/libft/ft_strtrim.c \
sources/libft/ft_substr.c \
sources/libft/ft_tolower.c \
sources/libft/ft_toupper.c

OBJS = $(SRCS:sources/%.c=objets/%.o)
AR = ar rcs
LIB = ranlib
OBJDEST = ./objets
OBJDIR = objets

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

$(NAME): $(OBJDIR) $(OBJS)
	@echo "$(GREEN)Making binary: $(NAME)"
	@printf "$(MAGENTA)"
	@$(AR) $(NAME) $(OBJS)
	@$(LIB) $(NAME)
	@printf "Done !$(DEF_COLOR)\n"

$(OBJDEST)/%.o: sources/%.c
	@printf '$(YELLOW)Compiling : %-40s $(CYAN)-->	$(YELLOW)%-30s\n' "$<" "$@";
	@printf "$(BLUE)"
	@$(CC) $(CFLAGS) -c $< -o $@
	@printf "$(DEF_COLOR)"

$(OBJDIR):
	@mkdir -p $(OBJDIR) $(OBJDIR)/custom_utils $(OBJDIR)/ft_printf $(OBJDIR)/libft

clean:
	@rm -rf $(OBJDIR)

fclean: clean
	@rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re bonus


