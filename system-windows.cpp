#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include "system.h"
#include "chess.h"

long int inter() {
	static int init = 0, pipe;
	static HANDLE inh;
	DWORD dw;
	if (!init) {
		init = 1;
		inh = GetStdHandle(STD_INPUT_HANDLE);
		pipe = !GetConsoleMode(inh, &dw);
		if (!pipe) {
			SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
			FlushConsoleInputBuffer(inh);
			FlushConsoleInputBuffer(inh);
  		}
  	}
  	if(pipe) {
		if(!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
		return dw;
  	} else {
		GetNumberOfConsoleInputEvents(inh, &dw);
		return dw <= 1 ? 0 : dw;
	}
}

int TempsCenti() {
	return GetTickCount64()/10;
}

// Cette fonction cherche le dernier numero du fichier de journal
// pour savoir quel est le numero de journal a creer.
int trouverDernierJournal() {
	WIN32_FIND_DATAA fd;
	int lastlog = 0;
	
	HANDLE hFind = ::FindFirstFileA("game.*", &fd);
	if (hFind != INVALID_HANDLE_VALUE) { // make sure valid
	    do {
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) { // don't care about directories
				int lenname = strlen(fd.cFileName);
				int nblog = atoi((const char *)&fd.cFileName[lenname-3]);
				if (nblog > lastlog)
					lastlog = nblog;
			}
		} while (::FindNextFileA(hFind, &fd)); // enumerates contents
		::FindClose(hFind);
	}

	return lastlog;
}
