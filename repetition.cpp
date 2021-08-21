#include "logger/log.h"
#include "repetition.h"

bool Repetition(int wtm)
{
  // Si il y a plus 50 coup sans qu'il y est une capture ou l'avancement d'un
  // pion, c'est nulle.
  if ( cb.Regle50Coup > 99 )
    return true;

  // Pour commencer, verifier si il y a possibilite de mat avec les pieces
  // sur le jeux.
  if ( cb.TotalPionBlanc == 0 && cb.TotalPionNoir == 0 ) {
    if ( cb.ScoreMaterielBlanc < VALTOUR && cb.ScoreMaterielNoir < VALTOUR )
      return true;

    if ( (cb.ScoreMaterielBlanc == VALCAVALIER*2 && cb.TotalCavalierBlanc == 2) &&
         (cb.ScoreMaterielNoir == VALCAVALIER*2 && cb.TotalCavalierNoir == 2) )
      return true;
  }

  // Maintenant, verifier si la position actuelle est deja apparu.
  // Seulement jusqu'a la derniere postition irreversible.
  int rep = 0;
  if ( cb.NoCoups > 2 ) {
    for( int i=cb.NoCoups-2; i>=cb.NoCoups-cb.Regle50Coup; i -= 2 )
      if ( !(cb.Positions[ i ] ^ cb.CleHachage) ) {
		  rep++;
		  if ( rep >= 2 ) {
		    return true;
		  }
	  }

  }

  return false;
}


int RepetitionNulle(int wtm)
{
  // Si il y a plus 50 coup sans qu'il y est une capture ou l'avancement d'un
  // pion, c'est nulle.
  if ( cb.Regle50Coup > 99 )
    return NULLE_REGLE_50COUPS;

  // Pour commencer, verifier si il y a possibilite de mat avec les pieces
  // sur le jeux.
  if ( cb.TotalPionBlanc == 0 && cb.TotalPionNoir == 0 ) {
    if ( cb.ScoreMaterielBlanc < VALTOUR && cb.ScoreMaterielNoir < VALTOUR )
      return NULLE_MANQUE_DE_MATERIEL;

    if ( (cb.ScoreMaterielBlanc == VALCAVALIER*2 && cb.TotalCavalierBlanc == 2) &&
         (cb.ScoreMaterielNoir == VALCAVALIER*2 && cb.TotalCavalierNoir == 2) )
      return NULLE_MANQUE_DE_MATERIEL;
  }

  // Maintenant, verifier si la position actuelle est deja apparu.
  // Seulement jusqu'a la derniere postition irreversible.
  int rep = 0;
  if ( cb.NoCoups > 2 ) {
    for( int i=cb.NoCoups-2; i>=cb.NoCoups-cb.Regle50Coup; i -= 2 )
      if ( !(cb.Positions[i] ^ cb.CleHachage) ) {
        rep++;
        if ( rep >= 2 )
          return NULLE_REPETITION;
      }
  }

  return false;
}

//---------------------------------------------------------------------------
 
