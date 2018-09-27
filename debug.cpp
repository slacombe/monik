#ifdef DEBUG
#include <vcl.h>
#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>

#include "Debug.h"
#include "Utile.h"
#include "Board.h"
#include "Chess.h"
#include "Journal.h"
#include "Sortie.h"

extern TJournal journal;

// Routine pour l'aide au debogage.
void Consistence( TChessBoard& i_CBoard, TMove& move )
{
  // Pour chaque case de l'echiquier, verifier si les bitboards sont
  // consistent avec.
  bool bConsistent = true;
  for( int i=A8; i<=H1; i++ ) {
    // Case vide.
    if ( i_CBoard.board[i] == 0 ) {
      bConsistent = bConsistent && (mask[i] & i_CBoard.piece) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et piece." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.pieceb) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceb." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.piecen) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et piecen." );

      bConsistent = bConsistent && (mask[Rotate90LMap[i]] & i_CBoard.pieceR90L) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR90L." );

      bConsistent = bConsistent && (mask[Rotate45LRealMap[i]] & i_CBoard.pieceR45L) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR45L." );

      bConsistent = bConsistent && (mask[Rotate45RRealMap[i]] & i_CBoard.pieceR45R) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR45R." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.vide) == mask[i];
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et vide." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.pionb) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pionb." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.pionn) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pionn." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.cavalierb) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et cavalierb." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.cavaliern) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et cavaliern." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.foub) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et foub." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.foun) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et foun." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.tourb) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et tourb." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.tourn) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et tourn." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.dameb) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et dameb." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.damen) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et damen." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.roib) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et roib." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.roin) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et roin." );

      if ( !bConsistent ) {
        char Erreur[255];
        sprintf( Erreur, "From: %d To: %d Piece: %d Capture: %d EnPassant: %d Promotion: %d",
                         move.From, move.To, move.Piece, move.Capture,
                         move.EnPassant, move.Promotion );
        journal.Log( "Erreur: La case du board est vide." );
        journal.Log( Erreur );
        sprintf( Erreur, "Case %d contient %d", i, i_CBoard.board[i] );
        journal.Log( Erreur );
        GetCurrent( ChessBoard, Erreur, 4 );
        journal.Log( Erreur );
        exit(1);
      }
    }
    // Avec une piece blanche.
    else if ( i_CBoard.board[i] > 0 ) {
      bConsistent = bConsistent && mask[i] & i_CBoard.piece;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et piece." );

      bConsistent = bConsistent && mask[i] & i_CBoard.pieceb;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceb." );

      bConsistent = bConsistent && mask[Rotate90LMap[i]] & i_CBoard.pieceR90L;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR90L b." );

      bConsistent = bConsistent && mask[Rotate45LRealMap[i]] & i_CBoard.pieceR45L;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR45L b." );

      bConsistent = bConsistent && mask[Rotate45RRealMap[i]] & i_CBoard.pieceR45R;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR45R b." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.vide) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et vide." );

      switch( i_CBoard.board[i] ) {
        case pion:
          bConsistent = bConsistent && mask[i] & i_CBoard.pionb;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et pionb." );

          break;
        case cavalier:
          bConsistent = bConsistent && mask[i] & i_CBoard.cavalierb;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et cavalierb." );
          break;
        case fou:
          bConsistent = bConsistent && mask[i] & i_CBoard.foub;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et foub." );
          break;
        case tour:
          bConsistent = bConsistent && mask[i] & i_CBoard.tourb;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et tourb." );
          break;
        case dame:
          bConsistent = bConsistent && mask[i] & i_CBoard.dameb;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et dameb." );
          break;
        case roi:
          bConsistent = bConsistent && mask[i] & i_CBoard.roib;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et roib." );
          break;
      }
      if ( !bConsistent ) {
        char Erreur[255];
        sprintf( Erreur, "From: %d To: %d Piece: %d Capture: %d EnPassant: %d Promotion: %d",
                         move.From, move.To, move.Piece, move.Capture,
                         move.EnPassant, move.Promotion );
        journal.Log( "Erreur: La case du board contient une piece blanche." );
        journal.Log( Erreur );
        sprintf( Erreur, "Case %d contient %d", i, i_CBoard.board[i] );
        journal.Log( Erreur );
        GetCurrent( ChessBoard, Erreur, 4 );
        journal.Log( Erreur );
        exit(1);
      }
    }
    // Avec une piece noire.
    else {
      bConsistent = bConsistent && mask[i] & i_CBoard.piece;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et piece." );

      bConsistent = bConsistent && mask[i] & i_CBoard.piecen;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et piecen." );

      bConsistent = bConsistent && mask[Rotate90LMap[i]] & i_CBoard.pieceR90L;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR90L n." );

      bConsistent = bConsistent && mask[Rotate45LRealMap[i]] & i_CBoard.pieceR45L;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR45L n." );

      bConsistent = bConsistent && mask[Rotate45RRealMap[i]] & i_CBoard.pieceR45R;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et pieceR45R n." );

      bConsistent = bConsistent && (mask[i] & i_CBoard.vide) == 0;
      if ( !bConsistent )
        journal.Log( "Inconsistence entre board et vide." );

      switch( i_CBoard.board[i] ) {
        case -pion:
          bConsistent = bConsistent && mask[i] & i_CBoard.pionn;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et pionn." );
          break;
        case cavalier:
          bConsistent = bConsistent && mask[i] & i_CBoard.cavaliern;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et cavaliern." );
          break;
        case -fou:
          bConsistent = bConsistent && mask[i] & i_CBoard.foun;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et foun." );
          break;
        case -tour:
          bConsistent = bConsistent && mask[i] & i_CBoard.tourn;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et tourn." );
          break;
        case -dame:
          bConsistent = bConsistent && mask[i] & i_CBoard.damen;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et damen." );
          break;
        case -roi:
          bConsistent = bConsistent && mask[i] & i_CBoard.roin;
          if ( !bConsistent )
            journal.Log( "Inconsistence entre board et roin." );
          break;
      }
      if ( !bConsistent ) {
        char Erreur[255];
        sprintf( Erreur, "From: %d To: %d Piece: %d Capture: %d EnPassant: %d Promotion: %d",
                         move.From, move.To, move.Piece, move.Capture,
                         move.EnPassant, move.Promotion );
        journal.Log( "Erreur: La case du board contient une piece noire." );
        journal.Log( Erreur );
        sprintf( Erreur, "Case %d contient %d", i, i_CBoard.board[i] );
        journal.Log( Erreur );
        GetCurrent( ChessBoard, Erreur, 4 );
        journal.Log( Erreur );
        exit(1);
      }
    }
  }
}
#endif
