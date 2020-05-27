SOURCE = proj2
CC = gcc
CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
all: $(SOURCE).c
	@$(CC) -std=gnu99 -Wall -Wextra -Werror -pedantic -o $(SOURCE) $(SOURCE).c -lpthread -lrt
clean: $(SOURCE)
	@rm $(SOURCE)
