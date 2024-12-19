# Simple function definition and call
# Simple
greet () { echo "Hello, ${1}!"; }
greet "World"

# Function with no parameters
say_hello () { echo "Hello, World!"; }
say_hello

# Function with local variables
add () { sum=$(( ${1} + ${2} )); echo "${1} + ${2} = ${sum}"; }
add 3 5

# Recursive function
factorial () {
  n=${1}
  if ! (exit $(( n <= 1 ))); then
    echo 1
  else
    echo $(( n * $(factorial $(( n - 1 )) ) ))
  fi
}
factorial 5

# # Function redefinition
increment () { echo $(( ${1} + 1 )); }
increment 10
increment () { echo $(( ${1} + 2 )); }
increment 10

# # Function with nested calls
nested_call () {
  echo "Outer call: ${1}"
  inner_call "$(( ${1} + 1 ))"
}
inner_call () { echo "Inner call: ${1}"; }
nested_call 5

# # Function with command substitution
get_date () { echo "$(date +%Y-%m-%d)"; }
get_date

# # Function with a loop
repeat_message () { for it in $(seq 1 ${1}); do echo "${2}"; done; }
repeat_message 3 "Repeating this message"

# # Function with an arithmetic operation
multiply () { echo "$(( ${1} * ${2} ))"; }
multiply 7 6
#
# # Function calling another function
outer_function () { inner_function "from outer"; }
inner_function () { echo "Inner function says: Hello ${1}"; }
outer_function
#
# # Function with no parameters, called with arguments
no_args () { echo "This function takes no arguments."; }
no_args 1 2 3

# # Function with positional parameters
print_all_args () { for arg in ${@}; do echo "${arg}"; done; }
print_all_args "arg1" "arg2" "arg3"

# # Function with variable scoping
global_var="I am global"
scope_test () {
  local_var="I am local"
  echo "G: ${global_var}"
  echo "L: ${local_var}"
}
scope_test
echo "${global_var}"

# # Function with special variables
print_special_vars () {
  echo "Script name: ${0}"
  echo "First argument: ${1}"
  echo "Number of arguments: ${#}"
  echo "All arguments: ${@}"
  echo "Exit status of last command: ${?}"
}
print_special_vars "test1" "test2"

# # Edge case: Function name starting with underscore
_valid_function_name () { echo "This is valid."; }
_valid_function_name

# # Edge case: Function redefinition with invalid name
valid_func () { echo "This is valid."; }
valid_func () { echo "Redefined func."; }
valid_func

# Define a simple function that uses return
simple_return_test() {
    return 42
}

simple_return_test; echo ${?}

# Test return value propagation
propagation_test() {
    return 5
    echo "This should not be printed"
}
propagation_test; echo ${?}

# Nested function calls with return
nested_return_test() {
    inner_function() {
        return 99
    }
    inner_function
    echo "Inner function returned ${?}" # Capture inner function's return value
    return 88
}
nested_return_test; echo ${?}

# Test invalid return outside a function
return 
echo ${?}
