/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io_redirect.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 15:41:47 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/27 11:16:00 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "dynamic_arrays.h"
#include "expansion.h"
#include "final_parser.h"
#include "utils.h"
#include "libft.h"
#include "lexer.h"

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_FD 1024
#include <sys/stat.h>

typedef enum {
    F_ISDIR = 0b000001, // Interdire si c'est un dossier
    F_EXEC  = 0b000010, // Vérifier bit d'exécution
    F_EXIST = 0b000100, // Doit exister
    F_WRITE = 0b001000, // Vérifier bit d'écriture
    F_READ  = 0b010000, // Vérifier bit de lecture
    F_POLY  = 0b100000, // Peut être un nombre (renvoi de fd)
} FilepathMode;

bool check_filepath(char *file_path, FilepathMode mode)
{
    if ((mode & F_POLY) && is_number(file_path))
        return true;

    struct stat file_stat;
    if (stat(file_path, &file_stat) == 0)
    {
        if ((mode & F_ISDIR) && S_ISDIR(file_stat.st_mode))
        {
            fprintf(stderr, "42sh: %s: Is a directory\n", file_path);
            g_exitno = 1;
            return false;
        }
        if (((mode & F_READ)  && !(file_stat.st_mode & S_IRUSR)) ||
            ((mode & F_WRITE) && !(file_stat.st_mode & S_IWUSR)) ||
            ((mode & F_EXEC)  && !(file_stat.st_mode & S_IXUSR)))
        {
            fprintf(stderr, "42sh: %s: Permission denied\n", file_path);
            g_exitno = 1;
            return false;
        }
        return true;
    }
    else
    {
        if (mode & F_EXIST)
        {
            fprintf(stderr, "42sh: %s: No such file or directory\n", file_path);
            g_exitno = 1;
            return false;
        }
    }
    return true;
}

int get_filepath_mode(TokenType type)
{
    if (type == LESS)          // <
        return F_EXIST | F_READ | F_ISDIR;
    else if (type == GREAT     // >
          || type == DGREAT    // >>
          || type == CLOBBER)  // >|
        return F_WRITE | F_ISDIR;
    else if (type == LESSGREAT) // <>
        return F_WRITE | F_READ | F_ISDIR;
    else if (type == GREATAND)   // >&
        return F_WRITE | F_ISDIR | F_POLY;
    else if (type == LESSAND)    // <&
        return F_READ | F_EXIST | F_ISDIR | F_POLY;
    return 0;
}

static void expand_filenames(RedirectionL *const redir_list, Vars *const shell_vars)
{
    da_create(tmp_redirs, StringList, sizeof(char *), GC_SUBSHELL);
    for (size_t i = 0; i < redir_list->size; i++)
        da_push(tmp_redirs, redir_list->data[i]->filename);

    ExpReturn ret = do_expansions(tmp_redirs, shell_vars, O_NONE);
    if (ret.error != 0)
        return;

    StringList *result = ret.ret;
    for (size_t i = 0; i < redir_list->size; i++)
    {
        redir_list->data[i]->filename = result->data[i];
    }
}

bool redirect_ios(RedirectionL * const redir_list, Vars * const shell_vars)
{
    if (!redir_list)
        return true;

    expand_filenames(redir_list, shell_vars);

    for (size_t i = 0; i < redir_list->size; i++)
    {
        const RedirectionP *redir = redir_list->data[i];
        int io_number = (redir->io_number) ? ft_atoi(redir->io_number) : -1;

        if (io_number >= MAX_FD)
        {
            g_exitno = 1;
            fprintf(stderr, "42sh: %d: Bad file descriptor\n", io_number);
            return false;
        }

        int mode = get_filepath_mode(redir->type);

        bool special_close =
            ((redir->type == LESSAND || redir->type == GREATAND)
              && strcmp(redir->filename, "-") == 0);

        if (!special_close)
        {
            if (!check_filepath(redir->filename, mode))
                return false;
        }

        if (redir->type == GREAT || redir->type == CLOBBER)
        {
            int fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
            if (fd < 0)
            {
                perror("open");
                g_exitno = 1;
                return false;
            }
            int dup_to = (io_number == -1) ? STDOUT_FILENO : io_number;
            if (!secure_dup2(fd, dup_to))
                return (g_exitno = 1, false);
            close(fd);
        }
        else if (redir->type == DGREAT)
        {
            int fd = open(redir->filename, O_WRONLY | O_CREAT | O_APPEND, 0664);
            if (fd < 0)
            {
                perror("open");
                g_exitno = 1;
                return false;
            }
            int dup_to = (io_number == -1) ? STDOUT_FILENO : io_number;
            if (!secure_dup2(fd, dup_to))
                return (g_exitno = 1, false);
            close(fd);
        }
        else if (redir->type == LESSGREAT)
        {
            int fd = open(redir->filename, O_RDWR | O_CREAT, 0664);
            if (fd < 0)
            {
                perror("open");
                g_exitno = 1;
                return false;
            }
            int dup_to = (io_number == -1) ? STDIN_FILENO : io_number;
            if (!secure_dup2(fd, dup_to))
                return (g_exitno = 1, false);
            close(fd);
        }
        else if (redir->type == LESS)
        {
            int fd = open(redir->filename, O_RDONLY);
            if (fd < 0)
            {
                perror("open");
                g_exitno = 1;
                return false;
            }
            int dup_to = (io_number == -1) ? STDIN_FILENO : io_number;
            if (!secure_dup2(fd, dup_to))
                return (g_exitno = 1, false);
            close(fd);
        }
        else if (redir->type == LESSAND)
        {
            int n = (io_number == -1) ? STDIN_FILENO : io_number;
            if (strcmp(redir->filename, "-") == 0)
            {
                close(n);
            }
            else
            {
                int fd = ft_atoi(redir->filename);
                if (fd < 0 || fcntl(fd, F_GETFD) == -1)
                {
                    fprintf(stderr, "42sh: %d: Bad file descriptor\n", fd);
                    g_exitno = 1;
                    return false;
                }
                if (fd != n)
                {
                    if (!secure_dup2(fd, n))
                        return (g_exitno = 1, false);
                }
            }
        }
        else if (redir->type == GREATAND)
        {
            int n = (io_number == -1) ? STDOUT_FILENO : io_number;
            if (strcmp(redir->filename, "-") == 0)
            {
                close(n);
            }
            else if (is_number(redir->filename))
            {
                int fd = ft_atoi(redir->filename);
                if (fd < 0 || fcntl(fd, F_GETFD) == -1)
                {
                    fprintf(stderr, "42sh: %d: Bad file descriptor\n", fd);
                    g_exitno = 1;
                    return false;
                }
                if (fd != n)
                {
                    if (!secure_dup2(fd, n))
                        return (g_exitno = 1, false);
                }
            }
            else
            {
                int fd = open(redir->filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
                if (fd < 0)
                {
                    perror("open");
                    g_exitno = 1;
                    return false;
                }
                if (!secure_dup2(fd, n))
                    return (g_exitno = 1, false);
                close(fd);
            }
        }
    }
    return true;
}
