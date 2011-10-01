#ifndef __ChessBoardView_h
#define __ChessBoardView_h

#include <QWidget>
#include <QImage>
#include <QString>
#include <QPen>
#include <QFont>

#include "ChessPosition.h"
#include "ChessMoveMap.h"

class ChessBoardView : public QWidget
{
   Q_OBJECT

   enum LastKeyType { ktColumn, ktRow, ktPiece, ktOther };

public:
   ChessBoardView(QWidget *parent);

   int padding() const;                     // chess board border width

   void updatePosition(const ChessPosition& position, // updates position and accompanying data (if provided)
                       const CoordPair& lastMove=CoordPair(),
                       const ChessMoveMap& possibleMoves=ChessMoveMap());
   void beginMoveSelection(); // enable move selection for current side (ends in signal moveEntered)
   void cancelMoveSelection();              // cancels move selection and hides move selection cursor
   void setDrawCoords(bool);                // toggle coords along board sides
   void setFlipped(bool);                   // toggle board flipped display
   void setPadding(unsigned);               // sets padding around the board (note: padding is required if you want to see coordinates)
   void setPiecesImage(const QImage& image); // sets image to be used for drawing board squares and pieces
   void setDrawMoveArrow(bool value);
   void setShowMoveHints(bool value);
   void setQuickSingleMoveSelection(bool value); // if selected piece has only one allowed move, make that move

   bool hasCursor() const;
   bool enter();  // to return to active board with suspended selection, returns false if board is not active
   void suspendSelection();
   void setInitialCursorPos(ChessCoord coord); // sets initial cursor position for next move selection

   void setKeyCoordSelect(bool value);     // set to true to allow selecting pieces by pressing column letters or row numbers
                                           // (e.g. pressing 'c' letter places cursor on the first piece in 'c' column,
                                           //  pressing 'c' once again will move to next piece on that column, pressing row number
                                           //  '5' will select the piece on row '5' if it is available)
   void setKeyPieceSelect(bool value);      // set to true to allow selecting pieces by pressing keys 'K', 'Q', 'R', 'B', 'N', 'P'
                                            // (e.g. pressing 'K' will select the king, pressing 'R' will select first rook,
                                            //  pressing 'R' another time will select second rook)
                                            // if keys column select is also on, use 'I' instead of 'B' for bIshops

   bool processKey(Qt::Key key);  // process key (can be called directly to avoid complex keypress event routing)

signals:
   void moveEntered(const CoordPair& pair); // user entered a move

protected:
   virtual void keyPressEvent(QKeyEvent*);
   virtual void paintEvent(QPaintEvent*);
   virtual void resizeEvent(QResizeEvent*);
   virtual void mousePressEvent(QMouseEvent *);

private:

   enum CursorDirection { cursorLeft, cursorUp, cursorDown, cursorRight };
   enum CellSelectMode { selectNone, selectSource, selectTarget };

   QRect getMoveRect(const CoordPair& move) const;
   QRect getCellRect(ChessCoord coord) const;
   QRect boardCellsRect() const; // returns a rect encompassing all cells on the board (excluding board border)

   ChessCoord chessCoordFromPoint(const QPoint& p) const;

   void flipDirection(CursorDirection& dir);

   void initGraphicObjects();
   void updateScaledPieces();

   void drawBoard(QPainter& painter, const QRect& clipRect);
   void drawPiece(QPainter& painter, const QRect& rect, ChessPiece piece, bool blackCell);
   void drawCellHighlight(QPainter& painter, const QRect& rect, bool bold);
   void drawMoveArrow(QPainter& painter, const QRect& fromRect, const QRect& toRect);
   void showMoveHints(QPainter& painter, ChessCoord& coord, const QRect& clipRect);
   void drawBorder(QPainter& painter, const QRect& clipRect);
   void drawCoords(QPainter& painter, const QRect& clipRect);

   void moveCursor(CursorDirection dir);
   void select();
   bool cancel();
   void advanceCoord(ChessCoord& coord, CursorDirection dir);

   bool checkColumnSelectionKey(Qt::Key key);
   bool checkRowSelectionKey(Qt::Key key);
   bool checkPieceSelectionKey(Qt::Key key);
   void columnSelect(int col);
   void rowSelect(int row);
   void pieceSelect(PieceType pt);

private:
   ChessPosition position_;
   CoordPair lastMove_;
   ChessMoveMap possibleMoves_;
   bool flipped_;
   CellSelectMode cellSelectMode_;
   bool drawCoords_;
   CoordPair move_;            // current move entered by user (if move input enabled)
   ChessCoord lastCursorPos_;  // remembered when move is entered or move input is cancelled
   unsigned padding_;          // padding around the board for coordinates, etc. (fixed!)
   QPen thinPen_;
   QPen thickPen_;
   QBrush borderBrush_;
   QFont horizCoordsFont_;
   QFont vertCoordsFont_;
   QImage originalPieces_;
   bool drawMoveArrow_;
   bool showMoveHints_;
   bool moveSelectionSuspended_;
   bool keyCoordSelect_;
   bool keyPieceSelect_;
   bool quickSingleMoveSelection_;
   LastKeyType lastKeyType_;
   std::vector<QImage> scaledPieces_;
};

#endif
