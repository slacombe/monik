CC	= g++

CPPFLAGS = -Wall -O2 -std=c++11 -DNULL_MOVE -DUSE_SEE -DTRANSPOSITION -DICSTALK
#LDFLAGS = -static

objects = monik.o board.o init.o iteration.o racine.o make.o unmake.o engine.o\
	  search.o chess.o utile.o quies.o genmove.o eval.o next.o\
	  valide.o repetition.o sortie.o entree.o log.o nextracine.o\
	  edit.o attaque.o chargeur.o system.o book.o transposition.o time.o test.o\
	  controller/Controller.o controller/ControllerFactory.o\
	  protocol/WinBoardProtocol.o protocol/NoProtocol.o protocol/ProtocolFactory.o\
	  ui/UIController.o ui/UIController.o ui/UIControllerFactory.o


monik: $(objects)

clean:
	-rm -f *~ *.o monik

install:
	cp monik ~/bin/monik
