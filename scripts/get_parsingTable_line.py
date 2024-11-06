# from bs4 import BeautifulSoup
#
# # Load HTML data from file
# with open("test.html", "r", encoding="utf-8") as file:
#     html_data = file.read()
#
# soup = BeautifulSoup(html_data, 'html.parser')
# table = soup.find('table')
# rows = table.find_all('tr')
#
# header_row = rows[2]
# header_cells = header_row.find_all('th')
#
# line_no = 74
#
# desired_line = rows[2 + line_no]
#
# header_list = [cell.get_text(strip=True) for cell in header_cells]
#
# print(header_list)

from bs4 import BeautifulSoup
from prettytable import PrettyTable

# Load HTML data from file
with open("table.html", "r", encoding="utf-8") as file:
    html_data = file.read()

# Parse HTML
soup = BeautifulSoup(html_data, 'html.parser')
table = soup.find('table')
rows = table.find_all('tr')

# Retrieve headers and data rows
header_row = rows[2]
header_cells = header_row.find_all('th')
header_list = [cell.get_text(strip=True) for cell in header_cells]

# Retrieve the desired line based on the line number
line_no = int(input())
desired_line = rows[3 + line_no]
data_cells = desired_line.find_all('td')

# Map headers to values in a 2D array format
table_data = ([header, cell.get_text(strip=True)] for header, cell in zip(header_list, data_cells[1:]))

# Pretty print the table data
pretty_table = PrettyTable()
pretty_table.field_names = ["Header", "Value"]

for row in table_data:
    pretty_table.add_row(row)

print(pretty_table)
