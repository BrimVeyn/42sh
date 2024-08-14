/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/10 12:13:14 by bvan-pae          #+#    #+#             */
/*   Updated: 2023/12/27 17:11:39 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H

/*_.-=-._.-=-._.-=-._.-=-._.- Includes -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

# include "./libft.h"
# include <stdarg.h>
# include <unistd.h>

/*_.-=-._.-=-._.-=-._.-=-._.- Structures -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

typedef struct s_triggers
{
	int		hyphen;
	int		zero;
	int		dot;
	int		hash;
	int		space;
	int		plus;
	int		number;
	int		number1;
	int		number2;
	int		tr_nbr;
	int		tr;
	int		index;
	int		origine;
	int		fr;
	char	*s_origine;
}			t_trigger;

/*_.-=-._.-=-._.-=-._.-=-._.- FSource -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

char		*fdot(char *str, t_trigger trigger, size_t lenstr);
char		*fzero(char *str, t_trigger trigger, size_t lenstr);
char		*sdot(char *str, t_trigger trigger);

/*_.-=-._.-=-._.-=-._.-=-._.- Fexec -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

char		*exec_d(char *str, t_trigger trigger);
char		*exec_x(char *str, t_trigger trigger, char flag);
char		*exec_s(char *str, t_trigger trigger);
char		*exec_p(char *str, t_trigger trigger);
char		*exec_c(char *str, t_trigger trigger);

/*_.-=-._.-=-._.-=-._.-=-._.- CustomsFREE -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

char		*ft_substr_free(char const *s, unsigned int start, size_t len);
char		*ft_strjoin_s2(char const *s1, char const *s2);
char		*interdup(char *str);
char		*ft_strdup_free(const char *source);
char		*joinflag(int pos, char *str, char value, int taille);

/*_.-=-._.-=-._.-=-._.-=-._.- T_flags -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

t_trigger	t_flags(const char *txt, int i);
t_trigger	t_flags3(const char *txt, int i, t_trigger trigger);
t_trigger	t_flags2(const char *txt, int i, t_trigger trigger);
t_trigger	t_flags4(const char *txt, int i, t_trigger trigger);

/*_.-=-._.-=-._.-=-._.-=-._.- Tools -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

char		*ft_itoa_u(unsigned int nbr);
t_trigger	get_nbr(const char *txt, int i, t_trigger trigger);
char		*itc(int c);
int			c_flags(const char *txt, unsigned int i);

/*_.-=-._.-=-._.-=-._.-=-._.- Tools 2 -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

t_trigger	reset_struct(void);
int			ft_instr(char c, char *str);

/*_.-=-._.-=-._.-=-._.-=-._.- Hexa -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

char		*to_hexa(unsigned int nbr, char x);
char		*to_hexa_p(unsigned long long nbr);

/*_.-=-._.-=-._.-=-._.-=-._.- Main -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

int			fd_printf(int fd, const char *txt, ...);
char		*check_p(const char *txt, unsigned int i, va_list args);
int			ft_printf(const char *txt, ...);
char		*ft_sprintf(char *txt, ...);
char		*exec_flags(const char *txt, int i, va_list args);
char		*exec_flags2(const char *txt, int i, va_list args,
				t_trigger trigger);

/*_.-=-._.-=-._.-=-._.-=-._.- Free_utils -._.-=-._.-=-._.-=-._.-=-._.-=-._*/

char		*ft_strjoin_free(char const *s1, char const *s2);
char		*ft_strdup_char(char c);

#endif
