/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/28 13:03:10 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/05 15:10:12 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "libft.h"

#include <sys/stat.h>
#include <unistd.h>

char *find_bin_location(char *bin, StringListL *env, bool *absolute){
	struct stat file_stat;
	if (stat(bin, &file_stat) != -1 && ft_strchr(bin, '/')) {

		if (S_ISDIR(file_stat.st_mode)) {
			dprintf(2, "%s: Is a directory\n", bin);
			g_exitno = 126;
			return (NULL);
		}
		if (file_stat.st_mode & S_IXUSR) {
			*absolute = true;
			return bin;
		}
		else {
			dprintf(2, "%s: Permission Denied\n", bin);
			g_exitno = 126;
			return (NULL);
		}
	}
	else if (stat(bin, &file_stat) == -1 && ft_strchr(bin, '/')) {
		dprintf(2, "%s: No such file or directory\n", bin);
		g_exitno = 127;
		return (NULL);
	}

	char **path = ft_split(string_list_get_value(env, "PATH"), ':');
	for (int i = 0; path[i]; i++) {
		char *bin_with_path = ft_strjoin(path[i], (char *)gc(GC_ADD, ft_strjoin("/",bin), GC_SUBSHELL));

		struct stat file_stat;
		if (stat(bin_with_path, &file_stat) != -1){
			if (file_stat.st_mode & S_IXUSR){
				free_charchar(path);
				return (char *)gc(GC_ADD, bin_with_path, GC_SUBSHELL);
			}
		}
		free(bin_with_path);
		bin_with_path = NULL;
	}
	free_charchar(path);
	dprintf(2, "%s: command not found\n", bin);
	g_exitno = 127;
	return NULL;
}

int get_highest_free_fd() {
	int highest = 0;
	for (size_t i = 0; i < g_fdSet->size; i++) {
		if (g_fdSet->data[i] >= highest) {
			highest = g_fdSet->data[i];
		}
	}
	return (highest == 0) ? 500 : highest + 1;
}

int move_to_high_fd(int fd) {
	int high_fd = get_highest_free_fd();
	if (dup2(fd, high_fd) == -1)
		fatal("dup2: fatal", 1);
	close(fd);
	return high_fd;
}

int *save_std_fds() {
	int *fds = gc(GC_CALLOC, 3, sizeof(int), GC_SUBSHELL);
	fds[0] = dup(STDIN_FILENO);
	fds[1] = dup(STDOUT_FILENO);
	fds[2] = dup(STDERR_FILENO);

	fds[0] = move_to_high_fd(fds[0]);
	da_push(g_fdSet, fds[0]);
	fds[1] = move_to_high_fd(fds[1]);
	da_push(g_fdSet, fds[1]);
	fds[2] = move_to_high_fd(fds[2]);
	da_push(g_fdSet, fds[2]);
	
	return fds;
}

void close_fd_set() {
	while (g_fdSet->size != 0) {
		close(da_pop(g_fdSet));
	}
}
