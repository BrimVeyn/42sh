/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 15:10:45 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/06 10:04:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//TODO: CD :)
#include "lexer.h"
#include "parser.h"
#include "libft.h"
#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

void update_pwd(Vars *shell_vars){
	(void)shell_vars;
	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));
	
	char *pwd = ft_strjoin("OLDPWD=", string_list_get_value(shell_vars->env, "PWD"));
	string_list_update(shell_vars->env, pwd); // Update OLDPWD
	free(pwd);
	pwd = ft_strjoin("PWD=", cwd);
	string_list_update(shell_vars->env, pwd); // Update PWD
}

static int is_valid_directory(const char *path){
	struct stat file_stat;
	if (stat(path, &file_stat) == -1){
		ft_dprintf(2, "42sh: cd: %s: No such file or directory\n", path);
		return false;
	}
	if (!S_ISDIR(file_stat.st_mode)){
		ft_dprintf(2, "42sh: cd: %s: Is not a directory\n", path);
		return false;
	}
	if (!(file_stat.st_mode & S_IXUSR)){
		ft_dprintf(2, "42sh: cd: %s: Permission Denied\n", path);
		return false;
	}
	return true;
}
// 	if (chdir(path) != 0){
// 		ft_dprintf(2, "42sh: cd: %s: chdir failed\n", path);
// 		return 1;
// 	}
// 	update_pwd(shell_vars);
// 	return 0;
// }

char *search_in_cdpath(char *operand, Vars *shell_vars) {
	char *cdpath_var = string_list_get_value(shell_vars->env, "CDPATH");
	if (!cdpath_var){
		return operand;
	}
	
	char **cdpath = ft_split(cdpath_var, ':');
	for (int i = 0; cdpath[i]; i++){
		char *base_path = (cdpath[i][ft_strlen(cdpath[i] - 1)] != '/') ? 
			ft_strjoin(cdpath[i], "/") :
			ft_strdup(cdpath[i]);

		char *curpath = ft_strjoin(base_path, operand);

		free(base_path);
		if (is_valid_directory(curpath)){
			return curpath;
		}
	}

	return NULL;
}

void builtin_cd(const SimpleCommand *command, Vars *shell_vars) {

	(void)command;(void)shell_vars;
	// char *curpath = NULL;
	// char *operand = command->args[1];
	// 
	// if (!operand){
	// 	curpath = ft_strdup(string_list_get_value(shell_vars->env, "HOME"));
	// 	if (!curpath){
	// 		write(2, "42sh: cd: HOME not set\n", 24);
	// 		g_exitno = 1;
	// 		return;
	// 	}
	// }
	//
	// if (operand[0] == '/') { //3
	// 	curpath = command->args[1];
	// } else {
	// 	if (operand[0] != '.'){
	// 		search_in_cdpath(operand);
	// 	}
	//
	// }
	//
	// g_exitno = go_to_path(curpath, shell_vars);
	return;
}
