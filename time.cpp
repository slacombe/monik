#include "time.h"
#include "iteration.h"
#include "system.h"
#include "log.h"

int TimeCheck()
{
	// If we still have time then continue.
	if ( TempsCenti() < timestamp + timeslot ) 
		return 0;

	// Time's up, can we stop.
	// If the score is a little bit down, then add some time
	// to try to find another move.
	if ( prev_root_score - root_score > 40 && !timeextension ) {
		gameLog.log( "root_score = %d, prev_root_score = %d, timeextension = %d", root_score, prev_root_score, timeextension );
		timeextension = timeslot;
		gameLog.log( "timeextension = %d", timeextension );
		timeslot += timeextension;
		return 0;
	}

	return 1;
}
