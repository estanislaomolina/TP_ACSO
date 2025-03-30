#!/bin/bash

#!/bin/bash

# Directory where test cases are stored
TEST_DIR="../inputs/bytecodes"
SIMULATOR="../ref_sim_x86"   # Path to the reference simulator
MY_FUNCTION="./sim"          # Path to your implementation

# Ensure the test directory exists
if [ ! -d "$TEST_DIR" ]; then
    echo "Test directory $TEST_DIR not found!"
    exit 1
fi

# Run all tests
for test in "$TEST_DIR"/*.x; do
    test_name=$(basename "$test" .x)
    echo "Running test: $test_name"

    # Initialize instruction counter
    instr_count=0

    while true; do
        instr_count=$((instr_count + 1))

        # Run my function for one instruction
        echo "run 1" | $MY_FUNCTION "$test" > "output_my_$test_name.txt"
        grep -E "rdump|mdump 0x10000000 0x10100000" "output_my_$test_name.txt" > "result_my_${test_name}_$instr_count.txt"

        # Run reference simulator for one instruction
        echo "run 1" | $SIMULATOR "$test" > "output_sim_$test_name.txt"
        grep -E "rdump|mdump 0x10000000 0x10100000" "output_sim_$test_name.txt" > "result_sim_${test_name}_$instr_count.txt"

        # Compare results for this instruction
        if diff "result_my_${test_name}_$instr_count.txt" "result_sim_${test_name}_$instr_count.txt" > /dev/null; then
            echo "Instruction $instr_count: TEST $test_name passed ✅"
        else
            echo "Instruction $instr_count: TEST $test_name FAILED ❌"
            diff "result_my_${test_name}_$instr_count.txt" "result_sim_${test_name}_$instr_count.txt"
            break  # Stop if an instruction fails
        fi

        # Check if the program has halted (HLT detected)
        if grep -q "HLT" "output_my_$test_name.txt"; then
            echo "Program finished for $test_name."
            break
        fi
    done
done