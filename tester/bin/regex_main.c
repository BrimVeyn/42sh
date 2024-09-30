/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   regex_main.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bvan-pae <bryan.vanpaemel@gmail.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/30 14:56:05 by bvan-pae          #+#    #+#             */
/*   Updated: 2024/09/30 15:06:02 by bvan-pae         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <readline/readline.h>
#include <readline/history.h>
#include "../../include/ft_regex.h"

int main(int ac, char **av) {
	(void)av; (void)ac;

    if (ac >= 2 && !strcmp(av[1], "-i")){
        char *input = NULL;
        char *string = readline("enter string\n> ");
        while((input = readline("> ")) != NULL){
            regex_test(input, string);
            add_history(input);
        }
    } else{
        char *text = "Heeello-world! 0123456789";
        regex_test("Heeello", text);
        regex_test("Hedllo", text);
        regex_test("Hi*", text);
        regex_test("He*", text);
        regex_test("He*l", text);
        regex_test("ee", text);
        regex_test("^ee", text);
        regex_test("^He", text);
        regex_test("H[A-Z]", text);
        regex_test("H[a-z]", text);
        regex_test("Heee[a-z]", text);
        regex_test("H[a-z]*", text);
        regex_test("[4-8]*", text);
        regex_test("[a-z4-8]*", "000aaaaa44444");
        regex_test("[a-z ]*", "Hello world!");
        regex_test("^[a-z ]*", "Hello world!");
        regex_test("^H[a-z-]*", "Hello-world!");
        // regex_test("^H[a-z]*\\sw", "Hello world!");
        // regex_test("\\${[A-Za-z_]*}", "echo ${PATH}");
		regex_test("\\$\\{[^\\}]*\\}", "echo ${PATH}");
		regex_test("\\$\\{[^\\}]*\\}", "echo ${}");
		regex_test("\\$\\{\\}", "${}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*\\}", "${}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*\\}", "${PATH}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*\\}", "${8PATH}");
		regex_test("\\$\\{[^\\}]*\\}", "${9PATH}");
		regex_test("\\$\\{[^\\}]*\\}", "${vide}");
		regex_test("\\$\\{[^\\}]*\\}", "${dsadas}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*\\}", "\\${PA\\${PATH}");
		regex_test("\\[.*d", "Hello World");
		regex_test(".*l$", "Hello World");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9?_\\-]*\\}", "${PWD-hello}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%].*\\}", "${PWD##/hel**lo}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%]*.*\\}", "${PWD}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%]*[^\\$]*\\}", "${PW${PWD}D}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%]*[^\\$\\}]*\\}", "${PWD}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%]*[^\\$\\}]*\\}", "echo ${PWD}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%]*[^\\$\\}]*\\}", "echo ${PWD:=/home/coucou}");
		regex_test("\\$\\{[A-Za-z_][A-Za-z0-9_]*[:#%]*[^\\$\\}]*\\}", "echo ${UNDEFINED:-NoValue}");
		regex_test("\\$\\{[^\\$]*\\}", "echo ${UNDEFINED:-${NoValue}");
		regex_test("\\$\\{[^\\$]*\\}", "echo ${DEFINED_VAR:-${NESTED_VARIABLE:-fallback}}");
		regex_test("\\$\\{[^\\$]*\\}", "echo ${UND}");
		regex_test("\\{[^\\$]*\\}", "${PWD}0123456");
		regex_test("\\{[a-z\\}]*\\}", "${sffs}dasd");
		regex_test("^A.*B.*C$", "A123B456C789B");
		regex_test("^A.?B$", "AB"); // '?' rend le match d'un caractère facultatif après 'A'
		regex_test("^A.?B$", "ACB"); // '?' match 'C', le caractère facultatif entre 'A' et 'B'
		regex_test("^A[^a-z]?B$", "A1B"); // '?' rend le match du caractère non minuscule facultatif entre 'A' et 'B'
		regex_test("^A[0-9]?B$", "AB"); // '?' rend facultatif le chiffre entre 'A' et 'B'
		regex_test("^A[0-9]?B$", "A5B"); // Match avec un chiffre facultatif entre 'A' et 'B'
		regex_test("^A.?C$", "AC"); // '?' fait que le match du caractère entre 'A' et 'C' est facultatif
		regex_test("^A.*?B.*?C$", "A123B456C789"); // Non-greedy match avec '*?' entre 'A' et 'B', puis 'B' et 'C'
		regex_test("^A.*?1?.*C$", "A12C"); // '?' après '1' rend le '1' facultatif dans la chaîne
		regex_test("^A.?B.?C$", "ABC"); // '?' rend facultatif le match entre 'A' et 'B', ainsi qu'entre 'B' et 'C'
		regex_test("a+", "aaa");
		regex_test("a+", "a");
		regex_test("a+", "");
		regex_test("a+b+", "aaabbb");
		regex_test("a+b+", "ab");
		regex_test("a+b+", "bb");
		regex_test("a+b+", "aab");
		regex_test("a+z+", "aaazz");
		regex_test("[a-z]+", "abcd");
		regex_test("[a-z]+", "1234");
		regex_test("a*", "aaa");
		regex_test("a*", "a");
		regex_test("a*", "");
		regex_test("a*b*", "aaabbb");
		regex_test("a*b*", "ab");
		regex_test("a*b*", "bb");
		regex_test("a*b*", "aab");
		regex_test("a*z*", "aaazz");
		regex_test("[a-z]*", "abcd");
		regex_test("[a-z]*", "1234");
		regex_test("\\$\\{#?[A-Za-z_][A-Za-z0-9_]*[#%:]?[^\\}%:#]+\\}", "${VAR:}");
		regex_test("\\$\\{#?[A-Za-z_][A-Za-z0-9_]*[#%:]?[^\\}%:#]+\\}", "echo ${DEFINED_VAR:-${NESTED_VARIABLE:-salut}}");
		// regex_test("^[^a-z]*[A-Z]*[^0-9]*$", "!@#$HELLO!!");
		// regex_test("^[A-Z][a-z]*[^A-Z]*[A-Z]*[^0-9]*$", "HelloWORLD!!");
		// regex_test("^[A-Za-z]*[^A-Z]*[A-Z]*[^0-9]*$", "hello!!WORLD!!");
		// regex_test("^[^a-zA-Z]*[a-z]*[^A-Z]*$", "!@#test!!");
		// regex_test("^[A-Z][^A-Z]*[A-Z]*[^a-z]*$", "H!@#WORLD");
		// regex_test("^[^A-Za-z]*[A-Za-z]*[^0-9]*$", "!@#$Test123!!");
		// regex_test("^[A-Za-z]*[^a-zA-Z]*[0-9]*[^]*$", "Test!!123!!");
		// regex_test("^[A-Z]*[^A-Z]*[A-Za-z]*[^0-9]*$", "HELLO!!world");
		// regex_test("^[^a-z]*[a-z]*[^A-Z]*[A-Z]*$", "!@#$test!!WORLD");
		// regex_test("^[A-Z][^A-Z]*[a-z]*[^0-9]*[A-Z]*$", "H!@#ello!!WORLD");
		// regex_test("^[^]*[A-Z]*[^a-z]*[a-z]*$", "!@#HELLO!!world");
		// regex_test("^[A-Za-z]*[^0-9]*[0-9]*[^A-Z]*$", "test123!!hello");
		// regex_test("^[^A-Z]*[A-Z]*[^a-z]*[a-z]*[^]*$", "!@#$WORLD!!test!!");
		// regex_test("^[A-Z][a-z]*[^A-Z]*[A-Z]*[^]*$", "Hello!!WORLD!!");
		// regex_test("^[^]*[A-Za-z]*[^]*[0-9]*$", "!@#$Test123!!");
		// regex_test("^[A-Z]*[^A-Z]*[A-Z]*[^a-z]*[a-z]*$", "HELLO!!world");
		// regex_test("^[^a-z]*[a-z]*[^0-9]*[A-Z]*[^]*$", "!@#$test123!!WORLD");
		// regex_test("^[^A-Z]*[A-Za-z]*[^0-9]*[0-9]*[^]*$", "hello123!!");
		// regex_test("^[A-Z][^A-Z]*[a-z]*[^A-Z]*[A-Z]*$", "H!@#ello!!WORLD");
		// regex_test("^[^A-Za-z]*[A-Z]*[^a-z]*[a-z]*[^0-9]*$", "!@#HELLO!!world!!");
    }
    return 0;
}

