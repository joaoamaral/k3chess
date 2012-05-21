#ifndef __SettingsDialog_h
#define __SettingsDialog_h

#include "KeyPreviewImpl.h"

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
   Q_OBJECT

public:
   explicit SettingsDialog(QWidget *parent = 0);
   ~SettingsDialog();

protected:
   virtual void showEvent(QShowEvent *);
   virtual void mousePressEvent(QMouseEvent *);

private slots:

   void on_cmbInterfaceLanguage_currentIndexChanged(QString );
   void on_SettingsDialog_accepted();
   void on_SettingsDialog_rejected();

   void on_cmbEngine_currentIndexChanged(QString );

   void on_btnOK_clicked();

private:
   void initializeLabels(const QString& engineProfile=QString());
   void adjustAppearance();
   void loadValues();
   void applyChanges();

   void updateEngineProfilesCombo(const QString& engineProfile=QString());

   QString getCanonizedProfileName() const;
   QString getLocalizedProfileName(const QString& profileName) const;
   QStringList getLocalizedProfileNames() const;

private:
   Ui::SettingsDialog *ui;
   bool isInitializing_;
};

#endif
