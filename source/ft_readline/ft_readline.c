/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_readline.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nbardavi <nbabardavid@gmail.com>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/08 11:26:40 by nbardavi          #+#    #+#             */
/*   Updated: 2024/10/09 15:54:39 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_readline.h"
#include "42sh.h"
#include "ft_regex.h"
#include "c_string.h"
#include "libft.h"
#include <linux/limits.h>
#include <stdio.h>
#include <termios.h>
#include <termcap.h>
#include <unistd.h>

void enable_raw_mode()
{
    struct termios raw;

    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode()
{
    struct termios raw;
    tcgetattr(STDIN_FILENO, &raw);
    raw.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void move_cursor(int x, int y) {
    char buffer[2048];
    char *area = buffer;
    char *move_cursor_seq = tgetstr("cm", &area);
	
    if (move_cursor_seq) {
        char *cursor_position = tgoto(move_cursor_seq, x, y);
		// disable_raw_mode();
		tputs(cursor_position, 1, putchar);
		fflush(stdout);
		// enable_raw_mode();
		// disable_raw_mode();
		// printf("x: %d\ny: %d\n", x, y);
		// enable_raw_mode();
    }
}

void set_cursor_pos(size_t *x, size_t *y){
    char buf[32];
    unsigned int i = 0;
	(void) y;
	(void) x;
	
	// disable_raw_mode();
    write(STDOUT_FILENO, "\033[6n", 4);

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1 || buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';
    if (buf[0] == '\033' && buf[1] == '[') {
		regex_match_t result = regex_match("\\[[0-9]*;", buf);
		char *tmp_y = ft_substr( buf, result.re_start + 1, result.re_end - result.re_start);
		*y = ft_atoi(tmp_y);
		free(tmp_y);
		
		result = regex_match(";[0-9]*$", buf);
		char *tmp_x = ft_substr( buf, result.re_start + 1, result.re_end - result.re_start);
		*x = ft_atoi(tmp_x);
		free(tmp_x);
	}
}

char *ft_readline(char *prompt){
	static bool history_defined = false;
	char c;
	(void)prompt;

	size_t cursor_x = 0;
	size_t cursor_y = 0;
	
	char buffer[2048];
	string line = STRING_L("");

	tgetent(buffer, getenv("TERM"));
	
	if (history_defined == false){
		init_history();
		history_defined = true;
	}

	// signal(SIGINT, signal_sigint_prompt);

	//place cursor and print prompt
	enable_raw_mode();
	set_cursor_pos(&cursor_x, &cursor_y);
	cursor_y--; //strange but need it
	cursor_x += ft_strlen(prompt) - 1;
	write(1, prompt, ft_strlen(prompt));
	move_cursor(cursor_x, cursor_y); //move it to syncronize the position (avoid a bug)

	while(true){
		read(STDIN_FILENO, &c, 1);

		if (c == VEOF){
			write(1, "\n", 1);
			return NULL;
		}
		//handle specials keys
		else if (c == '\033'){
			char seq[2];
			if (read(STDIN_FILENO, &seq[0], 1) == 0) break;
			if (read(STDIN_FILENO, &seq[1], 1) == 0) break;

			if (seq[0] == '['){
				if (seq[1] == 'A'){
				}
				else if (seq[1] == 'D'){
					if (cursor_x > ft_strlen(prompt)){
						move_cursor(--cursor_x, cursor_y);
					}
				}
				else if (seq[1] == 'C'){
					if (cursor_x <= (size_t)str_length(&line) + ft_strlen(prompt) - 1){
						move_cursor(++cursor_x, cursor_y);
					}
				}
				else if (seq[1] == 'B'){
				}
			}
		}
		else {
			//place new char
			if (c == '\n'){
				str_push_back(&line, '\n');
				write(1, "\033[2K\r", 5);
				write(1, prompt, ft_strlen(prompt));
				write(1, line.data, str_length(&line));
				break;
			}
			if (c == 127 && cursor_x == ft_strlen(prompt)){
				continue;
			}
			if (cursor_x - ft_strlen(prompt) == (size_t)str_length(&line))
				if (c == 127){
					str_pop_back(&line);
					cursor_x -= 2;
				}
				else
					str_push_back(&line, c);
			else
				if (c == 127){

					str_erase(&line, cursor_x - ft_strlen(prompt), 1);
					cursor_x -= 2;
				}
				else
					str_insert(&line, c, cursor_x - ft_strlen(prompt));

			write(1, "\033[2K\r", 5);
			write(1, prompt, ft_strlen(prompt));
			write(1, line.data, str_length(&line));
			cursor_x++;
			move_cursor(cursor_x, cursor_y);

		}
	}
	disable_raw_mode();
	// destroy_history();
	return line.data;
}
