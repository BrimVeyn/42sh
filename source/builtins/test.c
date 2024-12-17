/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 11:17:30 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/17 14:37:06 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lexer.h"
#include "parser.h"
#include "libft.h"
#include "utils.h"
#include "exec.h"
#include <sys/stat.h>

typedef enum {
    UNARY,
	BINARY,
} TestMode;

int apply_binary_ope(char *ope_string, char *word1, char *word2){
	(void)word1; (void)word2; (void)ope_string;
	return 0;
}
int apply_unary_ope(char *ope_string, char *word){
	if (*ope_string != '-' || ft_strlen(ope_string) > 2){
		ft_dprintf(2, "42sh: test: %s: unary operator expected\n", ope_string);
		return 2;
	}
	
	struct stat file_stat;
	int stat_success = stat(word, &file_stat) == 0;

	switch(ope_string[1]){
		case 'b':
			return !(stat_success && S_ISBLK(file_stat.st_mode));
		case 'c':
			return !(stat_success && S_ISCHR(file_stat.st_mode));
		case 'd':
			return !(stat_success && S_ISDIR(file_stat.st_mode));
		case 'e':
			return !(stat_success);
		case 'f':
			return !(stat_success && S_ISREG(file_stat.st_mode));
		case 'g':
			return !(stat_success && (file_stat.st_gid & S_ISGID));
		case 'h' :
			//fall
		case 'L' :
			return !(stat_success && S_ISLNK(file_stat.st_mode));
		case 'n' : 
			return !(ft_strlen(word));
		// case 'p'
		default:

			return 0;
	}
}

void builtin_test(__attribute__((unused)) const SimpleCommandP *command, __attribute__((unused)) Vars * const shell_vars) {
	
	g_exitno = 0;
	//passer en const
	char **argv = &command->word_list->data[1]; // start at 1
	const int nargs = command->word_list->size - 1;
	TestMode mode = (nargs == 2) ? UNARY : BINARY;

	if (nargs == 0){
		g_exitno = 1;
		return;
	}
	else if (nargs == 1){
		g_exitno = (argv[0][0]); // pas certain
		return;
	}
	else if (nargs <= 3){
		if (mode == UNARY){
			char *ope_string = argv[0];
			char *word = argv[1];

			g_exitno = apply_unary_ope(ope_string, word);
		} else {
			char *ope_string = argv[1];
			char *word1 = argv[0];
			char *word2 = argv[2];

			g_exitno = apply_binary_ope(ope_string, word1, word2);
		}
	}
	else {
		ft_dprintf(2, "42sh: test: too many arguments\n");
		g_exitno = 2;
		return;
	}

	
	// if (nargs == 2){
	// 	int opt = get_unary_opt(argv[1]);
	// }

	return;
}
