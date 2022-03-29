#ifndef HELPERS_H
#define HELPERS_H

#include <QCommandLineParser>
#include <QFile>
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
    static void openFileOrAbort(QFile& file, QIODevice::OpenModeFlag mode = QIODevice::ReadOnly);
    static bool filesAreIdentical(QString file_1, QString file_2, bool debug = true);
    static void abortIfNoSuchFile(QString filePath, QString context = "");

private:
    static QString removeFileExtension(QString);
};

#endif // HELPERS_H
