//---------------------------------------------------------------------------
#ifndef EvalH
#define EvalH

#include "chess.h"
#include "board.h"

#define DOUBLED_PAWN_PENALTY		10
#define ISOLATED_PAWN_PENALTY		20
#define BACKWARDS_PAWN_PENALTY		12
#define PASSED_PAWN_BONUS			10
#define KNIGHT_OUTPOST_BONUS		6
#define	KNIGHT_DOUBLE_PROT_BONUS	2
#define NO_FIANCHETTO_PEN			10
#define ROOK_SEMI_OPEN_FILE_BONUS	10
#define ROOK_OPEN_FILE_BONUS		15
#define ROOK_ON_SEVENTH_BONUS		20
#define ALL_PAWNS_PENALTY			20
#define MORE_PAWMS_BONUS			50
#define MORE_PIECES_BONUS			30
#define EXCHANGE_PEN				5

int Eval(int ply, int wtm, int alpha, int beta);
SCORE EvaluateMaterial(int wtm);
int EvaluerMat();
int ProtectionRoiBlanc(int ply);
int ProtectionRoiNoir(int ply);
SCORE EvaluerCoursePionPasses(int wtm);

//---------------------------------------------------------------------------
#endif
