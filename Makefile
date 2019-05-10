CFLAGS=-Wall -Wextra
CC=gcc

objects = ag sv cv ma teste

all: $(objects)

$(objects): %: %.c
		$(CC) $(CFLAGS) $< -o $@


clean:
	rm sv ma cv ag teste

cleanFiles:
	rm artigos strings vendas stocks
	touch artigos strings vendas stocks
