#ifndef __SettingsDialog_h
#define __SettingsDialog_h

#include "KeyPreviewImpl.h"

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog, public KeyPreviewImpl
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

private:
   void initializeLabels();
   void adjustAppearance();
   void loadValues();
   void applyChanges();

   void installChildEventFilter();
   void updateEngineProfilesCombo();

   QString getCanonizedProfileName() const;
   QString getLocalizedProfileName(const QString& profileName) const;
   QStringList getLocalizedProfileNames() const;

   virtual bool keyPreview(int key, Qt::KeyboardModifiers modifiers,
                           bool pressed);
   /*   void on_cmbInterfaceLanguage_currentIndexChanged(QString );
      void on_SettingsDialog_accepted();
      void on_SettingsDialog_rejected();
   */

private:
   Ui::SettingsDialog *ui;
   bool isInitializing_;
};

#endif
