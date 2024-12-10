/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:21:56 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/10 15:12:53 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#include "final_parser.h"
#include "expansion.h"

#include <termios.h>
#include <stdbool.h>

extern IntList *g_fdSet;

//--------------Shell Infos------------------//
typedef enum { SHELL_INIT, SHELL_GET } shell_interface_mode;

typedef struct ShellInfos {
	bool		interactive;
	bool		script;
	pid_t	shell_pgid;
	int		shell_terminal;
	struct termios shell_tmodes;
} ShellInfos;

ShellInfos *shell(int mode);
//-------------------------------------------//

void	close_std_fds(void);
void	close_saved_fds(int *saved_fds);
void	close_fd_set();
void	remove_fd_set(int fd);
void	close_all_fds(void);
int		get_highest_free_fd();
int		move_to_high_fd(int fd);
int		*save_std_fds();

bool	secure_dup2(const int from, const int to);
bool	is_builtin(const char *bin);
char	*find_bin_location(char *bin, StringListL *env, bool *absolute);
bool redirect_ios(RedirectionL * const redir_list, Vars * const shell_vars);

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

void *hash_interface(hash_mode mode, char *arg, Vars *shell_vars);
char *hash_find_bin(char *bin, Vars *shell_vars);

char *job_print(AndOrP * const andor, const bool newline);

void add_vars_to_local(StringListL * const list, const StringListL * const vars);
void add_vars_to_set(const Vars * const shell_vars, const StringListL * const vars);

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
