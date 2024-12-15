import json

def generate_html(json_data):
    html_content = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Test Results</title>
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; }
        .category { margin-bottom: 20px; }
        .test { display: none; margin-left: 20px; }
        .details { display: none; margin-left: 40px; }
        button { margin-right: 10px; }
    </style>
    <script>
        function toggleVisibility(id) {
            const element = document.getElementById(id);
            element.style.display = element.style.display === 'none' ? 'block' : 'none';
        }
    </script>
</head>
<body>
    <h1>Test Results</h1>
"""

    for category in json_data:
        html_content += f"<div class=\"category\">\n"
        html_content += f"  <h2>{category['category_name']}</h2>\n"
        html_content += f"  <button onclick=\"toggleVisibility('{category['category_name']}-tests')\">Show/Hide Tests</button>\n"
        html_content += f"  <div id=\"{category['category_name']}-tests\" class=\"tests\" style=\"display: none;\">\n"

        for test in category['tests']:
            test_id = test['id']
            html_content += f"    <div class=\"test\">\n"
            html_content += f"      <p>Test ID: {test_id}</p>\n"
            html_content += f"      <button onclick=\"toggleVisibility('{test_id}-details')\">Show/Hide Details</button>\n"
            html_content += f"      <div id=\"{test_id}-details\" class=\"details\">\n"
            html_content += f"        <button onclick=\"toggleVisibility('{test_id}-input')\">Input</button>\n"
            html_content += f"        <button onclick=\"toggleVisibility('{test_id}-output')\">Output</button>\n"
            html_content += f"        <button onclick=\"toggleVisibility('{test_id}-error')\">Error</button>\n"
            html_content += f"        <button onclick=\"toggleVisibility('{test_id}-exit-code')\">Exit Code</button>\n"

            html_content += f"        <div id=\"{test_id}-input\" style=\"display: none;\">\n"
            html_content += f"          <p><strong>Input:</strong> <a href=\"{test['input']}\" target=\"_blank\">{test['input']}</a></p>\n"
            html_content += f"        </div>\n"

            html_content += f"        <div id=\"{test_id}-output\" style=\"display: none;\">\n"
            html_content += f"          <p><strong>42sh Output:</strong> <a href=\"{test['42sh_output']}\" target=\"_blank\">{test['42sh_output']}</a></p>\n"
            html_content += f"          <p><strong>Bash Output:</strong> <a href=\"{test['bash_output']}\" target=\"_blank\">{test['bash_output']}</a></p>\n"
            html_content += f"        </div>\n"

            html_content += f"        <div id=\"{test_id}-error\" style=\"display: none;\">\n"
            html_content += f"          <p><strong>42sh Error:</strong> <a href=\"{test['42sh_error']}\" target=\"_blank\">{test['42sh_error']}</a></p>\n"
            html_content += f"          <p><strong>Bash Error:</strong> <a href=\"{test['bash_error']}\" target=\"_blank\">{test['bash_error']}</a></p>\n"
            html_content += f"        </div>\n"

            html_content += f"        <div id=\"{test_id}-exit-code\" style=\"display: none;\">\n"
            html_content += f"          <p><strong>42sh Exit Code:</strong> {test['42sh_exit_code']}</p>\n"
            html_content += f"          <p><strong>Bash Exit Code:</strong> {test['bash_exit_code']}</p>\n"
            html_content += f"        </div>\n"

            html_content += f"      </div>\n"
            html_content += f"    </div>\n"

        html_content += f"  </div>\n"
        html_content += f"</div>\n"

    html_content += """
</body>
</html>
"""
    return html_content

# Read JSON data from the file "log.json"
with open('log.json', 'r') as file:
    json_input = json.load(file)

# Generate HTML from the loaded JSON data
html = generate_html(json_input)

# Save the HTML to a file
with open("test_results.html", "w") as f:
    f.write(html)

print("HTML file generated: test_results.html")
