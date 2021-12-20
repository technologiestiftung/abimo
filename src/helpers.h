#ifndef HELPERS_H
#define HELPERS_H

#include <QCommandLineParser>
#include <QString>

class Helpers
{
public:
    Helpers();
    static QString nowString();
    static QString positionalArgOrNULL(QCommandLineParser*, int);
    static QString singleQuote(QString);
    static QString patternDbfFile();
    static QString patternXmlFile();
    static QString defaultOutputFileName(QString inputFileName);
    static QString defaultLogFileName(QString outputFileName);

private:
    static QString removeFileExtension(QString);
};

#endif // HELPERS_H
