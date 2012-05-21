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
   //enableKeyPreview();
}

SettingsDialog::~SettingsDialog()
{
   //disableKeyPreview();
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
   //ui->cbPondering->setChecked(g_settings.canPonder());
   ui->cbQuickSingleMoveSelection->setChecked(g_settings.quickSingleMoveSelection());
   ui->cbShowMoveHints->setChecked(g_settings.showMoveHints());
   //ui->cbCoordinateMoveInput->setChecked(g_settings.coordinateMoveInput());
   //ui->cbShowGameClock->setChecked(g_settings.showGameClock());
   //
   QStringList engineNames = g_settings.getEngineNames();
   QStringList piecesStyleNames = g_settings.getPiecesStyleNames();
   QStringList localeNames = g_settings.getLocaleNames();
   //
   ui->cmbEngine->addItems(engineNames);
   ui->cmbPiecesStyle->addItems(piecesStyleNames);
   ui->cmbInterfaceLanguage->addItems(localeNames);
   //
   ui->cmbEngine->setCurrentIndex(engineNames.indexOf(g_settings.engineInfo().name));
   ui->cmbPiecesStyle->setCurrentIndex(piecesStyleNames.indexOf(g_settings.piecesStyle()));
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
   playerTimes.append("60 0");
   playerTimes.append(g_label("Unlimited"));
   //
   int idxPlayerTime = playerTimes.indexOf(g_settings.playerClockString());
   //
   if(idxPlayerTime==-1)
   {
      idxPlayerTime = playerTimes.count()-1;
   }
   //
   QStringList engineTimes;
   engineTimes.append("0:10 1");
   engineTimes.append("0:10 2");
   engineTimes.append("0:10 5");
   engineTimes.append("2 1");
   engineTimes.append("2 2");
   engineTimes.append("5 0");
   engineTimes.append("5 1");
   engineTimes.append("5 2");
   engineTimes.append("10 0");
   engineTimes.append("15 0");
   //
   int idxEngineTime = engineTimes.indexOf(g_settings.engineClockString());
   //
   if(idxEngineTime==-1)
   {
      idxEngineTime = engineTimes.count()/2;
   }
   //
   ui->cmbPlayerTime->addItems(playerTimes);
   ui->cmbEngineTime->addItems(engineTimes);
   ui->cmbPlayerTime->setCurrentIndex(idxPlayerTime);
   ui->cmbEngineTime->setCurrentIndex(idxEngineTime);
   //
   updateEngineProfilesCombo();
   //
   if(g_settings.profile().contains("ebook"))
   {
      ui->spinBoardMargins->setEnabled(true);
      ui->spinBoardMargins->setValue(g_settings.boardMargins());
   }
   else
   {
      ui->spinBoardMargins->setEnabled(false);
      ui->spinBoardMargins->setValue(0);
   }
   //
   isInitializing_ = false;
}

void SettingsDialog::initializeLabels(const QString& engineProfile)
{
   bool prevIsInitializing = isInitializing_;
   isInitializing_ = true;
   //
   QSettings ini(g_settings.localeIniFilePath(), QSettings::IniFormat);
   ini.setIniCodec(QTextCodec::codecForName("UTF-8"));
   //
   ui->cbAutoSaveGames->setText(g_label("AutoSaveGames"));
   ui->cbDrawMoveArrow->setText(g_label("DrawMoveArrow"));
   ui->cbDrawCoordinates->setText(g_label("DrawCoordinates"));
   ui->cbShowMoveHints->setText(g_label("ShowMoveHints"));
   ui->cbQuickSingleMoveSelection->setText(g_label("QuickSingleMoveSelection"));
   //ui->cbPondering->setText(g_label("Pondering"));
   //ui->cbCoordinateMoveInput->setText(g_label("CoordinateMoveInput"));
   //ui->cbShowGameClock->setText(g_label("ShowGameClock"));
   ui->lGeneralSettings->setText(g_label("GeneralSettings"));
   ui->lGameControl->setText(g_label("GameControl"));
   ui->lOtherSettings->setText(g_label("OtherSettings"));
   ui->lInterfaceLanguage->setText(g_label("InterfaceLanguage"));
   ui->lPlayerName->setText(g_label("PlayerName"));
   ui->lEngine->setText(g_label("Engine"));
   ui->lEngineProfile->setText(g_label("EngineProfile"));
   ui->lPiecesStyle->setText(g_label("PiecesStyle"));
   ui->lPlayerTime->setText(g_label("PlayerTime"));
   ui->lEngineTime->setText(g_label("EngineTime"));
   //
   ui->lBoardMargins->setText(g_label("BoardMargins"));
   //
   if(ui->cmbPlayerTime->count()>0)
   {
      int k = ui->cmbPlayerTime->count()-1;
      bool lastSelected = ui->cmbPlayerTime->currentIndex()==k;
      ui->cmbPlayerTime->setItemText(k, g_label("Unlimited"));
      if(lastSelected)
         ui->cmbPlayerTime->setCurrentIndex(k);
   }
   //
   updateEngineProfilesCombo(engineProfile);
   //
   isInitializing_ = prevIsInitializing;
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

/*
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
*/

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
   QString engineProfile = getCanonizedProfileName();
   g_settings.setLocaleName(localeName);
   initializeLabels(engineProfile);
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
   //g_settings.setCanPonder(ui->cbPondering->isChecked());
   g_settings.setQuickSingleMoveSelection(ui->cbQuickSingleMoveSelection->isChecked());
   g_settings.setShowMoveHints(ui->cbShowMoveHints->isChecked());
   //g_settings.setCoordinateMoveInput(ui->cbCoordinateMoveInput->isChecked());
   //g_settings.setShowGameClock(ui->cbShowGameClock->isChecked());
   //
   g_settings.setPlayerName(ui->edPlayerName->text().trimmed());
   g_settings.setEngine(ui->cmbEngine->currentText(), getCanonizedProfileName());
   //
   g_settings.setPiecesStyle(ui->cmbPiecesStyle->currentText());
   //
   g_settings.setPlayerClock(ui->cmbPlayerTime->currentText());
   g_settings.setEngineClock(ui->cmbEngineTime->currentText());
   //
   g_settings.setBoardMargins(ui->spinBoardMargins->value());
}

QString SettingsDialog::getCanonizedProfileName() const
{
   const QStringList& profileNames = g_settings.engineInfo(ui->cmbEngine->currentText()).profileNames;
   QString selectedProfileName = ui->cmbEngineProfile->currentText();
   if(selectedProfileName.isEmpty()) return
         g_settings.currentEngineProfile();
   //
   if(selectedProfileName==g_label("Default")) selectedProfileName = "Default";
   else if(selectedProfileName==g_label("Weak")) selectedProfileName = "Weak";
   //
   if(profileNames.contains(selectedProfileName))
      return selectedProfileName;
   else if(profileNames.isEmpty())
      return "Default";
   else
      return profileNames[0];
}

QString SettingsDialog::getLocalizedProfileName(const QString& profileName) const
{
   if(profileName=="Default") return g_label("Default");
   else if(profileName=="Weak") return g_label("Weak");
   else return profileName;
}

QStringList SettingsDialog::getLocalizedProfileNames() const
{
   QStringList localized;
   const QStringList& profileNames = g_settings.engineInfo(ui->cmbEngine->currentText()).profileNames;
   foreach(QString profileName, profileNames)
   {
      localized.push_back(getLocalizedProfileName(profileName));
   }
   return localized;
}

void SettingsDialog::updateEngineProfilesCombo(const QString& engineProfile)
{
   bool prevIsInitializing = isInitializing_;
   isInitializing_ = true;
   //
   QString currentProfile = engineProfile.isEmpty() ?
            getCanonizedProfileName() : engineProfile;
   QStringList profileNames = getLocalizedProfileNames();
   currentProfile = getLocalizedProfileName(currentProfile);
   //
   ui->cmbEngineProfile->clear();
   ui->cmbEngineProfile->addItems(profileNames);
   ui->cmbEngineProfile->setCurrentIndex(profileNames.indexOf(currentProfile));
   //
   isInitializing_ = prevIsInitializing;
}

void SettingsDialog::on_cmbEngine_currentIndexChanged(QString)
{
    if(isInitializing_) return;
    updateEngineProfilesCombo();
}

void SettingsDialog::on_btnOK_clicked()
{
   accept();
}
