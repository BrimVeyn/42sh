#!/bin/bash

object_dir="objects"
count=0

# Loop over all .o files in the objects directory
for obj in $(find "$object_dir" -type f -name "*.o"); do
    # Determine the corresponding .d file
    dep="${obj%.o}.d"
    
    # Check if the .d file exists
    if [[ ! -f "$dep" ]]; then
        continue
    fi
    
    # Read dependencies from the .d file
	dependencies=$(sed -E 's/^([^:]*):\s*(.*)/\2/' "$dep" | sed 's/\\$//g');
    
    # Check if any dependency is newer than the .o file
    needs_recompile=false
    for dependency in $dependencies; do
        if [[ "$dependency" -nt "$obj" ]]; then
            needs_recompile=true
            count=$((count + 1))  # Increment the count
            break
        fi
    done
done 2>/dev/null

# Output the count of files to recompile
echo "$count"
