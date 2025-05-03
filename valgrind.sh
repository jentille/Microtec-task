#!/bin/bash

# Exit if binary is missing
if [ ! -f ./microtec_test ]; then
    echo "Error: ./microtec_test not found. Please run ./build.sh first."
    exit 1
fi

echo "Running Valgrind memory check..."
valgrind --leak-check=full --show-leak-kinds=all ./microtec_test
