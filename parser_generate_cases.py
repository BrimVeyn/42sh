from bs4 import BeautifulSoup

c_code = ""
case_number = 0

with open("cases.c", "w") as f:
    with open("final_grammar", "r") as input:
        for line in input:
            case_number += 1
            words = line.split(' ')
            production_rule = ' '.join(words[1:]).rstrip()
            symbol = words[1].title()
            object_count = len(words) - 3

            c_code += f'case {case_number}: {{ /* {production_rule} */\n' \
                    + ('\tda_pop(stack);\n' * object_count) \
                    + f'\treduced_entry->symbol.non_terminal = {symbol};\n' \
                    + f'\treduced_entry->is_terminal = false;\n' \
                    + f'\tstate = da_peak_back(stack)->state;\n' \
                    + f'\treduced_entry->state = parsingTable[state][{symbol}].value;\n' \
                    + f'\tbreak;\n' \
                    + '}\n'
    f.write(c_code)
