/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PSs_expansion.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/12 10:13:53 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/12/12 17:35:09 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "ft_regex.h"
#include "jobs.h"
#include "utils.h"
#include "libft.h"

#include <pwd.h>
#include <stddef.h>
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

char *get_ascii(void) {
	return "pffff";
}

char *get_date_format(const char *restrict format) {
	char date_str[DATE_SIZE] = {0};
	time_t current_time;
	struct tm *time_info;
	
	time(&current_time);
	time_info = localtime(&current_time);

	if (strftime(date_str, sizeof(date_str), format, time_info) <= 0)
		_fatal("strftime: buffer overflow", 1);

	return gc(GC_ADD, ft_strdup(date_str), GC_SUBSHELL);
}

char *get_pwd_basename(void) {
	char *pwd = getcwd(NULL, 0);
	if (!pwd)
		return "";

	gc(GC_ADD, pwd, GC_SUBSHELL);
	char *pwd_basename = __basename(pwd);

	return pwd_basename;
}

char *get_full_pwd(void) {
    struct passwd *pw;
    uid_t uid = getuid();

    pw = getpwuid(uid);
    if (!pw)
		return "";

	char *pwd = getcwd(NULL, 0);
	if (!pwd)
		return "";

	return gc(GC_ADD, pwd, GC_SUBSHELL);
}

char *get_username(void) {
    struct passwd *pwd;
    uid_t uid = getuid();

    pwd = getpwuid(uid);
    if (!pwd)
		return "";

	return pwd->pw_name;
}

char *get_cut_hostname(void) {
	char hostname[100] = {0};

	if (gethostname(hostname, sizeof(hostname)) < 0)
		_fatal("gethostname: buffer overflow", 1);

	int dot_pos = ft_strstr(hostname, ".");
	if (dot_pos != -1) {
		hostname[dot_pos] = 0;
	}

	return gc(GC_ADD, ft_strdup(hostname), GC_SUBSHELL);
}

char *get_full_hostname(void) {
	char hostname[100] = {0};

	if (gethostname(hostname, sizeof(hostname)) < 0)
		_fatal("gethostname: buffer overflow", 1);

	return gc(GC_ADD, ft_strdup(hostname), GC_SUBSHELL);
}

char *get_date_letter(void) { return get_date_format("%a %b %d"); }
char *get_time_24hour_secondless(void) { return get_date_format("%H:%M"); }
char *get_time_12hour_ampm(void) { return get_date_format("%I:%M %p"); }
char *get_time_24hour(void) { return get_date_format("%H:%M:%S"); }
char *get_time_12hour(void) { return get_date_format("%I:%M:%S"); }
char *get_cr(void) { return "\r"; }
char *get_newline(void) { return "\n"; }
char *get_backslash(void) { return "\\"; }
char *get_version(void) { return _42SH_VERSION; }
char *get_shell_name(void) { return _42SH_SHELL; }
char *get_basename(void) { return __basename(ttyname(shell(SHELL_GET)->shell_terminal)); }
char *get_job_number(void) { return gc(GC_ADD, ft_itoa(g_jobList->size), GC_SUBSHELL); }
char *get_uid(void) { return gc(GC_ADD, ft_itoa(getuid()), GC_SUBSHELL); }
char *get_escape_sequence(void) { return "\033"; }

//TODO: p'tit crack
char *get_command_number(void) { return "1"; } //history file command number 
char *get_history_number(void) { return "12"; } //from invocation command get_history_number

char *expand_prompt_special(const char *ps) {

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
					const char *const fend = ft_strchr(ps, '}');
					const char *const format = ft_substr(ps, 0, fend - ps);
					ps = fend; // }|...
					ss_push_string(ss, get_date_format(format));
				}
			} else if (*(ps + 1) == '[') {
				ps += 2; // \ + [;
				const size_t fend = ft_strstr(ps, "\\]");
				char *const sequence = ft_substr(ps, 0, fend);

				// dprintf(2, "sequence: {%s}\n", sequence);
				char *const res = ft_strsed(sequence, "\\e", "\033");
				// dprintf(2, "res: %s\n", res);
				ss_push_string(ss, res);
				ps += (fend + 1);
			}
		} else { da_push(ss, *ps); }
		ps++;
	}

	return ss_get_owned_slice(ss);
}
