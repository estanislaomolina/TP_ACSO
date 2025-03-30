#!/bin/bash

TEST_DIR="../inputs/bytecodes"
SIMULATOR="../ref_sim_x86"
MY_FUNCTION="./sim"

if [ ! -d "$TEST_DIR" ]; then
    echo "Test directory $TEST_DIR not found!"
    exit 1
fi

for test in "$TEST_DIR"/*.x; do
    test_name=$(basename "$test" .x)
    echo "Running test: $test_name"

    # Run my function
    echo -e "go\nrdump\nmdump 0x10000000 0x10100000\nquit" | $MY_FUNCTION "$test" > "output_my_$test_name.txt"
    grep -E "rdump|mdump 0x10000000 0x10100000" "output_my_$test_name.txt" > "result_my_$test_name.txt"

    # Run reference simulator
    echo -e "go\nrdump\nmdump 0x10000000 0x10100000\nquit" | $SIMULATOR "$test" > "output_sim_$test_name.txt"
    grep -E "rdump|mdump 0x10000000 0x10100000" "output_sim_$test_name.txt" > "result_sim_$test_name.txt"

    # Compare results
    if diff "result_my_$test_name.txt" "result_sim_$test_name.txt" > /dev/null; then
        echo "TEST $test_name PASSED ✅"
    else
        echo "TEST $test_name FAILED ❌"
        diff "result_my_$test_name.txt" "result_sim_$test_name.txt"
    fi

    # Cleanup temporary files
    rm -f "output_my_$test_name.txt" "output_sim_$test_name.txt" \
          "result_my_$test_name.txt" "result_sim_$test_name.txt"

done
