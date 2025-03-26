sim: shell.c sim.c 
	gcc -g -O0 $^ -o $@

.PHONY: clean
clean:
	rm -rf *.o *~ sim


run_k: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	echo -e "go\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "go\nrdump\nquit" | ../ref_sim_arm_m3 ../inputs/bytecodes/$(file).x
	make clean

run_c: sim
	@if [ -z "$(file)" ]; then echo "Error: Debes proporcionar un valor para 'file'. Usa: make run file=nombre"; exit 1; fi
	echo -e "go\nrdump\nquit" | ./sim ../inputs/bytecodes/$(file).x
	echo -e "go\nrdump\nquit" | ../sim_ref_x86 ../inputs/bytecodes/$(file).x
	make clean

