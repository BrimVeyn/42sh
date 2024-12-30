#!/bin/bash

object_dir="objects"
count=0

# Loop over all .o files in the objects directory
for obj in $(find "$object_dir" -type f -name "*.o"); do
    # Determine the corresponding .d file
    dep="${obj%.o}.d"
    
    # Skip if the .d file doesn't exist or if the .o file is invalid
    if [[ ! -f "$dep" || ! -s "$obj" ]]; then
        count=$((count + 1))  # Increment count for missing or invalid .o files
        continue
    fi
    
    # Read dependencies from the .d file
    dependencies=$(sed -E 's/^([^:]*):\s*(.*)/\2/' "$dep" | sed 's/\\$//g')

    # Check if any dependency is newer than the .o file
    needs_recompile=false
    for dependency in $dependencies; do
        if [[ "$dependency" -nt "$obj" ]]; then
            needs_recompile=true
            break
        fi
    done

    # Increment the count if the file needs recompilation
    if [[ "$needs_recompile" == true ]]; then
        count=$((count + 1))
    fi
done 2>/dev/null

# Output the count of files to recompile
echo "$count"
