#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"
#include "Settings.h"
#include "StyleSheets.h"
#include "GlobalStrings.h"
#include "KeyMapper.h"

#include <QStringList>
#include <QTextCodec>

SettingsDialog::SettingsDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::SettingsDialog),
   isInitializing_(false)
{
   ui->setupUi(this);
   //
   initializeLabels();
   adjustAppearance();
   loadValues();
   //
   QObject::connect(this, SIGNAL(accepted()), this, SLOT(on_SettingsDialog_accepted()), Qt::UniqueConnection);
   QObject::connect(this, SIGNAL(rejected()), this, SLOT(on_SettingsDialog_rejected()), Qt::UniqueConnection);
   //
   enableKeyPreview();
}

SettingsDialog::~SettingsDialog()
{
   disableKeyPreview();
   //
   delete ui;
}

void SettingsDialog::loadValues()
{
   isInitializing_ = true;
   //
   ui->cbAutoSaveGames->setChecked(g_settings.autoSaveGames());
   ui->cbDrawMoveArrow->setChecked(g_settings.drawMoveArrow());
   ui->cbDrawCoordinates->setChecked(g_settings.drawCoordinates());
   ui->cbPondering->setChecked(g_settings.canPonder());
   ui->cbQuickSingleMoveSelection->setChecked(g_settings.quickSingleMoveSelection());
   ui->cbShowMoveHints->setChecked(g_settings.showMoveHints());
   //
   QStringList engineNames = g_settings.getEngineNames();
   QStringList pieceStyleNames = g_settings.getPieceStyleNames();
   QStringList localeNames = g_settings.getLocaleNames();
   //
   ui->cmbEngine->addItems(engineNames);
   ui->cmbPieceStyle->addItems(pieceStyleNames);
   ui->cmbInterfaceLanguage->addItems(localeNames);
   //
   ui->cmbEngine->setCurrentIndex(engineNames.indexOf(g_settings.engineInfo().name));
   ui->cmbPieceStyle->setCurrentIndex(pieceStyleNames.indexOf(g_settings.pieceStyle()));
   ui->cmbInterfaceLanguage->setCurrentIndex(localeNames.indexOf(g_settings.localeName()));
   //
   ui->edPlayerName->setText(g_settings.playerName());
   //
   QStringList playerTimes;
   playerTimes.append("2 2");
   playerTimes.append("5 0");
   playerTimes.append("5 2");
   playerTimes.append("10 0");
   playerTimes.append("15 0");
   playerTimes.append("30 0");
   playerTimes.append(g_label("Unlimited"));
   //
   ui->cmbPlayerTime->addItems(playerTimes);
   //
   int idx = playerTimes.indexOf(g_settings.playerClockString());
   if(idx>=0)
   {
      ui->cmbPlayerTime->setCurrentIndex(idx);
   }
   else if(g_settings.playerClockString()=="--")
   {
      ui->cmbPlayerTime->setCurrentIndex(ui->cmbPlayerTime->count()-1);
   }
   //
   QStringList engineTimes;
   engineTimes.append("0:10 2");
   engineTimes.append("0:10 5");
   engineTimes.append("2 2");
   engineTimes.append("5 0");
   engineTimes.append("5 2");
   engineTimes.append("10 0");
   engineTimes.append("15 0");
   ui->cmbEngineTime->addItems(engineTimes);
   //
   int eidx = playerTimes.indexOf(g_settings.playerClockString());
   if(eidx>=0)
   {
      ui->cmbEngineTime->setCurrentIndex(eidx);
   }
   else if(g_settings.engineClockString()=="--")
   {
      ui->cmbEngineTime->setCurrentIndex(ui->cmbEngineTime->count()-1);
   }
   //
   isInitializing_ = false;
}

void SettingsDialog::initializeLabels()
{
   QSettings ini(g_settings.localeIniFilePath(), QSettings::IniFormat);
   ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
   //
   ui->cbAutoSaveGames->setText(g_label("AutoSaveGames"));
   ui->cbDrawMoveArrow->setText(g_label("DrawMoveArrow"));
   ui->cbDrawCoordinates->setText(g_label("DrawCoordinates"));
   ui->cbShowMoveHints->setText(g_label("ShowMoveHints"));
   ui->cbQuickSingleMoveSelection->setText(g_label("QuickSingleMoveSelection"));
   ui->cbPondering->setText(g_label("Pondering"));
   ui->lGeneralSettings->setText(g_label("GeneralSettings"));
   ui->lGameControl->setText(g_label("GameControl"));
   ui->lOtherSettings->setText(g_label("OtherSettings"));
   ui->lInterfaceLanguage->setText(g_label("InterfaceLanguage"));
   ui->lPlayerName->setText(g_label("PlayerName"));
   ui->lEngine->setText(g_label("Engine"));
   ui->lPieceStyle->setText(g_label("PieceStyle"));
   ui->lPlayerTime->setText(g_label("PlayerTime"));
   ui->lEngineTime->setText(g_label("EngineTime"));
   //
   if(ui->cmbPlayerTime->count()>0)
   {
      int idx = ui->cmbPlayerTime->currentIndex();
      ui->cmbPlayerTime->removeItem(ui->cmbPlayerTime->count()-1);
      ui->cmbPlayerTime->addItem(g_label("Unlimited"));
      ui->cmbPlayerTime->setCurrentIndex(idx);
   }
}

void SettingsDialog::adjustAppearance()
{
   // @@todo: possibly add layout and other visual adjustments for various profiles
   // see g_settings.profile()
   if(g_settings.profile().contains("ebook"))
   {
      setStyleSheet(cUIStyleSheet_Ebook);
   }
}

bool SettingsDialog::keyPreview(int key, Qt::KeyboardModifiers modifiers,
                                bool pressed)
{
   switch(g_keyMapper.getQtKey(key))
   {
      case Qt::Key_Menu:
         if(pressed)
         {
            accept();
         }
         return true;
      default:
         return false;
   }
}

void SettingsDialog::showEvent(QShowEvent *)
{
   ui->edPlayerName->setFocus(Qt::MouseFocusReason);
}

void SettingsDialog::mousePressEvent(QMouseEvent *)
{
   // support for stylus/mouse devices: close settings dialog
   // by clicking somewhere outside dialog controls
   accept();
}

void SettingsDialog::on_cmbInterfaceLanguage_currentIndexChanged(QString localeName)
{
   if(isInitializing_) return;
   if(localeName==g_settings.localeName()) return;
   g_settings.setLocaleName(localeName);
   initializeLabels();
}

void SettingsDialog::on_SettingsDialog_accepted()
{
   applyChanges();
}

void SettingsDialog::on_SettingsDialog_rejected()
{
   applyChanges();
}

void SettingsDialog::applyChanges()
{
   g_settings.setAutoSaveGames(ui->cbAutoSaveGames->isChecked());
   g_settings.setDrawMoveArrow(ui->cbDrawMoveArrow->isChecked());
   g_settings.setDrawCoordinates(ui->cbDrawCoordinates->isChecked());
   g_settings.setCanPonder(ui->cbPondering->isChecked());
   g_settings.setQuickSingleMoveSelection(ui->cbQuickSingleMoveSelection->isChecked());
   g_settings.setShowMoveHints(ui->cbShowMoveHints->isChecked());
   //
   g_settings.setPlayerName(ui->edPlayerName->text().trimmed());
   g_settings.setEngineName(ui->cmbEngine->currentText());
   g_settings.setPieceStyle(ui->cmbPieceStyle->currentText());
   //
   g_settings.setPlayerClock(ui->cmbPlayerTime->currentText());
   g_settings.setEngineClock(ui->cmbEngineTime->currentText());
}
