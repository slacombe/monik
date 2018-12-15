//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Attaque.cpp
// Debut: 10 novembre 1998.
//
//---------------------------------------------------------------------------

#include "utile.h"
#include "attaque.h"

// Retourne si une case demande est a attaque.
// Semblable a GenMove, mais ici je genere pas de coup dans une liste. Je
// verifie plutot les bitboards pour voir si la case demande est attaque par
// l'adversaire et quitte aussi tot que possible.
// Retourne la piece qui l'attaque.
int Attacked(int i_iCase, int i_bWtm)
{
  Bitboard attaque;
  // C'est simple, generer les attaques ennemis et verifier si ils
  // possedent une piece sur une de ces cases.
  if ( i_bWtm ) {
    // Les pions.
    attaque = cb.AttaquePionNoir[i_iCase] & cb.pionb;
    if ( attaque ) return pion;
    // Les cavaliers.
    attaque = cb.AttaqueCavalier[i_iCase] & cb.cavalierb;
    if ( attaque ) return cavalier;
    // Les fous.
    attaque = AttaqueFou(i_iCase) & cb.foub;
    if ( attaque ) return fou;
    // Les tours.
    attaque = AttaqueTour(i_iCase) & cb.tourb;
    if ( attaque ) return tour;
    // Les dames.
    attaque = AttaqueDame(i_iCase) & cb.dameb;
    if ( attaque ) return dame;
    // Le roi.
    attaque = cb.AttaqueRoi[i_iCase] & cb.roib;
    if ( attaque ) return roi;
  }
  else {
    // Les pions.
    attaque = cb.AttaquePionBlanc[i_iCase] & cb.pionn;
    if ( attaque ) return pion;
    // Les cavaliers.
    attaque = cb.AttaqueCavalier[i_iCase] & cb.cavaliern;
    if ( attaque ) return cavalier;
    // Les fous.
    attaque = AttaqueFou(i_iCase ) & cb.foun;
    if ( attaque ) return fou;
    // Les tours.
    attaque = AttaqueTour(i_iCase ) & cb.tourn;
    if ( attaque ) return tour;
    // Les dames.
    attaque = AttaqueDame(i_iCase ) & cb.damen;
    if ( attaque ) return dame;
    // Le roi.
    attaque = cb.AttaqueRoi[i_iCase] & cb.roin;
    if ( attaque ) return roi;
  }

  return 0;
}

// Retourne une map de toutes les cases attaques par cette piece.
Bitboard AttaqueDe(int i_iCase, int wtm)
{
  Bitboard map;

  if ( wtm ) {
    map = cb.AttaqueCavalier[i_iCase] & cb.cavalierb;
    map |= AttaqueTour(i_iCase ) & cb.tourb;
    map |= cb.AttaquePionNoir[i_iCase] & cb.pionb;
    map |= AttaqueFou(i_iCase ) & cb.foub;
    map |= AttaqueDame(i_iCase ) & cb.dameb;
    map |= cb.AttaqueRoi[i_iCase] & cb.roib;
    return map;
  }
  else {
    map = cb.AttaqueCavalier[i_iCase] & cb.cavaliern;
    map |= AttaqueTour(i_iCase) & cb.tourn;
    map |= cb.AttaquePionBlanc[i_iCase] & cb.pionn;
    map |= AttaqueFou(i_iCase) & cb.foun;
    map |= AttaqueDame(i_iCase) & cb.damen;
    map |= cb.AttaqueRoi[i_iCase] & cb.roin;
    return map;
  }
}

//---------------------------------------------------------------------------
