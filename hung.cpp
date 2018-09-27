#include "Chess.h"
#include "Board.h"
#include "Attaque.h"
#include "Utile.h"
#include "Hung.h"

SCORE Hung( TChessBoard& cb, int wtm )
{
  register Bitboard temp;
  register int pos, piece, score;

  if ( wtm ) {
    // Verifier si les dames sont attaques.
    temp = cb.dameb;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALDAME - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

    // Verifier si les tours sont attaque.
    temp = cb.tourb;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALTOUR - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

    // Verifier si les fous sont attaque.
    temp = cb.foub;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALFOU - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

    // Verifier si les cavaliers sont attaques.
    temp = cb.cavalierb;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALCAVALIER - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

  }
  else {
    // Verifier si les dames sont attaques.
    temp = cb.damen;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALDAME - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

    // Verifier si les tours sont attaque.
    temp = cb.tourn;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALTOUR - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

    // Verifier si les fous sont attaque.
    temp = cb.foun;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALFOU - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }

    // Verifier si les cavaliers sont attaques.
    temp = cb.cavaliern;
    while( temp ) {
      pos = DernierBit( temp );
      piece = Attacked( pos, !wtm );
      if ( piece != 0 ) {
        score = VALCAVALIER - ValeurPiece[piece];
        if ( score > 0 )
          return score;
      }
      ClearBit( temp, pos );
    }
  }

  return 0;
}
