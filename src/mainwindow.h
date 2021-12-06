/***************************************************************************
 *   Copyright (C) 2009 by Meiko Rachimow, Claus Rachimow                  *
 *   This file is part of Abimo 3.2                                        *
 *   Abimo 3.2 is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QLabel>
#include <QXmlSimpleReader>

#include "saxhandler.h"
#include "dbaseReader.h"
#include "calculation.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication*);
    ~MainWindow();

private slots:
    void processEvent(int, QString);
    void about();
    void computeFile(QString = NULL, QString = NULL, QString = NULL, QString = NULL);
    void userCancel();

private:
    const char* programName;
    void setText(QString);
    void critical(QString);
    void warning(QString);
    QString selectDbfFile(QString, QString, bool);
    QString removeFileExtension(QString);
    QString singleQuote(QString);
    void reportSuccess(Calculation*, QTextStream&, QString, QString);
    void reportCancelled(QTextStream&);
    InitValues updateInitialValues(QString);
    QAction *openAct;
    QAction *aboutAct;
    QLabel *textfield;
    QProgressDialog * progress;
    bool userStop;
    Calculation* calc;
    QApplication* app;
    QString folder;
    QWidget *widget;
    QString nowString();
};

#endif
