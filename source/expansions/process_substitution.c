#include "exec.h"
#include "expansion.h"
#include "libft.h"

char *process_substitution(char *const str, const ExpKind kind, Vars *const shell_vars, int * const error) {

	(void)str;(void)kind;(void)shell_vars;(void)error;

	return ft_strdup("P");
}

