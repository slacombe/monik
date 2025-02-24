#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "utile.h"
#include "board.h"
#include "chess.h"
#include "sortie.h"
#include "next.h"
#include "log.h"

// Routine pour l'aide au debogage.
void consistence(TChessBoard *cb, int ply, const char* caller, TMove move)
{
  // Pour chaque case de l'echiquier, verifier si les bitboards sont
  // consistent avec.
  bool bConsistent = true;
  for( int i=A8; i<=H1; i++ ) {
    // Case vide.
    if ( cb->board[i] == 0 ) {
      bConsistent = bConsistent && (mask[i] & cb->piece) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et piece." );

      bConsistent = bConsistent && (mask[i] & cb->pieceb) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceb." );

      bConsistent = bConsistent && (mask[i] & cb->piecen) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et piecen." );

      bConsistent = bConsistent && (mask[Rotate90LMap[i]] & cb->pieceR90L) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR90L." );

      bConsistent = bConsistent && (mask[Rotate45LRealMap[i]] & cb->pieceR45L) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR45L." );

      bConsistent = bConsistent && (mask[Rotate45RRealMap[i]] & cb->pieceR45R) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR45R." );

      bConsistent = bConsistent && (mask[i] & cb->vide) == mask[i];
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et vide." );

      bConsistent = bConsistent && (mask[i] & cb->pionb) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pionb." );

      bConsistent = bConsistent && (mask[i] & cb->pionn) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pionn." );

      bConsistent = bConsistent && (mask[i] & cb->cavalierb) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et cavalierb." );

      bConsistent = bConsistent && (mask[i] & cb->cavaliern) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et cavaliern." );

      bConsistent = bConsistent && (mask[i] & cb->foub) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et foub." );

      bConsistent = bConsistent && (mask[i] & cb->foun) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et foun." );

      bConsistent = bConsistent && (mask[i] & cb->tourb) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et tourb." );

      bConsistent = bConsistent && (mask[i] & cb->tourn) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et tourn." );

      bConsistent = bConsistent && (mask[i] & cb->dameb) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et dameb." );

      bConsistent = bConsistent && (mask[i] & cb->damen) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et damen." );

      bConsistent = bConsistent && (mask[i] & cb->roib) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et roib." );

      bConsistent = bConsistent && (mask[i] & cb->roin) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et roin." );

      if ( !bConsistent ) {
        char Erreur[255];
        gameLog << cb;
        sprintf( Erreur, "From: %d To: %d Piece: %d Capture: %d EnPassant: %d Promotion: %d",
                         move.From, move.To, move.Piece, move.Capture,
                         move.EnPassant, move.Promotion );
        gameLog.log("Caller: %s", caller);
        gameLog.log("Phase[ply] = %d", Phase[ply]);
        gameLog.log( "Erreur: La case du board est vide." );
        gameLog.log(Erreur);
        sprintf( Erreur, "Case %d contient %d", i, cb->board[i] );
        gameLog.log( Erreur );
        getCurrent(cb, Erreur, ply);
        gameLog.log( Erreur );
        exit(1);
      }
    }
    // Avec une piece blanche.
    else if ( cb->board[i] > 0 ) {
      bConsistent = bConsistent && mask[i] & cb->piece;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et piece." );

      bConsistent = bConsistent && mask[i] & cb->pieceb;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceb." );

      bConsistent = bConsistent && mask[Rotate90LMap[i]] & cb->pieceR90L;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR90L b." );

      bConsistent = bConsistent && mask[Rotate45LRealMap[i]] & cb->pieceR45L;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR45L b." );

      bConsistent = bConsistent && mask[Rotate45RRealMap[i]] & cb->pieceR45R;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR45R b." );

      bConsistent = bConsistent && (mask[i] & cb->vide) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et vide." );

      switch( cb->board[i] ) {
        case pion:
          bConsistent = bConsistent && mask[i] & cb->pionb;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et pionb." );

          break;
        case cavalier:
          bConsistent = bConsistent && mask[i] & cb->cavalierb;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et cavalierb." );
          break;
        case fou:
          bConsistent = bConsistent && mask[i] & cb->foub;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et foub." );
          break;
        case tour:
          bConsistent = bConsistent && mask[i] & cb->tourb;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et tourb." );
          break;
        case dame:
          bConsistent = bConsistent && mask[i] & cb->dameb;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et dameb." );
          break;
        case roi:
          bConsistent = bConsistent && mask[i] & cb->roib;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et roib." );
          break;
      }
      if ( !bConsistent ) {
        char Erreur[255];
        gameLog << cb;
        sprintf( Erreur, "From: %d To: %d Piece: %d Capture: %d EnPassant: %d Promotion: %d",
                         move.From, move.To, move.Piece, move.Capture,
                         move.EnPassant, move.Promotion );
        gameLog.log("Caller: %s", caller);
        gameLog.log("Phase[ply] = %d", Phase[ply]);
        gameLog.log( "Erreur: La case du board contient une piece blanche." );
        gameLog.log(Erreur);
        sprintf(Erreur, "Case %d contient %d", i, cb->board[i]);
        gameLog.log( Erreur );
        getCurrent(cb, Erreur, ply);
        gameLog.log( Erreur );
        exit(1);
      }
    }
    // Avec une piece noire.
    else {
      bConsistent = bConsistent && mask[i] & cb->piece;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et piece." );

      bConsistent = bConsistent && mask[i] & cb->piecen;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et piecen." );

      bConsistent = bConsistent && mask[Rotate90LMap[i]] & cb->pieceR90L;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR90L n." );

      bConsistent = bConsistent && mask[Rotate45LRealMap[i]] & cb->pieceR45L;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR45L n." );

      bConsistent = bConsistent && mask[Rotate45RRealMap[i]] & cb->pieceR45R;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et pieceR45R n." );

      bConsistent = bConsistent && (mask[i] & cb->vide) == 0;
      if ( !bConsistent )
        gameLog.log( "Inconsistence entre board et vide." );

      switch( cb->board[i] ) {
        case -pion:
          bConsistent = bConsistent && mask[i] & cb->pionn;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et pionn." );
          break;
        case cavalier:
          bConsistent = bConsistent && mask[i] & cb->cavaliern;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et cavaliern." );
          break;
        case -fou:
          bConsistent = bConsistent && mask[i] & cb->foun;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et foun." );
          break;
        case -tour:
          bConsistent = bConsistent && mask[i] & cb->tourn;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et tourn." );
          break;
        case -dame:
          bConsistent = bConsistent && mask[i] & cb->damen;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et damen." );
          break;
        case -roi:
          bConsistent = bConsistent && mask[i] & cb->roin;
          if ( !bConsistent )
            gameLog.log( "Inconsistence entre board et roin." );
          break;
      }
      if ( !bConsistent ) {
        char Erreur[255];
        gameLog << cb;
        sprintf( Erreur, "From: %d To: %d Piece: %d Capture: %d EnPassant: %d Promotion: %d",
                         move.From, move.To, move.Piece, move.Capture,
                         move.EnPassant, move.Promotion );
        gameLog.log("Caller: %s", caller);
        gameLog.log("Phase[ply] = %d", Phase[ply]);
        gameLog.log( "Erreur: La case du board contient une piece noire." );
        gameLog.log( Erreur );
        sprintf(Erreur, "Case %d contient %d", i, cb->board[i]);
        gameLog.log( Erreur );
        getCurrent(cb, Erreur, ply);
        gameLog.log( Erreur );
        exit(1);
      }
    }
  }
}
#endif
