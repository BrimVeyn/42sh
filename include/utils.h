/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/19 09:02:04 by nbardavi          #+#    #+#             */
/*   Updated: 2024/09/30 16:24:34 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

extern int g_exitno;

#define BUFFER_SIZE 1024
#define MAX_WORD_LEN 65536
#define MAX_FILENAME_LEN 1024
#define POSIX_MAX_ID_LEN 255

#define FREE_POINTERS(...) \
    void *ptrs[] = { __VA_ARGS__ }; \
    for (size_t i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); i++) { \
        if (ptrs[i] != NULL) { \
            free(ptrs[i]); \
        } \
    } \

typedef struct {
	int start;
	int end;
} Range;

typedef enum {
	GC_GENERAL,
	GC_SUBSHELL,
	GC_ALL,
} type_of_garbage;

typedef struct {
	void **garbage;
	uint16_t size;
	uint16_t capacity;
} Garbage;

//----------------Garbage-------------------//
Garbage			*gc_get(void);
void			gc_init(int n);
void			*gc_add(void *ptr, int n);
void			gc_cleanup(int n);
void			gc_free(void *addr, int n);
void			gc_addcharchar(char **str, int n);

//----------------Utils--------------------//
void			free_charchar(char **s);
void			*ft_realloc(void *ptr, size_t oldSize, size_t nbEl, size_t elSize);
size_t			ft_strlenlen(const char **strstr);
char			**ft_strdupdup(const char **env);
int				there_is_star(char *str);
int				there_is_slash(char *str);
char			*replace_char_greedy(char *str, char c, char by);
int				ft_strstr(char *haystack, char *needle);
int				ft_strrstr(char *haystack, char *needle);
void			ft_sprintf(char *buffer, char *fmt, ...);
void			ft_dprintf(int fd, char *fmt, ...);
long			ft_atol(const char *str);
char			*ft_ltoa(long n);

//----------------File Utils--------------------//
char			*get_next_line(int fd);
char			*read_whole_file(int fd);

#endif // !UTILS_H
