#ifndef HELPERS_H
#define HELPERS_H

#include <QCommandLineParser>
#include <QHash>
#include <QString>
#include <QStringList>

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
    static bool containsAll(QHash<QString, int> hash, QStringList keys);

private:
    static QString removeFileExtension(QString);
};

#endif // HELPERS_H
