#ifndef __MoveSelectionHelper_h
#define __MoveSelectionHelper_h

#include "ChessMoveMap.h"
#include "ChessPosition.h"

namespace MoveSelectionHelper
{

ChessCoord getNextPieceByCol(const ChessPosition& position, const ChessMoveMap& moves,
                             ChessCoord coord, int col, bool preferSamePieceType=false);
ChessCoord getNextPieceByType(const ChessPosition& position, const ChessMoveMap& moves,
                              ChessCoord coord, PieceType pt);
ChessCoord getNextTargetByCol(const ChessPosition& position, const ChessMoveMap& moves,
                              const CoordPair& cpair, int col, bool& isUnique);
ChessCoord getNextTargetByRow(const ChessPosition& position, const ChessMoveMap& moves,
                              const CoordPair& cpair, int row, bool& isUnique);
ChessCoord getNextTargetByPiece(const ChessPosition& position, const ChessMoveMap& moves,
                                const CoordPair& cpair, PieceType pt, bool& isUnique);

}

#endif
