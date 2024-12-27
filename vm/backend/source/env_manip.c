/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_manip.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/20 10:56:02 by nbardavi          #+#    #+#             */
/*   Updated: 2024/12/20 14:02:08 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <linux/limits.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void setup_testing_environment(const char * const base_path) {
    mkdir(base_path, 0755);

    char input_dir[PATH_MAX], output_dir[PATH_MAX], bash_output_files[PATH_MAX], sh_output_files[PATH_MAX];
    snprintf(input_dir, sizeof(input_dir), "%s/input_files", base_path);
    snprintf(output_dir, sizeof(output_dir), "%s/output_files", base_path);
    snprintf(bash_output_files, sizeof(input_dir), "%s/bash_output_files", base_path);
    snprintf(sh_output_files, sizeof(output_dir), "%s/sh_output_files", base_path);

    mkdir(input_dir, 0755);
    mkdir(output_dir, 0755);

    char filepath[PATH_MAX];
    snprintf(filepath, sizeof(filepath), "%s/normal", input_dir);
    FILE *file = fopen(filepath, "w");
    if (file) {
        fprintf(file, "Hello World!\n!dlroW olleH\n");
        fclose(file);
    }

    snprintf(filepath, sizeof(filepath), "%s/no_perms", input_dir);
    creat(filepath, 0000);

    snprintf(filepath, sizeof(filepath), "%s/no_exec", input_dir);
    creat(filepath, 0644);

    snprintf(filepath, sizeof(filepath), "%s/no_write", input_dir);
    creat(filepath, 0444);

    snprintf(filepath, sizeof(filepath), "%s/no_read", input_dir);
    creat(filepath, 0222);

    chmod(filepath, 0222);

    snprintf(filepath, sizeof(filepath), "%s/normal", output_dir);
    file = fopen(filepath, "w");
    if (file) {
        fprintf(file, "Hello World!\n!dlroW olleH\n");
        fclose(file);
    }

    snprintf(filepath, sizeof(filepath), "%s/no_perms", output_dir);
    creat(filepath, 0000);

    snprintf(filepath, sizeof(filepath), "%s/no_exec", output_dir);
    creat(filepath, 0644);

    snprintf(filepath, sizeof(filepath), "%s/no_write", output_dir);
    creat(filepath, 0444);

    snprintf(filepath, sizeof(filepath), "%s/no_read", output_dir);
    creat(filepath, 0222);
}

void remove_directory_recursively(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    char filepath[PATH_MAX];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        if (entry->d_type == DT_DIR) {
            remove_directory_recursively(filepath);
        } else {
            chmod(filepath, 0644);
            unlink(filepath);
        }
    }

    closedir(dir);
    rmdir(path);
}
