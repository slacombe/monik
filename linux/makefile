CC	= g++

CPPFLAGS = -Wall -O2 -DNULL_MOVE -DUSE_SEE -DTRANSPOSITION -DICSTALK
LDFLAGS = -static

windows-objs := obj/system-windows.o
linux-objs := obj/system-linux.o

objects = obj/monik.o obj/board.o obj/init.o obj/iteration.o obj/racine.o obj/make.o obj/unmake.o obj/engine.o\
	  obj/search.o obj/chess.o obj/utile.o obj/quies.o obj/genmove.o obj/eval.o obj/next.o\
	  obj/valide.o obj/repetition.o obj/sortie.o obj/entree.o obj/log.o obj/nextracine.o\
	  obj/edit.o obj/attaque.o obj/chargeur.o obj/book.o obj/transposition.o obj/time.o\
	  obj/test.o obj/system.o

obj/%.o: ../%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

monik: $(objects) $(linux-objs)
	$(CC) $(LDFLAGS) $(objects) $(linux-objs) -o ../monik-linux

clean:
	-rm -f *~ obj/*.o ../monik-linux

install:
	cp monik ~/bin/monik
