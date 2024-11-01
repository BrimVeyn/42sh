from bs4 import BeautifulSoup

# Load HTML data from file
with open("test.html", "r", encoding="utf-8") as file:
    html_data = file.read()

soup = BeautifulSoup(html_data, 'html.parser')
table = soup.find('table')
rows = table.find_all('tr')

# Initialize C code as a string
c_code = "/* HomeCooked bash LALR(1) Automaton */\n\n"

header_row = rows[2]
header_cells = header_row.find_all('th')

c_code += "typedef enum TokenType {\n"

debug_function = "char *tokenTypeStr(TokenType type) {\n"
debug_function += "\tswitch(type) {\n"

for cell in header_cells:
    value = cell.get_text(strip=True)
    if value == "$":
        value = "END"
    if value != value.upper():
        value = value.title()
    c_code += "\t" + value + ",\n"
    debug_function += "\t\tcase " + value + ": return \"" + value + "\";\n"


c_code += "} TokenType;\n\n"

debug_function += "\t}\n\treturn NULL;\n}\n\n"
c_code += debug_function

c_code += "typedef enum {SHIFT, REDUCE, GOTO, ACCEPT, ERROR} Action;\n\n"

c_code += "typedef struct TableEntry {\n\tAction action;\n\tint state;\n}\tTableEntry;\n\n"

num_state = len(rows) - 3
num_token = len(header_cells)

c_code = c_code + "static TableEntry parsingTable[" + str(num_state) + "][" + str(num_token) + "] = {\n"

for row in rows[3:]:  # Start at slice 3 (skip headers line)
    cells = row.find_all('td')
    values = []
    for cell in cells:
        if cell == cells[0]: # skip first cell (index)
            continue
        value = cell.get_text(strip=True)
        if value.isnumeric() :
            value = "{GOTO, " + value + "}"
        if len(value) and value[0] == 'r':
            value = "{REDUCE, " + value[1:] + "}"
        if len(value) and value[0] == 's':
            value = "{SHIFT, " + value[1:] + "}"
        if not len(value):
            value = "{ERROR, 0}"
        if value == "acc":
            value = "{ACCEPT, 0}"
        values.append(value)
    c_code += "    {" + ", ".join(values) + "},\n"

c_code += "};\n"

# Output to C code file
with open("automaton.c", "w") as f:
    f.write(c_code)
