
SHELL=/bin/sh
CC=CC

md5:		driver.o md5.o
	$(CC) -o md5 driver.o md5.o

driver.o:	driver.cc
	$(CC) -c driver.cc

md5.o:		md5.hh md5.cc
	$(CC) -c md5.cc

clean:
	-rm *.o md5
