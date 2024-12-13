/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/05 15:21:56 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/13 13:16:12 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#include "final_parser.h"
#include "expansion.h"

#include <termios.h>
#include <stdbool.h>

//DA of fds to keep precisely keep track of fds in forks
extern IntList *g_fdSet;

//Needed for the return keyword to exit from a function at anytime
extern int g_functionCtx;

#define NO_WAIT 0x0
#define WAIT	0x1
#define IS_CHILD(pid) ((pid == 0) ? true : false)

typedef enum {
	E_RETURN = (1 << 9),
	E_BREAK = (1 << 10),
	E_CONTINUE = (1 << 11),
} BuiltinExitStatus;

//--------------Shell Infos------------------//
typedef enum { SHELL_INIT, SHELL_GET } shell_interface_mode;

typedef struct ShellInfos {
	struct termios shell_tmodes;
	int		shell_terminal;
	pid_t	shell_pgid;
	bool		interactive;
	bool		script;
} ShellInfos;

ShellInfos *shell(int mode);
//-------------------------------------------//

#define O_FORKED 0x1
#define O_NOFORK 0x0

#define SAVE_FD (flag == O_NOFORK && command->redir_list)

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
bool	redirect_ios(RedirectionL * const redir_list, Vars * const shell_vars);

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

void builtin_hash(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_set(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_env(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_echo(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_exit(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_export(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_type(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_pwd(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_cd(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_unset(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_jobs(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_fg(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_bg(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_fc(const SimpleCommandP *command, Vars *const shell_vars);
void builtin_return(const SimpleCommandP *command, Vars *const shell_vars);
//---------------------------------------------------------------------//


#endif
