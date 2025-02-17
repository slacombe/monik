#include <stdio.h>
#include <stdlib.h>

#include "system.h"
#include "chess.h"

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

long trouverDernierJournal()
{
  static long iDernierNo = 0;

  return iDernierNo++;
}
