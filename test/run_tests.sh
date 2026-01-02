#!/bin/bash
# Run all unit tests using Unity from lib/

set -e
cd /home/runner/work/ProofingChamber2/ProofingChamber2

echo "======================================================================"
echo "Running Unit Tests (Unity from lib/Unity)"
echo "======================================================================"
echo ""

# Test SimpleTime
echo "Test: SimpleTime"
g++ -std=c++11 -I lib/Unity/src -I src lib/Unity/src/unity.c test/test_SimpleTime.cpp src/SimpleTime.cpp -o test_SimpleTime_bin
./test_SimpleTime_bin
echo ""

# Test OptionalBool
echo "Test: OptionalBool"
g++ -std=c++11 -I lib/Unity/src -I src lib/Unity/src/unity.c test/test_OptionalBool.cpp -o test_OptionalBool_bin
./test_OptionalBool_bin
echo ""

# Test SafePtr
echo "Test: SafePtr"
g++ -std=c++11 -I lib/Unity/src -I src lib/Unity/src/unity.c test/test_SafePtr.cpp -o test_SafePtr_bin
./test_SafePtr_bin
echo ""

# Test Graph
echo "Test: Graph"
g++ -std=c++11 -I lib/Unity/src -I test -I src lib/Unity/src/unity.c test/test_Graph.cpp src/Graph.cpp -o test_Graph_bin
./test_Graph_bin
echo ""

echo "======================================================================"
echo "All Tests Passed!"
echo "======================================================================"


