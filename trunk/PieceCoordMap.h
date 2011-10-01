#ifndef __PieceCoordMap_h
#define __PieceCoordMap_h

#include "ChessMoveMap.h"
#include "ChessPosition.h"

#include <map>
#include <list>
#include <vector>

struct PieceCoord : public ChessPiece, public ChessCoord
{
};

class PieceCoordMap
{
public:
    PieceCoordMap();

    void loadFrom(const ChessPosition& position, const ChessMoveMap& possibleMoves);

    PieceCoord getNextPieceByRow(int row);
    PieceCoord getNextPieceByCol(int col);
    PieceCoord getNextPieceByType(PieceType pt);
    ChessCoord getNextTargetCellByCol(int col);

private:
    typedef std::list<PieceCoord> PieceCoordList;
    std::vector<PieceCoordList> piecesOnRow_;
    std::vector<PieceCoordList> piecesOnCol_;
    PieceCoord lastPieceCoord_;
    ChessMoveMap possibleMoves_;
    ChessMoveMap::const_iterator itMove_;
};

#endif
