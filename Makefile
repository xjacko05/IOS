all: proj2.c
	@gcc -std=gnu99 -Wall -Wextra -Werror -pedantic -o proj2 proj2.c -lpthread -lrt
clean: proj2
	@rm proj2
