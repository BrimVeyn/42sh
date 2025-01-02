/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:14:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/29 23:34:29 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
#define UTILS_H

#define _42SH_VERSION "0.5"
#define _42SH_SHELL "42sh"

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../libftprintf/header/libft.h"

extern int g_exitno;

#define _fatal(msg, exitno) fatal(msg, __LINE__, __FILE_NAME__, exitno);

#ifndef __FILE_NAME__

#define __FILE_NAME__ (ft_strrchr(__FILE__, '/') ? ft_strrchr(__FILE__, '/') + 1 : __FILE__)

#endif

#define __basename(path) (ft_strrchr(path, '/') ? ft_strrchr(path, '/') + 1 : path)

#define BUFFER_SIZE 1024
#define MAX_WORD_LEN 65536
#define POSIX_MAX_ID_LEN 255
#define DATE_SIZE 255

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define FREE_POINTERS(...) \
    void *ptrs[] = { __VA_ARGS__ }; \
    for (size_t i = 0; i < ARRAY_SIZE(ptrs); i++) { \
        if (ptrs[i] != NULL) { \
            free(ptrs[i]); \
        } \
    } \

//----------------Garbage-------------------//
typedef enum {
	GC_GENERAL,
	GC_READLINE,
	GC_ENV,
	GC_SUBSHELL,
	GC_ALL,
} gc_level;

typedef enum {
	GC_ALLOC,
	GC_CALLOC,
	GC_REALLOC,
	GC_ADD,
	GC_MOVE,
	GC_FREE,
	GC_CLEANUP,
	GC_RESET,
	GC_CLEAN_IDX,
	GC_GET,
} gc_mode;

#define GC_LEVELS GC_ALL

typedef struct {
	void **garbage;
	uint32_t size;
	uint32_t capacity;
} Garbage;

void *gc(gc_mode mode, ...);
//------------------------------------------//


//----------------Dynamic arrays--------------------//
typedef struct {
	char	*data;
	size_t	size;
	size_t	capacity;
	size_t	size_of_element;
	int		gc_level;
} StringStream;

//----------------StringStream------------------//
char	*ss_get_owned_slice(StringStream * const ss);
char	*ss_to_string(StringStream * const ss);
void	ss_push_string(StringStream * const ss, const char * const str);
void	ss_cut(StringStream * const ss, const size_t new_size);
char	ss_pop_front(StringStream * const ss);
void	ss_insert_string(StringStream * const ss, const char * const str, const size_t pos);
void	da_transfer(StringStream *in, StringStream *out, int number);
//------------------------------------------//

#define gc_unique(datatype, garbage_collector_level) \
	gc(GC_CALLOC, 1, sizeof(datatype), garbage_collector_level); \

//------------------------------------------//

//----------------Utils--------------------//
void			*ft_realloc(void *ptr, size_t old_size, size_t new_size, size_t element_size);
void			free_charchar(char **array);
size_t			ft_strlenlen(const char **strstr);
char			**ft_strdupdup(const char **env);
char			*replace_char_greedy(char *str, char c, char by);
int				ft_strrstr(const char *const haystack, const char *const needle);
int				ft_strstr(const char *const haystack, const char *const needle);
char			*ft_strsed(char * src, const char *occ, const char *rep);
int				ft_sprintf(char *buffer, const char *fmt, ...);
int				ft_snprintf(char *buffer, const size_t size_of_buffer, const char *fmt, ...);
void			ft_dprintf(int fd, const char *fmt, ...);
long			ft_atol(const char *str);
char			*ft_ltoa(long n);
char			*boolStr(bool rhs);
int				read_x_base(const char *const input, char *buffer, const int x, const char *const base);
//------------------------------------------//

//----------------File Utils--------------------//
char			*read_whole_file(int fd);
char			*get_line_x(char *in, const size_t n);
//------------------------------------------//

//------------------Error-----------------------//
void error(const char * const msg, const int exit_code);
void exit_clean(void);
void fatal(const char * const msg, const int line, const char *filename, const int exit_code);
//----------------------------------------------//

#endif // !UTILS_H
