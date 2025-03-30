sim: shell.c sim.c instructions/sim_instructions.c utils.c
	gcc -g -O0 $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o *~ sim

run_server: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	echo -e "go\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "go\nrdump\nquit" | ../ref_sim_x86 ../inputs/bytecodes/$(file).x
	make clean

run_server_n: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	@if [ -z "$(n)" ]; then echo "Error: Debes proporcionar un valor para 'n'. Usa: make run file=nombre n=valor"; exit 1; fi
	echo -e "run $(n)\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "run $(n)\nrdump\nquit" | ../ref_sim_x86 ../inputs/bytecodes/$(file).x
	make clean

run_k: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	echo -e "go\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "go\nrdump\nquit" | ../ref_sim_arm_m3 ../inputs/bytecodes/$(file).x
	make clean

run_c: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	echo -e "go\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "go\nrdump\nquit" | ../ref_sim_x86 ../inputs/bytecodes/$(file).x
	make clean

run_k_n: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	@if [ -z "$(n)" ]; then echo "Error: Debes proporcionar un valor para 'n'. Usa: make run file=nombre n=valor"; exit 1; fi
	echo -e "run $(n)\nrdump\nmdump 0x10000000 0x10000008\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "run $(n)\nrdump\nmdump 0x10000000 0x10000008\nquit" | ../ref_sim_arm_m3 ../inputs/bytecodes/$(file).x
	make clean


run_server_all: sim
	@if [ ! -d "../inputs/bytecodes" ]; then echo "Error: Test directory '../inputs/bytecodes' not found!"; exit 1; fi
	for test in ../inputs/bytecodes/*.x; do \
		test_name=$$(basename $$test .x); \
		echo "Running test: $$test_name"; \
		echo -e "quit" | ./sim $$test > temp_output.txt; \
		instr_total=$$(grep -oE "Read [0-9]+ words from program into memory" temp_output.txt | awk '{print $$2}'); \
		if [ -z "$$instr_total" ]; then echo "Error: Could not determine instruction count for $$test_name"; exit 1; fi; \
		echo "Total instructions to execute: $$instr_total"; \
		\
		# Initialize instruction counter and test status
		instr_count=0; \
		test_passed=true; \
		\
		# Run the program step-by-step
		while [ $$instr_count -lt $$instr_total ]; do \
			instr_count=$$(expr $$instr_count + 1); \
			echo -e "run 1\nrdump\nmdump 0x10000000 0x10100000\nquit" | ./sim $$test > output_my_$$test_name.txt; \
			grep -E "rdump|mdump 0x10000000 0x10100000" output_my_$$test_name.txt > result_my_$$test_name_$$instr_count.txt; \
			echo -e "run 1\nrdump\nmdump 0x10000000 0x10100000\nquit" | ../ref_sim_x86 $$test > output_sim_$$test_name.txt; \
			grep -E "rdump|mdump 0x10000000 0x10100000" output_sim_$$test_name.txt > result_sim_$$test_name_$$instr_count.txt; \
			if ! diff result_my_$$test_name_$$instr_count.txt result_sim_$$test_name_$$instr_count.txt > /dev/null; then \
				echo "Instruction $$instr_count: TEST $$test_name FAILED ❌"; \
				diff result_my_$$test_name_$$instr_count.txt result_sim_$$test_name_$$instr_count.txt; \
				test_passed=false; \
			fi; \
		done; \
		\
		# If all iterations matched, print test passed
		if [ "$$test_passed" = true ]; then \
			echo "TEST $$test_name PASSED ✅"; \
		else \
			echo "TEST $$test_name FAILED ❌"; \
		fi; \
	done