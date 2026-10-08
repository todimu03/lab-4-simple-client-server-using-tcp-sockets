serv: serv.c list.c
	gcc serv.c list.c -o server

cli: cli.c
	gcc cli.c -o client