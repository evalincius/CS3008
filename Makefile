shell: shell.c filesys.c filesys.h
	gcc *.c -o shell
	./shell
	hexdump -C virtualdisk9_11
	hexdump -C virtualdisk12_14
	hexdump -C virtualdisk12_14 > trace12_14.txt