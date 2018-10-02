#include "chess.h"
#include "init.h"
#include "utile.h"
#include "book.h"

void InitialiseData()
{
  // On initialise les premiers bits.
  // Au depart la position du bit est 0.
  // Ex.
  // Valeur    Bits      Pos. premier bits. Prochain shift
  // 0         0000      16                 n/a
  // 1         0001      1                  1
  // 2         0010      2                  2
  // 3         0011      2
  // 4         0100      3                  4
  // 5         0101      3
  // ...
  // 8         1000      4
  // ...
  // 15        1111      4
  // On remarque que le prochain shift se fait toujours en incrementant
  // de double. C'est normal, c'est ca le binaire.
  //
  // On va boucler de 1 a 65535 et a chaque shift a gauche on double
  // le compteur du prochain shift.
  dernier_bits[0] = 0; // Cas particulier. Pas de premier bits.
  unsigned char PosBit = 0; // On commence par la position 1.
  unsigned int ProchainShift = 1, ProchainShiftAFaire = 1;
  for( int i=1; i<65536; i++ ) {
    dernier_bits[i] = PosBit;
    ProchainShift--;
    // Si on a atteind le prochain shift a faire on le shift.
    if ( ProchainShift == 0 ) {
      // On double le prochain shift a faire.
      ProchainShiftAFaire *= 2;
      ProchainShift = ProchainShiftAFaire;
      PosBit++;
    }
  }

	// Initialisation des cles de hachages.
	for( int i=A8; i<=H1; i++ ) {
		ClePionB[i] = Random64();
		ClePionN[i] = Random64();
		CleCavalierB[i] = Random64();
		CleCavalierN[i] = Random64();
		CleFouB[i] = Random64();
		CleFouN[i] = Random64();
		CleTourB[i] = Random64();
		CleTourN[i] = Random64();
		CleDameB[i] = Random64();
		CleDameN[i] = Random64();
		CleRoiB[i] = Random64();
		CleRoiN[i] = Random64();
	}

    openingBook = new Book();
}
