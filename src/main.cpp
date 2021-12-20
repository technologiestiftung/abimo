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

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QtDebug>

#include "calculation.h"
#include "constants.h"
#include "dbaseReader.h"
#include "helpers.h"
#include "initvalues.h"
#include "mainwindow.h"

bool parseForBatch(int &argc, char *argv[])
{
    //return false;
    return argc > 1;

    /*
    bool isBatch = false;

    for (int i = 0; i < argc; i++) {

        qDebug() << "Argument arg[" << i << "]: " << argv[i];

        if (! qstrcmp(argv[i], "--batch"))
            isBatch = true;
    }

    return isBatch;
    */
}

void defineParser(QCommandLineParser* parser)
{
    parser->setApplicationDescription("Abimo-Programm");
    parser->addHelpOption();
    parser->addVersionOption();

    parser->addPositionalArgument(
        "source",
        QCoreApplication::translate("main", "Input dbf-file.")
    );

    parser->addPositionalArgument(
        "destination",
        QCoreApplication::translate("main", "Destination dbf-file (optional)."),
        "[destination]"
    );

    // Option -d --debug: debug mode
    QCommandLineOption debugOption(
        QStringList() << "d" << "debug",
        QCoreApplication::translate("main", "Show debug information.")
    );

    // Option -c --config <config-file>
    QCommandLineOption configOption(
        QStringList() << "c" << "config",
        QCoreApplication::translate("main", "Override initial values with values in 'config.xml'"),
        QCoreApplication::translate("main", "config-file"));

    parser->addOption(debugOption);
    parser->addOption(configOption);
}

void debugInputs(
    QString inputFileName,
    QString outputFileName,
    QString configFileName,
    QString logFileName,
    bool debug
)
{
    qDebug() << "Running in batch mode...";
    qDebug() << "inputFileName =" << inputFileName;
    qDebug() << "outputFileName =" << outputFileName;
    qDebug() << "configFile =" << configFileName;
    qDebug() << "logFileName =" << logFileName;
    qDebug() << "debug =" << debug;

}

// Options/arguments for example call on the command line
// --config ..\config.xml ..\abimo_2012ges.dbf ..\abimo-result.dbf

int main(int argc, char *argv[])
{
    qDebug() << "Number of command line arguments: " << argc;

    const QString appVersion = VERSION_STRING;

    if (parseForBatch(argc, argv)) {

        // Start batch version...
        QCoreApplication app(argc, argv);
        app.setApplicationVersion(appVersion);

        QCommandLineParser parser;
        defineParser(&parser);

        // Process the actual command line arguments given by the user
        parser.process(app);

        const QStringList positionalArgs = parser.positionalArguments();
        QString inputFileName = positionalArgs.at(0);
        QString outputFileName = positionalArgs.at(1);
        QString configFileName= parser.value("config");
        QString logFileName = Helpers::removeFileExtension(inputFileName) + "Log.txt";
        bool debug = parser.isSet("debug");

        debugInputs(inputFileName, outputFileName, configFileName, logFileName, debug);

        DbaseReader dbReader(inputFileName);

        if (! dbReader.checkAndRead()) {
            qDebug() << dbReader.getFullError();
            return 2;
        }

        // Update default initial values with values given in config.xml
        InitValues initValues;
        QString errorMessage = InitValues::updateFromConfig(initValues, configFileName);

        if (errorMessage.length() > 0) {
            qDebug() << "Error: " << errorMessage;
        }

        QFile logFile(logFileName);

        if (! logFile.open(QFile::WriteOnly)) {
            qDebug() << "Konnte Datei: '" << logFileName <<
                "' nicht oeffnen.\n" << logFile.error();
            return 1;
        }

        QTextStream logStream(&logFile);

        logStream << "Start der Berechnung " + Helpers::nowString() + "\r\n";

        // Create calculator object
        Calculation calculator(dbReader, initValues, logStream);

        qDebug() << "Start the calculation";
        calculator.calc(outputFileName);
        qDebug() << "End of calculation (Results are in " << outputFileName << ").";

        return -1;
    }
    else {

        // Start GUI version...
        QApplication app(argc, argv);
        app.setApplicationVersion(appVersion);

        QCommandLineParser parser;
        defineParser(&parser);

        // Process the actual command line arguments given by the user
        parser.process(app);

        MainWindow mainWin(&app, &parser);
        mainWin.show();

        return app.exec();
    }
}
