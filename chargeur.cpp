//-----------------------------------------------------------------------------
// Projet: Monik
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: Chargeur.cpp
// Debut: 18 janvier 1999.
//
//---------------------------------------------------------------------------
#include <string>
#include <fstream>
#include <vector>

#include <stdio.h>
#include <sstream>
#include <string.h>
#include <ctype.h>

#include "chess.h"
#include "board.h"

#include "chargeur.h"

using namespace std;

extern int wtm;

bool loadPosition(TChessBoard *cb, const string& filename)
{
  // Verifier si le fichier existe.
  // Si il n'existe pas retourner une erreur.
  std::fstream f;
  f.open(filename, ios::in);
  if (!f.is_open()) {
    return false;
  }
  
  std::string line;
  std::getline(f, line);

  parseFen(cb, line);

  f.close();

  initialiseBitboard(cb);
  
  return true;
}


// La position FEN ce lit comme suit.
// Commencer a la case A8. Lire chaque caractere.
// Les caracteres signifient ceci:
// #   Un chiffre de 1 a 7. Donne le nombre de case vide avant
//     la prochaine piece.
// x   Une piece noir pour les minuscules ou une piece blanche pour
//     les majuscules.
//     P = pion, N = cavalier, B = fou, R = tour, Q = dame, K = king.
// /   Saute a la prochaine rangee.
bool parseFen(TChessBoard *cb, const string& fen) {
  char ch;
  int posCourante = A8;
  uint32 i = 0;
  memset(cb, 0, sizeof(TChessBoard));
  while (fen[i] != ' ' && i < fen.size()) {
      char ch = fen[i];
      if (isdigit(ch)) {
          posCourante += ch - '0';
      } else if (isalpha(ch)) {
          int bWhite = isupper(ch);
          ch = toupper(ch);
          const char* szCode = "PNBRQK";
          const char* pPiece = strchr(szCode, ch);
          if (!pPiece) {
              return false;
          }
          int piece = pPiece - szCode + 1;
          if (!bWhite) {
              piece = -piece;
          }
          cb->board[posCourante++] = (Piece)piece;
      } else if (ch == '/') {
          posCourante = ((posCourante + 7) / 8) * 8;
      }
      i++;
  }

  // C'est a qui a jouer.
  i++;
  ch = fen[i];
  wtm = ch == 'w';

  std::string parsed, input=fen.substr(i + 2);
  std::stringstream input_stringstream(fen.substr(i + 2));
  std::vector<std::string> tokens;
  while(getline(input_stringstream, parsed, ' ')) {
    tokens.push_back(parsed);
  }

  assert(tokens.size() >= 1);
  if (tokens[0].find('K') != std::string::npos) {
    cb->Roque |= ROQUEROIBLANC;
  }
  if (tokens[0].find('Q') != std::string::npos) {
    cb->Roque |= ROQUEDAMEBLANC;
  }
  if (tokens[0].find('k') != std::string::npos) {
    cb->Roque |= ROQUEROINOIR;
  }
  if (tokens[0].find('q') != std::string::npos) {
    cb->Roque |= ROQUEDAMENOIR;
  }  

  // Les coups en passant.
  assert(tokens.size() >= 2);
  std::string enPassant = tokens[1];
  if (enPassant != "-") {
    int colonne = enPassant[0] - 'a';
    int rangee = 8 - (enPassant[1] - '0');
    cb->EnPassant[0] = rangee * 8 + colonne;
  }

  // Le nombre de coups depuis le dernier coup de pion ou de capture.
  cb->Regle50Coup = atoi(tokens[2].c_str());

  // Le nombre de coups.
  cb->NoCoups = atoi(tokens[3].c_str());

  initialiseBitboard(cb);

  return true;
}
