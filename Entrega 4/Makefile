CFLAGS = -g -Wall -pedantic

i-banco: i-banco.o contas.o commandlinereader.o comando.o
	gcc -o i-banco i-banco.o contas.o commandlinereader.o comando.o -lpthread
	
i-banco.o: i-banco.c
	gcc $(CFLAGS) -c i-banco.c
	
contas.o: contas.c contas.h
	gcc $(CFLAGS) -c contas.c

commandlinereader.o: commandlinereader.c commandlinereader.h
	gcc $(CFLAGS) -c commandlinereader.c
	
comando.o:	comando.c comando.h
	gcc $(CFLAGS) -c comando.c
	
clean: 
	rm -f *.o i-banco
