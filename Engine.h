//-----------------------------------------------------------------------------
// Projet: Monik           Copyright (C) 1998-2014. All right reserved.
// Auteur: Sylvain Lacombe.

#ifndef engineH
#define engineH

#include "chess.h"

enum EngineMode
{
	Force,
	Edit,
	Play
};

class Engine
{
public:
	static int InputMove(char* text, int ply, int wtm, TMove& move);

	Engine();
	void Edit( TChessBoard& cb, const char* i_szCommande, char* o_szReponse );
	void Entree( char* o_szCommande );
	bool Run(const char *i_szCommande, char* o_szReponse);

private:
	EngineMode Mode;
	bool WhiteToMove;

	bool Option( const char* i_szCommande, char* o_szReponse );
	static bool Parse(const char* i_szEntree, int ply, int wtm, TMove& o_Move);
};

//---------------------------------------------------------------------------
#endif
