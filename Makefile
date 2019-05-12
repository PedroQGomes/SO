CFLAGS=-Wall -Wextra
CC=gcc

objects = ag sv cv ma teste

all: $(objects)

$(objects): %: %.c
		$(CC) $(CFLAGS) $< -o $@


clean:
	rm sv ma cv ag teste

cleanFiles:
	rm -f artigos strings vendas stocks agregRes agState
	touch artigos strings vendas stocks
