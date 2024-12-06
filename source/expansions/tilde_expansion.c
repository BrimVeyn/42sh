/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tilde_expansion.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/06 10:52:02 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/06 15:50:57 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "final_parser.h"
#include "parser.h"
#include "ft_regex.h"
#include "libft.h"
#include "utils.h"
#include "utils.h"
#include <pwd.h>

//TODO:

void parser_tilde_expansion(StringStream *cache, StringStream *word, Vars *shell_vars, const int options){
	(void)shell_vars;
	if (options & O_ASSIGN){
		// char *username;
	}
	else {
		if (da_peak_front(word) != '~'){
			da_transfer(cache, word, 0);
			return;
		}

		regex_match_t result = regex_match("[!-~]+[/:]?", word->data);
		if (result.is_found){
			const char *name = ft_substr(word->data, 1, result.re_end - 1); // -1 for removing ~
			printf("name: %s\n", name);
			struct passwd *passwd = getpwnam(name);
			if (passwd){
				ss_push_string(cache, passwd->pw_dir);
				for (int i = 0; i < result.re_end; i++){
					ss_pop_front(word);
				}
				return;
				// da_push(cache, )
				// printf("Username: %s\n", passwd->pw_name);
				// printf("UID: %d\n", passwd->pw_uid);
				// printf("GID: %d\n", passwd->pw_gid);
				// printf("Home directory: %s\n", passwd->pw_dir);
				// printf("Shell: %s\n", passwd->pw_shell);
			} else {
				return;
			}
		}
	}
}
