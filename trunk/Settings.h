#ifndef __K3ChessSettings_h
#define __K3ChessSettings_h

#include <QSettings>
#include <QDir>

#include "Singletons.h"
#include "GameProfile.h"
#include "EngineInfo.h"

#include <map>

class Profile
{
public:
   static Profile fromString(const QString& str);
   bool contains(const QString& str) const;
private:
   QStringList keywords_;
};

class K3ChessSettings : public QObject
{
   Q_OBJECT

   friend void Singletons::initialize();
   friend void Singletons::finalize();

   K3ChessSettings();
   ~K3ChessSettings();

public:

   const EngineInfo& engineInfo() const;
   QString piecesStyle() const;
   QString localeName() const;

   const EngineInfo& engineInfo(const QString& engineName) const;
   QString currentEngineProfile() const;

   QString pieceImageFilePath() const;
   QString localeIniFilePath() const;

   QString pgnFilePath() const;
   QString playerName() const;

   ChessClock playerClock() const;
   ChessClock engineClock() const;

   QString playerClockString() const;
   QString engineClockString() const;

   bool drawCoordinates() const;  // show coordinates on board sides
   bool drawMoveArrow() const;    // mark last move with an 'arrow' on board
   bool showMoveHints() const; // draw hints on board squares that the current piece can move to
   bool showGameClock() const; // show chess clock for both players during the game
   bool coordinateMoveInput() const; // moves are selected by entering direct coordinates in col-row form (e.g. e2e4)
   bool quickSingleMoveSelection() const; // if selected piece has only one allowed move, make that move
   bool keyColumnSelect() const;  // pressing keys 'a'..'h' will select pieces on corresponding columns
   bool keyPieceSelect() const;   // pressing keys 'K', 'Q', 'R', 'B' ('I'), 'P' will select pieces of corresponding types

   QString keymapFile() const;
   QString lastGameFile() const;

   bool autoSaveGames() const; // if true, games will be saved to pgn file automatically, without prompt
   bool canPonder() const; // global ponder setting (may be overridden by internal engine parameters)
   const Profile& profile() const; // use "Profile" setting for tuning program behavior for various handheld devices, e-books, etc.
                                   // a profile consists of one or more keywords separated by semicolos
                                   // e.g. "ebook;grayscale;6-inch" or "netbook;truecolor;widescreen" etc.
                                   // probe for keywords by calling profile().contains(keyword)

   QStringList getEngineNames() const;
   QStringList getPiecesStyleNames() const;
   QStringList getLocaleNames() const;

   void setEngine(const QString& engineName, const QString& profileName);
   void setPiecesStyle(const QString& name);
   void setLocaleName(const QString& name);

   void setCanPonder(bool value);
   void setDrawCoordinates(bool value);
   void setDrawMoveArrow(bool value);
   void setShowMoveHints(bool value);
   void setShowGameClock(bool value);
   void setCoordinateMoveInput(bool value);
   void setQuickSingleMoveSelection(bool value);
   void setAutoSaveGames(bool value);
   void setPlayerName(const QString& name);
   void setPlayerClock(const QString& str);
   void setEngineClock(const QString& str);

   bool isChess960() const;
   void setChess960(bool value);

   QString initialPositionFen() const;
   void setInitialPositionFen(const QString& fen);

   void flush();

signals:
   void engineChanged();
   void boardStyleChanged();
   void inputSettingsChanged();
   void timeSettingsChanged();
   void ponderingChanged();
   void playerNameChanged();
   void localeChanged();

private:
   void enumEngines(QDir dir);
   void enumEnginesProc(QDir dir);
   bool readEngineInfo(const QString& engineIniFile, EngineInfo& info);
   void enumPiecesStyles(QDir dir);
   void enumLocales(QDir dir);

   QString pieceImageFileFromName(const QString& name) const;
   QString localeIniFilePathFromName(const QString& name) const;

   void initKeyboardProfile();

private:
   QSettings settings_;
   ChessClock playerClock_;
   ChessClock engineClock_;
   Profile profile_;
   std::map<QString, EngineInfo> engines_; // maps engine names to engine executable paths
   std::map<QString, QString> piecesStyles_; // maps piece style names to piece image paths
   std::map<QString, QString> locales_; // maps locale names to locale ini files
};

#endif
