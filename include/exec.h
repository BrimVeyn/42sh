/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/15 14:44:54 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/11/25 17:29:42 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#include "lexer.h"

#include <termios.h>
#include <stdbool.h>
//--------------Shell Infos------------------//
typedef enum { SHELL_INIT, SHELL_GET } shell_interface_mode;

typedef struct ShellInfos {
	int		interactive;
	pid_t	shell_pgid;
	int		shell_terminal;
	struct termios shell_tmodes;
} ShellInfos;

ShellInfos *shell(int mode);
//-------------------------------------------//

void	close_saved_fds(int *saved_fds);
void	close_std_fds(void);
void	close_all_fds(void);

int		secure_fork(void);
bool	secure_dup2(int from, int to);
void	secure_pipe2(int pipefd[2], int flags);
void	secure_execve(const char *pathname, char **const argv, char **const envp);
bool	is_builtin(const char *bin);
char	*find_bin_location(char *bin, StringList *env, bool *absolute);

char	*sigStr(const int sig);
//-------------------------------------------------------------//

//------------------------Builtins-----------------------------//
#define TABLE_SIZE 1000

typedef struct Entry {
	int	 hits;
	char *command;
	char *bin;
	struct Entry *next; //to handle collisions
} Entry;

typedef enum {
	HASH_ADD_USED,
	HASH_ADD_UNUSED,
	HASH_FIND,
	HASH_REMOVE,
	HASH_CLEAR,
	HASH_PRINT,
	HASH_GET,
} hash_mode;

typedef struct cdOpt {
	int L;
	int P;
} cdOpt;


void *hash_interface(hash_mode mode, char *arg, Vars *shell_vars);
char *hash_find_bin(char *bin, Vars *shell_vars);

#include "final_parser.h"

void add_vars_to_local(StringList * const list, const StringListL * const vars);
void add_vars_to_set(const Vars * const shell_vars, const StringListL * const vars);
char *get_variable_value(Vars *shell_vars, char *name);

void builtin_hash(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_set(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_env(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_echo(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_exit(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_export(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_type(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_pwd(const SimpleCommandP *command, Vars * const shell_vars);
void builtin_cd (const SimpleCommandP *command, Vars * const shell_vars);
void builtin_unset (const SimpleCommandP *command, Vars * const shell_vars);
void builtin_jobs (const SimpleCommandP *command, Vars * const shell_vars);
void builtin_fg (const SimpleCommandP *command, Vars * const shell_vars);
void builtin_bg (const SimpleCommandP *command, Vars * const shell_vars);
void builtin_fc (const SimpleCommandP *command, Vars * const shell_vars);
//---------------------------------------------------------------------//


#endif
