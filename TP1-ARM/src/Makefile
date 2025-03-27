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
	echo -e "run $(n)\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "run $(n)\nrdump\nquit" | ../ref_sim_arm_m3 ../inputs/bytecodes/$(file).x
	make clean

valgrind: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make valgrind file=nombre"; exit 1; fi
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./sim ../inputs/bytecodes/$(file).x
	make clean
leaks: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make leaks file=nombre"; exit 1; fi
	leaks -atExit -- ./sim ../inputs/bytecodes/$(file).x
	make clean
