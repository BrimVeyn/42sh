/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:18:00 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/17 13:05:21 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXEC_H
# define EXEC_H

#include "ast.h"
#include "parser.h"
#include <stdbool.h>

//------------Shell Infos------------------//
typedef enum { SHELL_INIT, SHELL_GET } shell_interface_mode;

typedef struct ShellInfos {
	bool	interactive;
	pid_t	shell_pgid;
	int		shell_terminal;
	struct termios shell_tmodes;
} ShellInfos;

ShellInfos *shell(int mode);
//---------------------------------------//
void	close_saved_fds(int *saved_fds);
void	close_std_fds(void);
void	close_all_fds(void);

int		secure_fork(void);
bool	secure_dup2(int from, int to);
void	secure_pipe2(int pipefd[2], int flags);
void	secure_execve(const char *pathname, char **const argv, char **const envp);

bool	is_builtin(const char *bin);
bool	builtin_executer(const SimpleCommand *command, Vars *shell_vars);

int		exec_node(Node *node, Vars *shell_vars, bool foreground);
void	add_vars_to_local(StringList *list, TokenList *vars);
void	add_vars_to_set(Vars *shell_vars, TokenList *vars);

char *find_bin_location(char *bin, StringList *env, bool *absolute);

//------------------------Jobs---------------------------------//

typedef struct {
	Job **data;
	size_t size;
	size_t capacity;
} JobList;

extern JobList *job_list;

JobList *job_list_init(void);
void	job_list_add(Job *j);

void	wait_for_job (Job *j);
void	update_status(void);
void	do_job_notification(void);
void	format_job_info (Job *j, const char *status);
int		mark_process_status (JobList *j, pid_t pid, int status);
void	put_job_in_foreground (Job *j, int cont);
void	put_job_in_background (Job *j);
char	*get_pipeline(Job *j);
char	*sigStr(const int sig);
int		job_is_completed(Job *j);
int		job_is_stopped(Job *j);
void	job_list_addback(Job **lst, Job *new_value);
void	job_move(Job *job);
void	job_list_remove(Job *el);
void	job_killall(void);

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

void builtin_hash(const SimpleCommand *command, Vars *shell_vars);
void builtin_set(const SimpleCommand *command, Vars *shell_vars);
void builtin_env(const SimpleCommand *command, Vars *shell_vars);
void builtin_echo(const SimpleCommand *command, Vars *shell_vars);
void builtin_exit(const SimpleCommand *command, Vars *shell_vars);
void builtin_export(const SimpleCommand *command, Vars *shell_vars);
void builtin_type(const SimpleCommand *command, Vars *shell_vars);
void builtin_pwd(const SimpleCommand *command, Vars *shell_vars);
void builtin_cd(const SimpleCommand *command, Vars *shell_vars);
void builtin_unset(const SimpleCommand *command, Vars *shell_vars);
void builtin_jobs(const SimpleCommand *command, Vars *shell_vars);
void builtin_fg(const SimpleCommand *command, Vars *shell_vars);
void builtin_bg(const SimpleCommand *command, Vars *shell_vars);
//-------------------------------------------------------------//

#endif
