CC = gcc

CFLAGS = -Wall -g -Wextra -pedantic -Werror

LD = gcc

all: mytar

mytar: mytar.o parser.o explorer.o stack.o contents.o conformer.o create.o extract.o mytarutils.o
	$(CC) $(CFLAGS) -o mytar mytar.o parser.o explorer.o stack.o contents.o conformer.o create.o extract.o mytarutils.o

mytar.o: mytar.c mytar.h parser.h explorer.h contents.h stack.h create.h extract.h mytarutils.h
	$(CC) $(CFLAGS) -c mytar.c

mytarutils.o: mytarutils.c mytarutils.h mytar.h
	$(CC) $(CFLAGS) -c mytarutils.c

parser.o: parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

extract.o: extract.c extract.h mytar.h mytarutils.h
	$(CC) $(CFLAGS) -c extract.c

create.o: create.c create.h conformer.h mytar.h mytarutils.h
	$(CC) $(CFLAGS) -c create.c

explorer.o: explorer.c explorer.h stack.h
	$(CC) $(CFLAGS) -c explorer.c

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

contents.o: contents.c contents.h mytar.h mytarutils.h
	$(CC) $(CFLAGS) -c contents.c

conformer.o: conformer.c conformer.h
	$(CC) $(CFLAGS) -c conformer.c

contents: all
	./mytar cf TARFILE Test/
	./mytar tvf TARFILE

extract: all
	~pn-cs357/demos/mytar xvf ~pn-cs357/lib/asgn4/Tests/data/Archives/Deep.tar TreeNoPrefix/Deep/Deeper/EvenDeeper/WayFarDown/FarDownFile-6 TreeNoPrefix/Deep/Deeper/EvenDeeper/WayFarDown/FarDownFile-13 < demo.out
	./mytar xvf ~pn-cs357/lib/asgn4/Tests/data/Archives/Deep.tar TreeNoPrefix/Deep/Deeper/EvenDeeper/WayFarDown/FarDownFile-6 TreeNoPrefix/Deep/Deeper/EvenDeeper/WayFarDown/FarDownFile-13 < mytar.out
	diff mytar.out demo.out

extract2: all
	./mytar xvf ~pn-cs357/lib/asgn4/Tests/data/Archives/Deep.tar TreeNoPrefix/Deep/Deeper/EvenDeeper/WayFarDown/FarDownFile-6 TreeNoPrefix/Deep/Deeper/EvenDeeper/WayFarDown/FarDownFile-13

extract3: all
	./mytar xvf ~pn-cs357/lib/asgn4/Tests/data/Archives/Deep.tar

create: all
	./mytar cf TARFILE ../asgn3 ../asgn1
	~pn-cs357/demos/mytar cf DEMO_TARFILE ../asgn3 ../asgn1
	od -c -b TARFILE > tar.out
	od -c -b DEMO_TARFILE > demo_tar.out
	diff tar.out demo_tar.out

test: all
	./mytar cvf TARFILE Test/
	./mytar tvf TARFILE
	rm -rf Test/
	./mytar xvf TARFILE

bad: all
	./mytar cf TARFILE Test/ blah
	od -c -b TARFILE

leakcheck: all
	valgrind --leak-check=full --show-leak-kinds=all ./mytar xf TARFILE 

clean:
	rm -f *.o
	rm -f *.out
	rm -f DEMO_TARFILE
	rm -f TARFILE
	rm -f diffs.*
	rm -f *.tar
