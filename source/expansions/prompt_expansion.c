/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt_expansion.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 10:13:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/30 19:04:50 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ft_regex.h"
#include "jobs.h"
#include "utils.h"
#include "dynamic_arrays.h"
#include "libft.h"

#include <pwd.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

/* ----------------------- Bash prompt special characters --------------------------
\a A bell character. 
\d The date, in "Weekday Month Date" format (e.g., "Tue May 26"). 
\D{format} The format is passed to strftime(3) and the result is inserted into the prompt string; an empty format results in a locale-specific time representation. The braces are required. 
\e An escape character. 
\h The hostname, up to the first ‘.’. 
\H The hostname. 
\j The number of jobs currently managed by the shell. 
\l The basename of the shell’s terminal device name. 
\n A newline. 
\r A carriage return. 
\s The name of the shell, the basename of $0 (the portion following the final slash). 
\t The time, in 24-hour HH:MM:SS format. 
\T The time, in 12-hour HH:MM:SS format. 
\@ The time, in 12-hour am/pm format. 
\A The time, in 24-hour HH:MM format. 
\u The username of the current user. 
\v The version of Bash (e.g., 2.00) 
\V The release of Bash, version + patchlevel (e.g., 2.00.0) 
\w The value of the PWD shell variable ($PWD), with $HOME abbreviated with a tilde (uses the $PROMPT_DIRTRIM variable). 
\W The basename of $PWD, with $HOME abbreviated with a tilde. 
\! The history number of this command. 
\# The command number of this command. 
\$ If the effective uid is 0, #, otherwise $. 
\nnn The character whose ASCII code is the octal value nnn. 
\\ A backslash. 
\[ Begin a sequence of non-printing characters. This could be used to embed a terminal control sequence into the prompt. 
\] End a sequence of non-printing characters. 
----------------------------------------------------------------------------------- */

static char *get_date_format(const char *restrict format) {
	char date_str[DATE_SIZE] = {0};
	time_t current_time;
	struct tm *time_info;
	
	time(&current_time);
	time_info = localtime(&current_time);

	if (strftime(date_str, sizeof(date_str), format, time_info) <= 0)
		_fatal("strftime: buffer overflow", 1);

	return gc(GC_ADD, ft_strdup(date_str), GC_SUBSHELL);
}

static char *get_pwd_basename(void) {
	char *pwd = getcwd(NULL, 0);
	if (!pwd)
		return "";

	gc(GC_ADD, pwd, GC_SUBSHELL);
	char *pwd_basename = __basename(pwd);

	return pwd_basename;
}

static char *get_full_pwd(void) {
    struct passwd *pw;
    uid_t uid = getuid();

    pw = getpwuid(uid);
    if (!pw)
		return "";


	char *pwd = getcwd(NULL, 0);
	if (!pwd)
		return "";

	int match = ft_strstr(pwd, pw->pw_dir);
	if (match != -1) {
		char *ret = ft_strsed(pwd, pw->pw_dir, "~");
		free(pwd);
		return gc(GC_ADD, ret, GC_SUBSHELL);
    }

	return gc(GC_ADD, pwd, GC_SUBSHELL);
}

static char *get_username(void) {
    struct passwd *pwd;
    uid_t uid = getuid();

    pwd = getpwuid(uid);
    if (!pwd)
		return "";

	return pwd->pw_name;
}

static char *get_cut_hostname(void) {
	char hostname[100] = {0};

	if (gethostname(hostname, sizeof(hostname)) < 0)
		_fatal("gethostname: buffer overflow", 1);

	int dot_pos = ft_strstr(hostname, ".");
	if (dot_pos != -1) {
		hostname[dot_pos] = 0;
	}

	return gc(GC_ADD, ft_strdup(hostname), GC_SUBSHELL);
}

static char *get_full_hostname(void) {
	char hostname[100] = {0};

	if (gethostname(hostname, sizeof(hostname)) < 0)
		_fatal("gethostname: buffer overflow", 1);

	return gc(GC_ADD, ft_strdup(hostname), GC_SUBSHELL);
}

static char *get_date_letter(void) { return get_date_format("%a %b %d"); }
static char *get_time_24hour_secondless(void) { return get_date_format("%H:%M"); }
static char *get_time_12hour_ampm(void) { return get_date_format("%I:%M %p"); }
static char *get_time_24hour(void) { return get_date_format("%H:%M:%S"); }
static char *get_time_12hour(void) { return get_date_format("%I:%M:%S"); }
static char *get_cr(void) { return "\r"; }
static char *get_newline(void) { return "\n"; }
static char *get_backslash(void) { return "\\"; }
static char *get_version(void) { return _42SH_VERSION; }
static char *get_shell_name(void) { return _42SH_SHELL; }
static char *get_basename(void) { return __basename(ttyname(shell(SHELL_GET)->shell_terminal)); }
static char *get_job_number(void) { return gc(GC_ADD, ft_itoa(g_jobList->size), GC_SUBSHELL); }
static char *get_uid(void) { return (getuid() == 0) ? "#" : "$"; }

//TODO: p'tit crack
char *get_command_number(void) { return "1"; } //history file command number 
char *get_history_number(void) { return "12"; } //from invocation command get_history_number

static char *expand_prompt_special(const char *ps) {

	char *(*special[255])(void) = {
		['d'] = get_date_letter, ['j'] = get_job_number,
		['h'] = get_cut_hostname, ['H'] = get_full_hostname,
		['n'] = get_newline, ['r'] = get_cr,
		['s'] = get_shell_name, ['t'] = get_time_24hour,
		['T'] = get_time_12hour, ['@'] = get_time_12hour_ampm,
		['A'] = get_time_24hour_secondless, ['u'] = get_username,
		['v'] = get_version, ['V'] = get_version,
		['w'] = get_full_pwd, ['W'] = get_pwd_basename,
		['!'] = get_history_number, ['#'] = get_command_number,
		['$'] = get_uid, ['\\'] = get_backslash,
		['l'] = get_basename,
	};

	//missing \D{format} | \e | \[...\]

	da_create(ss, StringStream, sizeof(char), GC_SUBSHELL);

	while (*ps) {
		if (*ps == '\\') {
			if (special[(size_t)*(ps + 1)] != NULL) {
				ss_push_string(ss, special[(size_t)*(ps + 1)]());
				++ps;
            } else if (*(ps + 1) == 'D') {
				if (regex_match("\\D{.*}", (char *)ps).is_found) {
					ps += 3; // \ + D + {
					const char *const format_end = ft_strchr(ps, '}');
					char *const format = ft_substr(ps, 0, format_end - ps);
					ps = format_end; // }|...
					ss_push_string(ss, get_date_format(format));
					free(format);
				}
			} else if (*(ps + 1) == '[') {
				ps += 2; // \ + [;
				const size_t format_end = ft_strstr(ps, "\\]");
				char *const sequence = ft_substr(ps, 0, format_end);
				char *const res = ft_strsed(sequence, "\\e", "\033");

				ss_push_string(ss, res);
				ps += (format_end + 1);
				FREE_POINTERS(sequence, res);
			} else if (*(ps + 1) >= '0' && *(ps + 1) <= '7') {
				ps++; //skip '\'
				char buffer[4] = {0};
				int bytes_read = read_x_base(ps, buffer, 3, "01234567");
				ps += (bytes_read - 1);
				uint64_t ret = ft_atoi_base(buffer, "01234567");
				da_push(ss, ret);
			}
		} else { da_push(ss, *ps); }
		ps++;
	}

	return ss_get_owned_slice(ss);
}

char *prompt_expansion(char *prompt, Vars *shell_vars) {
	//put prompt in double quotes so it doesn't remove '\'
	char buffer[MAX_WORD_LEN] = {0};
	ft_sprintf(buffer, "\"%s\"", prompt);

	int error = 0;
	StringList *maybe_prompt = do_expansions_word(buffer, &error, shell_vars, O_NONE);
	if (!maybe_prompt) prompt = "";
	else prompt = maybe_prompt->data[0];

	prompt	= expand_prompt_special(prompt);
	return prompt;
}
