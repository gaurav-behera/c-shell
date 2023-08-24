main:
	gcc -g main.c prompt.c setup.c warp.c peek.c seek.c pastevents.c proclore.c execute.c dircontrol.c

valgrind: 
	clear
	gcc -g main.c prompt.c setup.c warp.c peek.c seek.c pastevents.c proclore.c execute.c dircontrol.c
	valgrind -s --leak-check=full --show-leak-kinds=all --track-origins=yes ./a.out

start: 
	gcc -g main.c prompt.c setup.c warp.c peek.c seek.c pastevents.c proclore.c execute.c dircontrol.c
	./a.out