/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:18:21 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/11 13:39:45 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */

#include "ft_regex.h"
#include "lexer.h"
#include "parser.h"
#include "libft.h"
#include "utils.h"
#include "exec.h"
#include "regex.h"

#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

#define CD_P 1
#define CD_L 2

typedef enum {
    VALID_DIRECTORY,
    ERROR_NO_SUCH_FILE,
    ERROR_NOT_A_DIRECTORY,
    ERROR_PERMISSION_DENIED,
	ERROR_INVALID_OPTION,
	ERROR_TOO_MANY_ARGUMENTS,
	ERROR_HOME_NOT_SET,
	ERROR_OLDPWD_NOT_SET
} DirectoryStatus;

static DirectoryStatus cd_status;

void print_cd_error(DirectoryStatus n, void **arg){
	ft_dprintf(2, "42sh: cd: ");
	switch (n) {
		case ERROR_NO_SUCH_FILE:
			ft_dprintf(2, "%s: such file or directory\n", (char *)*arg);
			break;
		case ERROR_NOT_A_DIRECTORY:
			ft_dprintf(2, "%s: Is not a directory\n", (char *)*arg);
			break;
		case ERROR_PERMISSION_DENIED:
			ft_dprintf(2, "%s: Permission Denied\n" );
			break;
		case ERROR_INVALID_OPTION: {
			char option_str[2] = {(char)(*((int *)arg)), '\0'};
			ft_dprintf(2, "-%s: Invalid option\n", option_str);
			break;
		}
		case ERROR_TOO_MANY_ARGUMENTS:
			ft_dprintf(2, "Too many arguments\n");
			break;
		case ERROR_HOME_NOT_SET:
			ft_dprintf(2, "HOME not set\n");
			break;
		case ERROR_OLDPWD_NOT_SET:
			ft_dprintf(2, "OLDPWD not set\n");
			break;
		default:
			break;
	}
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
			gc(GC_ADD, base_path, GC_SUBSHELL);
			char *curpath = ft_strjoin(base_path, operand);
			gc(GC_ADD, curpath, GC_SUBSHELL);

			gc(GC_FREE, base_path, GC_SUBSHELL);
			if (!is_valid_directory(curpath)){
				return curpath;
			}
		}
	}
	return gc(GC_ADD, ft_strdup(operand), GC_SUBSHELL);
}

static char *build_path(StringListL *stack){
	char *full_path = gc(GC_ADD, ft_strdup(""), GC_SUBSHELL);

	for (size_t i = 0; i < stack->size; i++){
		char *next = stack->data[i];
		char *tmp = ft_strjoin(full_path, next);
		gc(GC_FREE, full_path, GC_SUBSHELL);
		full_path = gc(GC_ADD, tmp, GC_SUBSHELL);
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

	// regex_test("\\/?[^\\/]+", curpath);
	
	da_create(stack, StringListL, sizeof(char *), GC_SUBSHELL);

	while(1){
		result = regex_match("\\/?[^\\/]+", curpath);
		if (!result.is_found){
			break;
		}
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
		// printf("full_path: %s\n", full_path);
		cd_status = is_valid_directory(full_path);
		gc(GC_FREE, full_path, GC_SUBSHELL);
		if (cd_status){
			return;
		}
	}

	if (stack->size)
		curpath = build_path(stack);

	//removing trailing /
	
	result = regex_match("^\\/\\/\\/+", curpath);
	if (result.is_found){
		curpath += result.re_end - 1;
	}
	result = regex_match("\\/+$", curpath);
	if (result.is_found && result.re_start != 0){
		curpath[result.re_start] = '\0';
	}
	// printf("\ncurpath in canonical form: %s\n", curpath);
	
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
	*pcurpath = curpath;
}

int last_elem_is(const char *str, char c){
	return (str[ft_strlen(str) - 1] == c);
}

int get_flags_and_operand(char **args, int *options, char **operand){
	for (int i = 1; args[i]; i++){
		if (args[i][0] == '-' && args[i][1]){ //args[i][1] for handling cd -
			for (int j = 1; args[i][j]; j++){
				if (args[i][j] == 'P'){
					*options |= CD_P;
				}
				else if (args[i][j] == 'L'){
					*options |= CD_L;
				}
				else {
					cd_status = ERROR_INVALID_OPTION;
					*options = args[i][j];
					return -1;
				}
			}
		} else if (*operand){
			cd_status = ERROR_TOO_MANY_ARGUMENTS;
			return -1;
		} else {
			*operand = gc(GC_ADD, ft_strdup(args[i]), GC_SUBSHELL);
		}
	}
	return 0;
}

void builtin_cd(const SimpleCommandP *command, Vars * const shell_vars) {
	cd_status = VALID_DIRECTORY;
	char *curpath = NULL;

	int options = 0;
	char *operand = NULL;
	if (get_flags_and_operand(command->word_list->data, &options, &operand) == -1){
		print_cd_error(cd_status, (void **)&options);
		g_exitno = 1;
		return;
	}

	if (options > 2){
		ft_dprintf(2, "42sh: cd: -%s: Invalid option\n", (char*)&options);
		g_exitno = 1;
		return;
	}
	
	if (!operand){
		const char *home = get_variable_value(shell_vars, "HOME");
		if (!home){
			g_exitno = 1;
			cd_status = ERROR_OLDPWD_NOT_SET;
			print_cd_error(ERROR_OLDPWD_NOT_SET, NULL);
			return;
		}
		operand = gc(GC_ADD, ft_strdup(home), GC_SUBSHELL);
	}

	if (!ft_strcmp(operand, "-")){
		const char *oldpwd = get_variable_value(shell_vars, "OLDPWD");
		if (!oldpwd){
			g_exitno = 1;
			cd_status = ERROR_OLDPWD_NOT_SET;
			print_cd_error(ERROR_OLDPWD_NOT_SET, NULL);
			return;
		}
		operand = gc(GC_ADD, ft_strdup(oldpwd), GC_SUBSHELL);
		ft_dprintf(1, "%s\n", operand);
	}
	

	if (operand[0] == '/') { //3
		curpath = gc(GC_ADD, ft_strdup(operand), GC_SUBSHELL);
	} else {
		if (operand[0] != '.'){
			curpath = search_in_cdpath(operand, shell_vars);
		}
		if (!curpath){
			curpath = gc(GC_ADD, ft_strdup(operand), GC_SUBSHELL);
		}
	}
	
	// if option -P -> 10
	char *pwd = get_variable_value(shell_vars, "PWD");
	if (!pwd)
		pwd = getcwd(NULL, PATH_MAX);
    if (curpath[0] != '/') {

		char buffer[MAX_WORD_LEN] = {0};
		if (pwd && !last_elem_is(pwd, '/')){
			if (ft_snprintf(buffer, MAX_WORD_LEN, "%s/%s", pwd, curpath) == -1)
				_fatal("cd: FATAL", 1);
		} else{
			if (ft_snprintf(buffer, MAX_WORD_LEN, "%s%s", pwd, curpath) == -1)
				_fatal("cd: FATAL", 1);
		}
		
		curpath = gc(GC_ADD, ft_strdup(buffer), GC_SUBSHELL);
    }

	canonical_convertion(&curpath);

	if (options & CD_P) {
		char resolved_path[PATH_MAX];
		if (!realpath(curpath, resolved_path)) {
			print_cd_error(ERROR_NO_SUCH_FILE, (void **)&curpath);
			g_exitno = 1;
			return;
		}
		curpath = gc(GC_ADD, ft_strdup(resolved_path), GC_SUBSHELL);
	}

	if (cd_status){
		print_cd_error(cd_status, (void **)&operand);
		g_exitno = 1;
		return;
	}
	
	char *new_pwd = gc(GC_ADD, ft_strjoin("PWD=", curpath), GC_ENV);
	char *new_oldpwd = gc(GC_ADD, ft_strjoin("OLDPWD=", pwd), GC_ENV);
	if (ft_strlen(curpath) > PATH_MAX && ft_strlen(operand) < PATH_MAX){
		curpath += (last_elem_is(pwd, '/')) ? ft_strlen(pwd) : ft_strlen(pwd) + 1;
	}

	if (chdir(curpath) == -1){
		_fatal("cd: Fatal", 1);
	}

	// printf("curpath: %s\n", curpath);
	string_list_update(shell_vars->env, new_pwd);
	string_list_update(shell_vars->env, new_oldpwd);
	//is valid
	g_exitno = 0;
	return;
}
