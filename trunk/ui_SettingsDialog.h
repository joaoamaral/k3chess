/********************************************************************************
** Form generated from reading UI file 'SettingsDialog.ui'
**
** Created: Sat Oct 1 17:37:03 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QLabel *lEngine;
    QComboBox *cmbEngine;
    QComboBox *cmbPieceStyle;
    QLabel *lPieceStyle;
    QLabel *lGameControl;
    QLabel *lPlayerTime;
    QLabel *lEngineTime;
    QComboBox *cmbPlayerTime;
    QComboBox *cmbEngineTime;
    QCheckBox *cbPondering;
    QLabel *lOtherSettings;
    QLabel *lGeneralSettings;
    QLabel *lPlayerName;
    QLineEdit *edPlayerName;
    QLabel *lInterfaceLanguage;
    QComboBox *cmbInterfaceLanguage;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QCheckBox *cbDrawMoveArrow;
    QCheckBox *cbDrawCoordinates;
    QCheckBox *cbShowMoveHints;
    QCheckBox *cbQuickSingleMoveSelection;
    QCheckBox *cbAutoSaveGames;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
        SettingsDialog->resize(600, 744);
        QFont font;
        font.setFamily(QString::fromUtf8("Sans Serif"));
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        SettingsDialog->setFont(font);
        SettingsDialog->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lEngine = new QLabel(SettingsDialog);
        lEngine->setObjectName(QString::fromUtf8("lEngine"));
        lEngine->setGeometry(QRect(80, 129, 201, 16));
        QFont font1;
        font1.setPointSize(11);
        lEngine->setFont(font1);
        lEngine->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmbEngine = new QComboBox(SettingsDialog);
        cmbEngine->setObjectName(QString::fromUtf8("cmbEngine"));
        cmbEngine->setGeometry(QRect(300, 123, 231, 30));
        cmbEngine->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmbPieceStyle = new QComboBox(SettingsDialog);
        cmbPieceStyle->setObjectName(QString::fromUtf8("cmbPieceStyle"));
        cmbPieceStyle->setGeometry(QRect(300, 173, 230, 30));
        lPieceStyle = new QLabel(SettingsDialog);
        lPieceStyle->setObjectName(QString::fromUtf8("lPieceStyle"));
        lPieceStyle->setGeometry(QRect(80, 180, 201, 16));
        lPieceStyle->setFont(font1);
        lPieceStyle->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lGameControl = new QLabel(SettingsDialog);
        lGameControl->setObjectName(QString::fromUtf8("lGameControl"));
        lGameControl->setGeometry(QRect(40, 240, 511, 16));
        lGameControl->setFont(font1);
        lGameControl->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lPlayerTime = new QLabel(SettingsDialog);
        lPlayerTime->setObjectName(QString::fromUtf8("lPlayerTime"));
        lPlayerTime->setGeometry(QRect(80, 280, 211, 16));
        lPlayerTime->setFont(font1);
        lPlayerTime->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lEngineTime = new QLabel(SettingsDialog);
        lEngineTime->setObjectName(QString::fromUtf8("lEngineTime"));
        lEngineTime->setGeometry(QRect(80, 329, 211, 16));
        lEngineTime->setFont(font1);
        lEngineTime->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmbPlayerTime = new QComboBox(SettingsDialog);
        cmbPlayerTime->setObjectName(QString::fromUtf8("cmbPlayerTime"));
        cmbPlayerTime->setGeometry(QRect(300, 273, 231, 30));
        cmbPlayerTime->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmbEngineTime = new QComboBox(SettingsDialog);
        cmbEngineTime->setObjectName(QString::fromUtf8("cmbEngineTime"));
        cmbEngineTime->setGeometry(QRect(300, 320, 230, 30));
        cmbEngineTime->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cbPondering = new QCheckBox(SettingsDialog);
        cbPondering->setObjectName(QString::fromUtf8("cbPondering"));
        cbPondering->setGeometry(QRect(120, 380, 441, 18));
        lOtherSettings = new QLabel(SettingsDialog);
        lOtherSettings->setObjectName(QString::fromUtf8("lOtherSettings"));
        lOtherSettings->setGeometry(QRect(40, 440, 501, 16));
        lOtherSettings->setFont(font1);
        lOtherSettings->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lGeneralSettings = new QLabel(SettingsDialog);
        lGeneralSettings->setObjectName(QString::fromUtf8("lGeneralSettings"));
        lGeneralSettings->setGeometry(QRect(40, 40, 521, 16));
        QFont font2;
        font2.setPointSize(11);
        font2.setItalic(false);
        lGeneralSettings->setFont(font2);
        lGeneralSettings->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        lGeneralSettings->setText(QString::fromUtf8("General settings"));
        lPlayerName = new QLabel(SettingsDialog);
        lPlayerName->setObjectName(QString::fromUtf8("lPlayerName"));
        lPlayerName->setGeometry(QRect(80, 80, 201, 16));
        lPlayerName->setFont(font1);
        lPlayerName->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        edPlayerName = new QLineEdit(SettingsDialog);
        edPlayerName->setObjectName(QString::fromUtf8("edPlayerName"));
        edPlayerName->setGeometry(QRect(300, 73, 231, 31));
        lInterfaceLanguage = new QLabel(SettingsDialog);
        lInterfaceLanguage->setObjectName(QString::fromUtf8("lInterfaceLanguage"));
        lInterfaceLanguage->setGeometry(QRect(40, 686, 211, 16));
        lInterfaceLanguage->setFont(font1);
        lInterfaceLanguage->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        cmbInterfaceLanguage = new QComboBox(SettingsDialog);
        cmbInterfaceLanguage->setObjectName(QString::fromUtf8("cmbInterfaceLanguage"));
        cmbInterfaceLanguage->setGeometry(QRect(300, 680, 231, 30));
        cmbInterfaceLanguage->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        layoutWidget = new QWidget(SettingsDialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(80, 480, 341, 161));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        cbDrawMoveArrow = new QCheckBox(layoutWidget);
        cbDrawMoveArrow->setObjectName(QString::fromUtf8("cbDrawMoveArrow"));

        verticalLayout->addWidget(cbDrawMoveArrow);

        cbDrawCoordinates = new QCheckBox(layoutWidget);
        cbDrawCoordinates->setObjectName(QString::fromUtf8("cbDrawCoordinates"));

        verticalLayout->addWidget(cbDrawCoordinates);

        cbShowMoveHints = new QCheckBox(layoutWidget);
        cbShowMoveHints->setObjectName(QString::fromUtf8("cbShowMoveHints"));

        verticalLayout->addWidget(cbShowMoveHints);

        cbQuickSingleMoveSelection = new QCheckBox(layoutWidget);
        cbQuickSingleMoveSelection->setObjectName(QString::fromUtf8("cbQuickSingleMoveSelection"));

        verticalLayout->addWidget(cbQuickSingleMoveSelection);

        cbAutoSaveGames = new QCheckBox(layoutWidget);
        cbAutoSaveGames->setObjectName(QString::fromUtf8("cbAutoSaveGames"));

        verticalLayout->addWidget(cbAutoSaveGames);

        layoutWidget->raise();
        lEngine->raise();
        cmbEngine->raise();
        cmbPieceStyle->raise();
        lPieceStyle->raise();
        lGameControl->raise();
        lPlayerTime->raise();
        lEngineTime->raise();
        cmbPlayerTime->raise();
        cmbEngineTime->raise();
        cbPondering->raise();
        lOtherSettings->raise();
        lGeneralSettings->raise();
        lPlayerName->raise();
        edPlayerName->raise();
        lInterfaceLanguage->raise();
        cmbInterfaceLanguage->raise();
        QWidget::setTabOrder(edPlayerName, cmbEngine);
        QWidget::setTabOrder(cmbEngine, cmbPieceStyle);
        QWidget::setTabOrder(cmbPieceStyle, cmbPlayerTime);
        QWidget::setTabOrder(cmbPlayerTime, cmbEngineTime);
        QWidget::setTabOrder(cmbEngineTime, cbPondering);
        QWidget::setTabOrder(cbPondering, cbDrawMoveArrow);
        QWidget::setTabOrder(cbDrawMoveArrow, cbDrawCoordinates);
        QWidget::setTabOrder(cbDrawCoordinates, cbAutoSaveGames);
        QWidget::setTabOrder(cbAutoSaveGames, cmbInterfaceLanguage);

        retranslateUi(SettingsDialog);

        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        lEngine->setText(QApplication::translate("SettingsDialog", "Engine", 0, QApplication::UnicodeUTF8));
        lPieceStyle->setText(QApplication::translate("SettingsDialog", "Piece style", 0, QApplication::UnicodeUTF8));
        lGameControl->setText(QApplication::translate("SettingsDialog", "Game control", 0, QApplication::UnicodeUTF8));
        lPlayerTime->setText(QApplication::translate("SettingsDialog", "Player time", 0, QApplication::UnicodeUTF8));
        lEngineTime->setText(QApplication::translate("SettingsDialog", "Engine time", 0, QApplication::UnicodeUTF8));
        cbPondering->setText(QApplication::translate("SettingsDialog", "Pondering", 0, QApplication::UnicodeUTF8));
        lOtherSettings->setText(QApplication::translate("SettingsDialog", "Other settings", 0, QApplication::UnicodeUTF8));
        lPlayerName->setText(QApplication::translate("SettingsDialog", "Player name", 0, QApplication::UnicodeUTF8));
        lInterfaceLanguage->setText(QApplication::translate("SettingsDialog", "Interface language", 0, QApplication::UnicodeUTF8));
        cbDrawMoveArrow->setText(QApplication::translate("SettingsDialog", "Draw last move arrow", 0, QApplication::UnicodeUTF8));
        cbDrawCoordinates->setText(QApplication::translate("SettingsDialog", "Draw coordinates on board sides", 0, QApplication::UnicodeUTF8));
        cbShowMoveHints->setText(QApplication::translate("SettingsDialog", "Show move hints", 0, QApplication::UnicodeUTF8));
        cbQuickSingleMoveSelection->setText(QApplication::translate("SettingsDialog", "Allow quick selection of single moves", 0, QApplication::UnicodeUTF8));
        cbAutoSaveGames->setText(QApplication::translate("SettingsDialog", "Auto save games", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
