valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --track-fds=yes --trace-children=yes --suppressions=readline_suppression.supp ./minishell
