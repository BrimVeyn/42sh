/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/17 11:17:30 by nbardavi          #+#    #+#             */
/*   Updated: 2025/01/02 13:34:17 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "utils.h"
#include "exec.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef enum {
    UNARY,
	BINARY,
} TestMode;

int apply_binary_ope(char *ope_string, char *word1, char *word2){
	(void)word1; (void)word2; (void)ope_string;
	if (*ope_string == '-'){
		if (ft_strlen(ope_string) != 3){
			ft_dprintf(2, "42sh: test: %s: binary operator expected\n", ope_string);
			return 2;
		}

		int key = (ope_string[1] << 8) | ope_string[2];
		int n1 = ft_atoi(word1);
		int n2 = ft_atoi(word2);

		switch (key) {
			case ('e' << 8 | 'q'):
				return !(n1 == n2);
			case ('n' << 8 | 'e'):
				return !(n1 != n2);
			case ('g' << 8 | 't'):
				return !(n1 > n2);
			case ('g' << 8 | 'e'):
				return !(n1 >= n2);
			case ('l' << 8 | 't'):
				return !(n1 < n2);
			case ('l' << 8 | 'e'):
				return !(n1 <= n2);
		}
	} else {
		if (!ft_strcmp(ope_string, "!=")){
			return !(ft_strcmp(word1, word2));
		} else if (!ft_strcmp(ope_string, "=")){
			return !(!ft_strcmp(word1, word2));
		} else {
			ft_dprintf(2, "42sh: test: %s: binary operator expected\n", ope_string);
			return 2;
		}
	}
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
		case 'p' :
			return !(stat_success && file_stat.st_mode & S_IFIFO);
		case 'r' :
			return !(stat_success && file_stat.st_mode & S_IRUSR);
		case 'S' :
			return !(stat_success && S_ISSOCK(file_stat.st_mode));
		case 's' :
			return !(stat_success && file_stat.st_size > 0);
		case 't':
			return !isatty(ft_atoi(word));
		case 'u' :
			return !(stat_success && file_stat.st_mode & S_ISUID);
		case 'w' :
			return !(stat_success && file_stat.st_mode & S_IWUSR);
		case 'x' :
			return !(stat_success && file_stat.st_mode & S_IXUSR);
		case 'z' :
			return !(ft_strlen(word) == 0);
		default:
			return 0;
	}
}

void builtin_test(__attribute__((unused)) const SimpleCommandP *command, __attribute__((unused)) Vars * const shell_vars) {
	
	g_exitno = 0;
	//passer en const
	
	const int negat = (!ft_strcmp(command->word_list->data[1], "!"));
		
	// if (negat){
	// 	printf("SALUT\n");
	// }
	char **argv = &command->word_list->data[negat + 1]; // start at 1
	const int nargs = command->word_list->size - negat - 1;
	TestMode mode = (nargs == 2) ? UNARY : BINARY;

	if (nargs == 0){
		g_exitno = 1;
		return;
	}
	else if (nargs == 1){
		g_exitno = (argv[0][0]);
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

	if (g_exitno != 2 && negat){ g_exitno = g_exitno ^ 1; }

	return;
}
