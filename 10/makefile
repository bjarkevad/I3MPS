CC = gcc

	#gcc -g -O<0/1/2> -D<SLOW/FASTER/FASTEST> -o <name> 
all:
	$(CC) -g -O0 -D SLOW -o slow0 testcache.c
	$(CC) -g -O1 -D SLOW -o slow1 testcache.c
	$(CC) -g -O2 -D SLOW -o slow2 testcache.c
	$(CC) -g -O3 -D SLOW -o slow3 testcache.c

	$(CC) -g -O0 -D FASTER -o faster0 testcache.c
	$(CC) -g -O1 -D FASTER -o faster1 testcache.c
	$(CC) -g -O2 -D FASTER -o faster2 testcache.c
	$(CC) -g -O3 -D FASTER -o faster3 testcache.c

	$(CC) -g -O0 -D FASTEST -o fastest0 testcache.c
	$(CC) -g -O1 -D FASTEST -o fastest1 testcache.c
	$(CC) -g -O2 -D FASTEST -o fastest2 testcache.c
	$(CC) -g -O3 -D FASTEST -o fastest3 testcache.c
	


