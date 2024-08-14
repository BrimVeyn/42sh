/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 10:15:50 by nbardavi          #+#    #+#             */
/*   Updated: 2024/02/08 13:52:25 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# include <stddef.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

// definitions

# define SIZE_MAX (18446744073709551615UL)

// definitions de structures

typedef struct s_list
{
	char			*content;
	struct s_list	*next;
}					t_list;

// Fonctions de manipulation de caractères
char				ft_toupper(int c);
char				ft_tolower(int c);
int					ft_isprint(char c);
int					ft_isdigit(int c);
int					ft_isascii(int c);
int					ft_isalpha(int c);
int					ft_isalnum(int c);

// Fonctions de manipulation de chaînes de caractères
char				*ft_strrchr(const char *string, int searchedChar);
size_t				ft_strlen(const char *theString);
char				*ft_strchr(const char *string, int searchedChar);
size_t				ft_strlcpy(char *dst, const char *src, size_t size);
size_t				ft_strlcat(char *dst, const char *src, size_t size);
char				*ft_strnstr(const char *big, const char *little,
						size_t len);
char				*ft_strdup(const char *source);
char				*ft_substr(char const *s, unsigned int start, size_t len);
char				*ft_strjoin(char const *s1, char const *s2);
char				*ft_strtrim(char const *s1, char const *set);
char				**ft_split(char const *s, char c);
char				*ft_itoa(int n);
char				*ft_litoa(long n);
char				*ft_strmapi(char const *s, char (*f)(unsigned int, char));
void				ft_striteri(char *s, void (*f)(unsigned int, char *));
int					ft_strncmp(const char *first, const char *second,
						size_t length);

// Fonctions de manipulation de mémoire
void				*ft_memset(void *pointer, int value, size_t count);
void				*ft_memchr(const void *memoryBlock, int searchedChar,
						size_t size);
void				*ft_memmove(void *destination, const void *source,
						size_t size);
void				*ft_memcpy(void *dst, const void *src, size_t n);

int					ft_memcmp(const void *pointer1, const void *pointer2,
						size_t size);
void				*ft_calloc(size_t elementCount, size_t elementSize);

// Fonctions de manipulation de listes
t_list				*ft_lstnew(char *content);
void				ft_lstadd_front(t_list **lst, t_list *new);
int					ft_lstsize(t_list *lst);
t_list				*ft_lstlast(t_list *lst);
void				ft_lstadd_back(t_list **lst, t_list *new);
void				ft_lstdelone(t_list *lst, void (*del)(void *));
void				ft_lstclear(t_list **lst, void (*del)(void *));
void				ft_lstiter(t_list *lst, void (*f)(void *));
t_list				*ft_lstmap(t_list *lst, void *(*f)(void *),
						void (*del)(void *));

// Fonctions d'affichages
void				ft_putnbr_fd(int n, int fd);
void				ft_putstr_fd(char *s, int fd);
void				ft_putnbr_fd(int n, int fd);
void				ft_putendl_fd(char *s, int fd);
void				ft_putchar_fd(char c, int fd);

// Autres fonctions
void				ft_bzero(void *s, size_t n);
int					ft_atoi(const char *theString);

// Fonctions Customs

int					ft_strcmp(const char *s1, const char *s2);

#endif
