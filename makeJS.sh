#!/bin/bash

# Read "payload.bin" byte by byte and convert to decimal
# The output is written to a file called "loader.js"
echo -n $(od -t u1 -An -v "build/payload.bin" | head -n1) | tr ' ' ',' > "loader.js"
od -t u1 -An -v "build/payload.bin" | tail -n+2 | tr ' ' ',' | tr '\n' ' ' | tr -d '[:space:]' | sed 's/,,*/,/g; s/,$//' >> "loader.js"

# Read the contents of the file into a variable called "content"
content=$(cat loader.js)

# Construct the output string
output="window.mira_blob=malloc(65536);\nwrite_mem(window.mira_blob,[$content]);"

# Write the output to the file "loader.js"
echo -e "$output" > "loader.js"
