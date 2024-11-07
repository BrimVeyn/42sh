/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 15:10:45 by nbardavi          #+#    #+#             */
/*   Updated: 2024/11/07 17:16:38 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//TODO: CD :)
#include "ft_regex.h"
#include "lexer.h"
#include "parser.h"
#include "libft.h"
#include "utils.h"
#include "regex.h"
#include <linux/limits.h>
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

DirectoryStatus cd_status;

void print_cd_error(DirectoryStatus n, char *curpath){
	ft_dprintf(2, "42sh: cd: %s: ", curpath);
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
	else if (!S_ISDIR(file_stat.st_mode)){
		return ERROR_NOT_A_DIRECTORY;
	}
	else if (!(file_stat.st_mode & S_IXUSR)){
		return ERROR_PERMISSION_DENIED;
	}
	return VALID_DIRECTORY;
}

char *search_in_cdpath(char *operand, Vars *shell_vars) {
	const char *cdpath_var = string_list_get_value(shell_vars->env, "CDPATH");

	if (cdpath_var){
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
	}
	return ft_strdup(operand);
}

static char *build_path(StringList *stack){
	char *full_path = ft_strdup("");

	for (size_t i = 0; i < stack->size; i++){
		char *next = stack->data[i];
		char *tmp = ft_strjoin(full_path, next);
		free(full_path);
		full_path = tmp;
	}
	return full_path;
}

void canonical_convertion(char **pcurpath){
	regex_match_t result;
	char *curpath = *pcurpath;
	
	//TODO:cut le premier ./
	while (1){ //a
		result = regex_match("\\/\\.\\/+", curpath);
		if (!result.is_found)
			break;

		char *first_split = (result.re_start == 1) ? ft_strdup("") : ft_substr(curpath, 0, result.re_start + 1);
		char *second_part = ft_substr(curpath, result.re_end, ft_strlen(curpath) - result.re_end - 1);
		
		curpath = gc(GC_ADD, ft_strjoin(first_split, second_part), GC_SUBSHELL);
		FREE_POINTERS(first_split, second_part);
	}

	printf("curpath after cut ./: %s\n", curpath);
	
	da_create(stack, StringList, sizeof(char *), GC_SUBSHELL);

	while(1){
		result = regex_match("\\/?[^\\/]+", curpath);
		if (!result.is_found)
			break;
		char *next_directory = gc(GC_ADD, ft_substr(curpath, 0, result.re_end), GC_SUBSHELL);

		//cut first elem in curpath
		char *tmp = ft_substr(curpath, result.re_end, ft_strlen(curpath) - result.re_end);
		curpath = gc(GC_ADD, tmp, GC_SUBSHELL);

		if (!stack->size){
			da_push(stack, next_directory);
		} else {
			char *top_el = da_pop(stack);
			
			result = regex_match("[^\\/]+$", top_el);
			char *top_el_name = ft_substr(top_el, result.re_start, ft_strlen(top_el) - result.re_start);

			result = regex_match("[^\\/]+$", next_directory);
			char *next_directory_name = ft_substr(next_directory, result.re_start, ft_strlen(top_el) - result.re_start);

			// printf("top: %s\n", top_el);
			// printf("top el name: %s\n", top_el_name);
			// printf("next_directory_name: %s\n\n", next_directory_name);

			if (ft_strcmp("..", next_directory_name) || !ft_strcmp("..", top_el_name)){
				da_push(stack, top_el);
				da_push(stack, next_directory);
			}
			FREE_POINTERS(top_el_name, next_directory_name);
		}
		
		char *full_path = build_path(stack);
		cd_status = is_valid_directory(full_path);
		free(full_path);
		if (cd_status){
			return;
		}
	}
	
	curpath = gc(GC_ADD, build_path(stack), GC_SUBSHELL);

	//removing trailing /
	
	result = regex_match("^\\/\\/\\/+", curpath);
	if (result.is_found){
		curpath += result.re_end - 1;
	}
	result = regex_match("\\/+$", curpath);
	if (result.is_found){
		curpath[result.re_start] = '\0';
	}
	
	while (1){
		result = regex_match("\\/\\/+", curpath);
		if (!result.is_found){
			break;
		}

		char *first = ft_substr(curpath, 0, result.re_start + 1);
		char *end = ft_substr(curpath, result.re_end, ft_strlen(curpath) - result.re_end);

		curpath = ft_strjoin(first, end);
		gc(GC_ADD, curpath, GC_SUBSHELL);
		FREE_POINTERS(first, end);
	}
	printf("\ncurpath in canonical form: %s\n", curpath);
	*pcurpath = curpath;
}

////usr/.////././///home/
//TODO:get home dabord local etc
//Error a gerer, error print operand et pas curpath
//
void builtin_cd(const SimpleCommand *command, Vars *shell_vars) {
	cd_status = VALID_DIRECTORY;
	char *curpath = NULL;
	char *operand = (command->args[1] != NULL) ? ft_strdup(command->args[1]) : ft_strdup("");
	gc(GC_ADD, operand, GC_SUBSHELL);
	char *error_msg = NULL;
	
	if (!operand){
		curpath = gc(GC_ADD, ft_strdup(string_list_get_value(shell_vars->env, "HOME")), GC_SUBSHELL);
		if (!curpath){
			write(2, "42sh: cd: HOME not set\n", 24);
			g_exitno = 1;
			return;
		}
	}

	if (operand[0] == '/') { //3
		curpath = gc(GC_ADD, ft_strdup(operand), GC_SUBSHELL);
	} else {
		if (operand[0] != '.'){
			curpath = gc(GC_ADD, search_in_cdpath(operand, shell_vars), GC_SUBSHELL);
		}
		if (!curpath){
			curpath = gc(GC_ADD, ft_strdup(operand), GC_SUBSHELL);
		}
	}
	
	// if option -P -> 10
    if (curpath[0] != '/') {
        const char *pwd = string_list_get_value(shell_vars->env, "PWD");

		char buffer[MAX_WORD_LEN] = {0};
		if (pwd && pwd[ft_strlen(pwd) - 1] != '/'){
			if (ft_snprintf(buffer, MAX_WORD_LEN, "%s/%s", pwd, curpath) == -1)
				fatal("cd: FATAL", 255);
		} else{
			if (ft_snprintf(buffer, MAX_WORD_LEN, "%s%s", pwd, curpath) == -1)
				fatal("cd: FATAL", 255);
		}
		
		curpath = gc(GC_ADD, ft_strdup(buffer), GC_SUBSHELL);
    }

	canonical_convertion(&curpath);

	
	if (cd_status){
		print_cd_error(cd_status, operand);
		g_exitno = 1;
		return;
	}
	
	if (ft_strlen(curpath) > PATH_MAX && ft_strlen(operand) < PATH_MAX)
	//is valid
	return;
}
