/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/20 16:01:45 by bvan-pae          #+#    #+#             */
/*   Updated: 2025/01/13 13:09:17 by nbardavi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "exec.h"
#include "final_parser.h"
#include "parser.h"
#include <stdio.h>
#define OPTION_SET 1

int manage_vi_option(int mode, bool new_state){
    static bool state = false;
    if (mode == OPTION_SET){
        state = new_state;
    }
    return state;
}

void builtin_set(__attribute__((unused)) const SimpleCommandP * command, Vars * const shell_vars) {
    char **argv = command->word_list->data;
    
    if (argv[1] && !ft_strcmp("-o", argv[1])){
        if (!ft_strcmp("vi", argv[2])){
            manage_vi_option(OPTION_SET, true);
        } else {
            dprintf(2, "42sh: set: no such option: %s\n", argv[2]);
        }
    } else if (argv[1] && ft_strcmp("+o", argv[1])){
        if (!ft_strcmp("vi", argv[2])){
            manage_vi_option(OPTION_SET, false);
        } else {
            dprintf(2, "42sh: set: no such option: %s\n", argv[2]);
        }
    } else if (!argv[1]){
        string_list_print(shell_vars->set);
    }
}
