#!/bin/bash

# Exit immediately if any command fails
set -e

# === Configuration ===
SRC_FILE="SensorDataManager.cpp"
TEST_FILE="unit_tests_microtec.cpp"  # Change this to swap test/main files
OUTPUT_BINARY="./microtec_test"
GPP_FLAGS=("-g" "-o" "$OUTPUT_BINARY" "-lpthread")

# === Compilation ===
echo "  ~ Compiling SensorDataManager with g++..."
g++ "${GPP_FLAGS[@]}" "${SRC_FILE}" "${TEST_FILE}"

echo "  ~ Build successful. Output binary: $OUTPUT_BINARY"