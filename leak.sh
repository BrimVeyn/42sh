valgrind -q --show-leak-kinds=all --leak-check=full --track-origins=yes --trace-children=yes --track-fds=all --suppressions=readline_suppression.supp $1
