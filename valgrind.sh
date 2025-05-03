#!/bin/bash

# === Configuration ===
TARGET_BINARY="./microtec_test"
VALGRIND_FLAGS=("--leak-check=full" "--show-leak-kinds=all")

# Exit if binary is missing
if [ ! -f "$TARGET_BINARY" ]; then
    echo "Error: $TARGET_BINARY not found. Please run bash build.sh first."
    exit 1
fi

echo "  ~ Running Valgrind memory check..."
valgrind "${VALGRIND_FLAGS[@]}" "$TARGET_BINARY"