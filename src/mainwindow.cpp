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

#include "mainwindow.h"

MainWindow::MainWindow(QApplication* app):
    QMainWindow(),
    programName("Abimo 3.2"),
    userStop(false),
    calc(0),
    app(app),
    folder("/")
{
    openAct = new QAction(tr("&Compute File"), this);
    openAct->setShortcut(tr("Ctrl+C"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(computeFile()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setShortcut(tr("Ctrl+A"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    menuBar()->addAction(openAct);
    menuBar()->addAction(aboutAct);

    setWindowTitle(tr(programName));
    resize(350, 150);

    w = new QWidget();
    setCentralWidget(w);

    textfield = new QLabel("Willkommen...", w);
    textfield->setMargin(4);
    textfield->setFont(QFont("Arial", 8, QFont::Bold));

    progress = new QProgressDialog( "Lese Datei.", "Abbrechen", 0, 50, this, 0);
    progress->setWindowTitle(tr(programName));
    progress->setModal(true);
    progress->setMinimumDuration (0);
    connect(progress, SIGNAL(canceled()), this, SLOT(userCancel()));
}

MainWindow::~MainWindow()
{
    delete textfield;
    delete openAct;
    delete aboutAct;
    delete progress;
    delete w;
}

void MainWindow::processEvent(int i, QString str) {
    progress->setValue(i);
    progress->setLabelText(str);
    app->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void MainWindow::userCancel()
{
    userStop = true;
    setText("Berechnungen abgebrochen.");

    if (calc != 0) {
        calc->stop();
    }

    app->processEvents(QEventLoop::ExcludeUserInputEvents);
}

void MainWindow::about()
{
    QMessageBox::about(
        this,
        tr("About") + tr(programName),
        tr("Claus & Meiko Rachimow\nCopyright 2009")
    );
}

void MainWindow::setText(QString info)
{
    textfield->setText(info);
    textfield->adjustSize();
}

void MainWindow::critical(QString str)
{
    progress->close();
    QMessageBox::critical(this, programName, str);
}

InitValues MainWindow::updateInitialValues(QString configFileName)
{
    InitValues initValues;
    QFile initFile(configFileName);

    if (! initFile.exists()) {
        QMessageBox::warning(
            this, programName,
            "Keine '" + configFileName + "': gefunden.\n"
            "Nutze Standardwerte."
        );

        return initValues;
    }

    QXmlSimpleReader xmlReader;
    QXmlInputSource data(&initFile);

    SaxHandler handler(initValues);

    xmlReader.setContentHandler(&handler);
    xmlReader.setErrorHandler(&handler);

    if (!xmlReader.parse(&data)) {
        QMessageBox::warning(
            this, programName,
            "'" + configFileName + "': korrupte Datei.\n"
            "Nutze Standardwerte."
        );
    }
    else if (!initValues.allSet()) {
        QMessageBox::warning(
            this, programName,
            "'" + configFileName + "': fehlende Werte.\n"
            "Ergaenze mit Standardwerten."
        );
    }

    initFile.close();

    return initValues;
}

void MainWindow::computeFile()
{
    QString file = QFileDialog::getOpenFileName(
        this,
        "Daten einlesen von...",
        folder,
        "dBase (*.dbf)"
    );

    if (file == NULL) {
        return;
    }

    setText("Lese Quelldatei...");
    repaint();
    //processEvent(0, "Lese Datei.");

    // Open a DBASE File
    DbaseReader dbReader(file);

    if (! dbReader.read()) {
        critical(
            "Problem beim Oeffnen der Datei: '" + file + "' aufgetreten.\n"
            "Grund: " + dbReader.getError()
        );
        return;
    }

    if (! dbReader.isAbimoFile()) {
        critical(
            "Die Datei '" + file + "' ist kein valider 'Input File',\n" +
            "Ueberpruefen sie die Spaltennamen und die Vollstaendigkeit."
        );
        return;
    }

    // Read initial values from XML
    QString configFileName("config.xml");

    // Update initial values with values given in config.xml
    InitValues initValues = updateInitialValues(configFileName);

    setText("Quelldatei eingelesen, waehlen sie eine Zieldatei...");
    userStop = false;

    QFileInfo infoFile(file);
    folder = infoFile.absolutePath();
    QString base = infoFile.baseName();
    QString outFileString(folder + "/" + base + "out.dbf");

    QString outFile = QFileDialog::getSaveFileName(
        this,
        "Ergebnisse schreiben nach...",
        outFileString,
        "dBase (*.dbf)"
    );

    if (outFile == NULL) {
        setText("Willkommen...");
        return;
    }

    QFileInfo infoOutFile(outFile);
    QString folderOut = infoOutFile.absolutePath();
    QString baseOut = infoOutFile.baseName();
    QString protokollFileName(folderOut + "/" + baseOut + "Protokoll.txt");

    setText("Bitte Warten...");
    processEvent(0, "Lese Datei.");

    // Protokoll
    QFile protokollFile(protokollFileName);

    if (! protokollFile.open(QFile::WriteOnly)) {
        critical("Konnte Datei: '" + protokollFileName + "' nicht oeffnen.\n" + protokollFile.error());
        return;
    }

    QTextStream protokollStream(&protokollFile);

    // Start the Calculation
    protokollStream << "Start der Berechnung am: " + QDateTime::currentDateTime().toString("dd.MM.yyyy") + " um: " + QDateTime::currentDateTime().toString("hh:mm:ss") + "\r\n";

    calc = new Calculation(dbReader, initValues, protokollStream);
    connect(calc, SIGNAL(processSignal(int, QString)), this, SLOT(processEvent(int, QString)));

    if (calc->calc(outFile)) {

        if (! userStop) {

            QString protCount;
            QString nutzungIstNull;
            QString keineFlaechenAngegeben;
            QString readRecCount;
            QString writeRecCount;

            protCount.setNum(calc->getProtCount());
            nutzungIstNull.setNum(calc->getNutzungIstNull());
            keineFlaechenAngegeben.setNum(calc->getKeineFlaechenAngegeben());
            readRecCount.setNum(calc->getTotalRecRead());
            writeRecCount.setNum(calc->getTotalRecWrite());

            setText(
                "Berechnungen mit " + protCount + " Fehlern beendet.\n"
                "Eingelesene Records: " + readRecCount +"\n"
                "Geschriebene Records: " + writeRecCount +"\n"
                "Ergebnisse in Datei: '" + outFile + "' geschrieben.\n"
                "Protokoll in Datei: '" + protokollFileName + "' geschrieben."
            );

            protokollStream << "\r\nBei der Berechnung traten " << protCount << " Fehler auf.\r\n";

            if (calc->getKeineFlaechenAngegeben() != 0) {
                protokollStream << "\r\nBei " + keineFlaechenAngegeben +
                    " Flaechen deren Wert 0 war wurde 100 eingesetzt.\r\n";
            }

            if (calc->getNutzungIstNull() != 0) {
                protokollStream << "\r\nBei " + nutzungIstNull +
                    " Records war die Nutzung 0, diese wurden ignoriert.\r\n";
            }

            if (calc->getTotalBERtoZeroForced() != 0) {
                protokollStream << "\r\nBei " << calc->getTotalBERtoZeroForced() <<
                    " Records wurde BER==0 erzwungen.\r\n";
            }

            protokollStream << "\r\nEingelesene Records: " + readRecCount + "\r\n";
            protokollStream << "\r\nGeschriebene Records: " + writeRecCount + "\r\n";
            protokollStream << "\r\nEnde der Berechnung am: " +
                QDateTime::currentDateTime().toString("dd.MM.yyyy") + " um: " +
                QDateTime::currentDateTime().toString("hh:mm:ss") + "\r\n";
        }
    }
    else {
        critical(calc->getError());
    }

    delete calc;
    calc = 0;
    protokollFile.close();
}
