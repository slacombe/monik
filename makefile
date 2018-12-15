CC	= g++

CPPFLAGS = -Wall -g -std=c++11 -DNULL_MOVE -DUSE_SEE -DTRANSPOSITION -DICSTALK
#LDFLAGS = -static

objects = monik.o board.o init.o iteration.o racine.o make.o unmake.o engine.o\
	  search.o chess.o utile.o quies.o genmove.o eval.o next.o\
	  valide.o repetition.o sortie.o entree.o log.o nextracine.o\
	  edit.o attaque.o chargeur.o system.o book.o transposition.o time.o test.o\
	  controller/Controller.o controller/ControllerFactory.o\
	  protocol/NoGuiProtocol.o protocol/ProtocolFactory.o\
	  ui/UIController.o ui/UIController.o ui/UIControllerFactory.o


monik: $(objects)

clean:
	-rm -f *~ *.o controller/*.o protocol/*.o ui/*.o monik

install:
	cp monik ~/bin/monik
