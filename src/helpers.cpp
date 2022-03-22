#include <QCommandLineParser>
#include <QDateTime>
#include <QFileInfo>
#include <QString>
#include <QStringList>

#include "helpers.h"

Helpers::Helpers()
{
}

QString Helpers::nowString()
{
    QDateTime now = QDateTime::currentDateTime();

    return
        "am: " + now.toString("dd.MM.yyyy") +
        " um: " + now.toString("hh:mm:ss");
}

QString Helpers::positionalArgOrNULL(QCommandLineParser* arguments, int index)
{
    const QStringList posArgs = arguments->positionalArguments();
    return (posArgs.length() > index) ? posArgs.at(index) : NULL;
}

QString Helpers::removeFileExtension(QString fileName)
{
    QFileInfo fileInfo(fileName);

    return fileInfo.absolutePath() + "/" + fileInfo.baseName();
}

QString Helpers::singleQuote(QString string)
{
    return "'" + string + "'";
}

QString Helpers::patternDbfFile()
{
    return QString("dBase (*.dbf)");
}

QString Helpers::patternXmlFile()
{
    return QString("Extensible Markup Language (*.xml)");
}

QString Helpers::defaultOutputFileName(QString inputFileName)
{
    return Helpers::removeFileExtension(inputFileName)  + "_out.dbf";
}

QString Helpers::defaultLogFileName(QString outputFileName)
{
    return Helpers::removeFileExtension(outputFileName)  + ".log";
}

// Return true if all keys are contained in the hash, else false
bool Helpers::containsAll(QHash<QString, int> hash, QStringList keys)
{
    for (int i = 0; i < keys.length(); i++) {
        if (! hash.contains(keys.at(i))) {
            return false;
        }
    }

    return true;
}
