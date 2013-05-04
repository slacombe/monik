//-----------------------------------------------------------------------------
// Projet: SLChess
// Auteur: Sylvain Lacombe.
// Debut du projet: 2 novembre 1998.
// Fichier: TJournal.h
// Debut: 2 novembre 1998.
// Description: Permet d'ecrire dans un log.
//
//---------------------------------------------------------------------------
#ifndef TJournalH
#define TJournalH

class TJournal
{
  char m_szFichier[255];
public:
  TJournal();
  ~TJournal();
  void Nouveau();
  void Log( const char* i_szMessage, ... );
  void logBoard();
};

extern TJournal journal;

//---------------------------------------------------------------------------
#endif
 
