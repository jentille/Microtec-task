#!/bin/bash

# Exit immediately if any command fails
set -e

echo "  ~ Compiling SensorDataManager with g++..."
g++ -g -o microtec_test SensorDataManager.cpp unit_tests_microtec.cpp -lpthread

echo "  ~ Build successful. Output binary: ./microtec_test"
