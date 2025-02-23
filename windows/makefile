CC	= clang++

CPPFLAGS = -Wall -std=c++17 -O2 -D_CRT_SECURE_NO_WARNINGS -DUSE_SEE -DTRANSPOSITION
#CPPFLAGS = -Wall -std=c++17 --target=x86_64-pc-windows-msvc -O2 -D_CRT_SECURE_NO_WARNINGS -DUSE_SEE -DTRANSPOSITION -I"C:/Program Files/LLVM/include/llvm"
#CPPFLAGS = -Wall -g -O0 -std=c++17 --target=x86_64 -D_CRT_SECURE_NO_WARNINGS -DUSE_SEE -DTRANSPOSITION
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
