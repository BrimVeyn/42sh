/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 15:10:45 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/06 16:10:04 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//TODO: CD :)
#include "ft_regex.h"
#include "lexer.h"
#include "parser.h"
#include "libft.h"
#include "utils.h"
#include "regex.h"
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

typedef enum {
    VALID_DIRECTORY,
    ERROR_NO_SUCH_FILE,
    ERROR_NOT_A_DIRECTORY,
    ERROR_PERMISSION_DENIED
} DirectoryStatus;

void print_cd_error(DirectoryStatus n, char *curpath){
	ft_dprintf(2, "42sh: cd: %s ", curpath);
	switch (n) {
		case ERROR_NO_SUCH_FILE:
			ft_dprintf(2, "such file or directory\n");
			break;
		case ERROR_NOT_A_DIRECTORY:
			ft_dprintf(2, "Is not a directory\n");
			break;
		case ERROR_PERMISSION_DENIED:
			ft_dprintf(2, "Permission Denied\n");
			break;
		default:
			break;
	}
}

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

static DirectoryStatus is_valid_directory(const char *path){
	struct stat file_stat;
	if (stat(path, &file_stat) == -1){
		return ERROR_NO_SUCH_FILE;
	}
	if (!S_ISDIR(file_stat.st_mode)){
		return ERROR_NOT_A_DIRECTORY;
	}
	if (!(file_stat.st_mode & S_IXUSR)){
		return ERROR_PERMISSION_DENIED;
	}
	return VALID_DIRECTORY;
}

char *search_in_cdpath(char *operand, Vars *shell_vars) {
	char *cdpath_var = string_list_get_value(shell_vars->env, "CDPATH");

	if (!cdpath_var){
		return operand;
	}
	
	char **cdpath = ft_split(cdpath_var, ':');
	for (int i = 0; cdpath[i]; i++){
		char *base_path = (cdpath[i][ft_strlen(cdpath[i]) - 1] != '/') ? 
			ft_strjoin(cdpath[i], "/") :
			ft_strdup(cdpath[i]);

		char *curpath = ft_strjoin(base_path, operand);

		free(base_path);
		if (!is_valid_directory(curpath)){
			return curpath;
		}
	}

	return NULL;
}

static void canonical_convertion(char **pcurpath){
	regex_match_t result;
	char *curpath = *pcurpath;

	while (1){ //a
		result = regex_match("\\.\\/+", curpath);
		if (!result.is_found)
			break;

		char *first_split = (result.re_start == 0) ? ft_strdup("") : ft_substr(curpath, 0, result.re_start);
		char *second_part = ft_substr(curpath, result.re_end, ft_strlen(curpath) - result.re_end);
		
		free(curpath);
		curpath = ft_strjoin(first_split, second_part);
		FREE_POINTERS(first_split, second_part);
	}
	
	da_create(stack, StringList, sizeof(char *), GC_SUBSHELL);

	while(1){
		result = regex_match("^\\/+.+[^/]", curpath);
		if (!result.is_found)
			break;

		char *next_directory = ft_substr(curpath, 0, result.re_end);
		if (!stack->size){
			da_push(stack, next_directory);
		} else {
			char *top_el = stack->data[stack->size - 1];
			
			//TODO: attention regex pas bon car / (root)
			result = regex_match("[^\\/].*$", top_el);
			char *top_el_name = ft_substr(top_el, result.re_start, ft_strlen(top_el) - result.re_start);

			result = regex_match("[^\\/].*$", top_el);
			char *next_directory_name = ft_substr(top_el, result.re_start, ft_strlen(top_el) - result.re_start);

			if (!ft_strcmp("..", next_directory_name) && ft_strcmp("")){
			}
		}
	}
	*pcurpath = curpath;
}

//TODO:get home dabord local etc
//Error a gerer, error print operand et pas curpath
//
void builtin_cd(const SimpleCommand *command, Vars *shell_vars) {

	(void)command;(void)shell_vars;
	char *curpath = NULL;
	char *operand = ft_strdup(command->args[1]);
	char *error_msg = NULL;

	if (!operand){
		curpath = ft_strdup(string_list_get_value(shell_vars->env, "HOME"));
		if (!curpath){
			write(2, "42sh: cd: HOME not set\n", 24);
			g_exitno = 1;
			return;
		}
	}

	if (operand[0] == '/') { //3
		curpath = operand;
	} else {
		if (operand[0] != '.'){
			curpath = search_in_cdpath(operand, shell_vars);
		}
		if (!curpath){
			curpath = operand;
		}
	}
	
	//if option -P -> 10
    if (curpath[0] != '/') {
        char *pwd = string_list_get_value(shell_vars->env, "PWD");
        char *tmp_pwd = (pwd && pwd[ft_strlen(pwd) - 1] != '/') ? ft_strjoin(pwd, "/") : ft_strdup(pwd);
        char *tmp = ft_strjoin(tmp_pwd, curpath);
        
        free(curpath);
        curpath = tmp;

        free(tmp_pwd);
    }

	canonical_convertion(&curpath);

	(void)error_msg;
	//is valid
	printf("curpath: %s\n", curpath);

	return;
}
