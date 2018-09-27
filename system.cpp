#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "chess.h"

#ifdef _WIN32
#include <windows.h>
int inter() {
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

long TempsCenti()
{
  return GetTickCount()/10;
}

// Cette fonction cherche le dernier numero du fichier de journal
// pour savoir quel est le numero de journal a creer.
int TrouverDernierJournal() {
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

#else
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int TempsCenti()
{
  return clock() * 100 / CLOCKS_PER_SEC;
}

long inter()
{
  fd_set readfds;
  timeval timeout;
  int retval;

  FD_ZERO( &readfds );
  FD_SET( 0, &readfds );
  timeout.tv_sec = 0;
  timeout.tv_usec = 0;
  retval = select(1, &readfds, NULL, NULL, &timeout );

  return retval;
}

long TrouverDernierJournal()
{
  static long iDernierNo = 0;

  return iDernierNo++;
}

#endif

void MakePath(char* path, const char* filename)
{
	strcpy(path, MonikHome);
	int l = strlen(path);
	if (path[l-1] != '/') {
		strcat(path, "/");
	}
	strcpy(path, filename);
}
