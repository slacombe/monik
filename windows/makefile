CC	= clang++

CPPFLAGS = -Wall -D_CRT_SECURE_NO_WARNINGS -DNULL_MOVE -DUSE_SEE -DTRANSPOSITION -DICSTALK
LDFLAGS = -static

objects = obj/monik.obj obj/board.obj obj/init.obj obj/iteration.obj obj/racine.obj obj/make.obj obj/unmake.obj obj/engine.obj\
	  obj/search.obj obj/chess.obj obj/utile.obj obj/quies.obj obj/genmove.obj obj/eval.obj obj/next.obj\
	  obj/valide.obj obj/repetition.obj obj/sortie.obj obj/entree.obj obj/log.obj obj/nextracine.obj\
	  obj/edit.obj obj/attaque.obj obj/chargeur.obj obj/book.obj obj/transposition.obj obj/time.obj\
	  obj/system.obj obj/system-windows.obj obj/stats.obj obj/benchmark.obj

obj/%.obj: ../%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@ 

monik: $(objects) $(windows-objs)
	$(CC) $(LDFLAGS) -o monik.exe $(objects)

clean:
	-del .\obj\*.obj
	-del .\monik.exe
