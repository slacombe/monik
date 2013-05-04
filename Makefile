CC	= g++

CPPFLAGS = -g -O2 -DNULL_MOVE -DUSE_SEE -DTRANSPOSITION -DICSTALK
#CFLAGS = -DTRANSPOSITION -DNULL_MOVE -DUSE_SEE -DJOURNAL -g

#LDFLAGS = -lstdc++

objects = monik.o board.o init.o iteration.o racine.o make.o unmake.o engine.o\
	  search.o chess.o utile.o quies.o genmove.o eval.o next.o\
	  valide.o repetition.o sortie.o entree.o journal.o nextracine.o\
	  edit.o attaque.o chargeur.o system.o book.o transposition.o time.o


monik: $(objects)

clean:
	-rm -f *~ *.o monik

install:
	cp monik /home/lacos00/monik


