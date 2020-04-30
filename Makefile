all: proj2.c
	@gcc -std=gnu99 -Wall -Wextra -Werror -pedantic -lpthread -lrt -o proj2 proj2.c
clean: proj2
	@rm proj2
