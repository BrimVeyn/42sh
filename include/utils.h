/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/10 11:14:39 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/20 16:13:14 by bvan-pae         ###   ########.fr       */
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

#include "libft.h"
#define __FILE_NAME__ (ft_strrchr(__FILE__, '/') ? ft_strrchr(__FILE__, '/') + 1 : __FILE__)

#endif

#define __basename(path) (ft_strrchr(path, '/') ? ft_strrchr(path, '/') + 1 : path)

#define BUFFER_SIZE 1024
#define MAX_WORD_LEN 65536
#define POSIX_MAX_ID_LEN 255
#define DATE_SIZE 255

#define FREE_POINTERS(...) \
    void *ptrs[] = { __VA_ARGS__ }; \
    for (size_t i = 0; i < sizeof(ptrs)/sizeof(ptrs[0]); i++) { \
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

#define da_create(name, type_of_array, element_size, garbage_collector_level) \
	type_of_array *name = gc(GC_CALLOC, 1, sizeof(type_of_array), garbage_collector_level); \
	(name)->data = gc(GC_CALLOC, 10, element_size, garbage_collector_level); \
	(name)->size = 0; \
	(name)->capacity = 10; \
	(name)->size_of_element = element_size; \
	(name)->gc_level = garbage_collector_level; \

#define da_push(array, new_element) \
	do { \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, (array)->size_of_element, (array)->gc_level); \
		} \
		(array)->data[(array)->size++] = new_element; \
	} while (0); \

#define da_push_front(array, new_element) \
	do { \
		if ((array)->size >= (array)->capacity) { \
			(array)->capacity *= 2; \
			(array)->data = gc(GC_REALLOC, (array)->data, (array)->size, (array)->capacity, (array)->size_of_element, (array)->gc_level); \
		} \
		ft_memmove(&(array)->data[1], (array)->data, (array)->size_of_element * (array)->size++); \
		(array)->data[0] = new_element; \
	} while (0); \

#define da_erase_index(array, index) \
	do { \
		if (index < (array)->size)  { \
			ft_memmove(&(array)->data[index], &(array)->data[index + 1], (array)->size_of_element * ((array)->size - index - 1)); \
			(array)->data[--(array)->size] = 0; \
		} \
	} while (0); \

#define da_clear(array) \
	do { \
		ft_memset((array)->data, 0, (array)->size_of_element * (array)->size); \
		(array)->size = 0; \
	} while (0); \
	
#define da_pop(array) \
	((array)->size == 0 ? 0 : (array)->data[--(array)->size]) \

#define da_peak_back(array) \
	(array)->size == 0 ? 0 : (array)->data[(array)->size - 1] \

#define da_peak_front(array) \
	(array)->size == 0 ? 0 : (array)->data[0] \

#define da_peak_index (array, index) \
	(array)->size > index ? (array)->data[index] : 0 \

#define da_print(array) \
	_Generic((array), \
		StrList *: str_list_print, \
		StringListL *: string_list_print, \
		TokenList *: tokenListToString, \
		ExpKindList *: exp_kind_list_print, \
		ExprArray *: expr_array_print \
	)((array))

#define da_pop_front(array) \
	_Generic((array), \
		StringStream *: ss_pop_front \
	)(array)

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
