#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
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

void Helpers::openFileOrAbort(QFile& file, QIODevice::OpenModeFlag mode)
{
    if (!file.open(mode)) {
        qDebug() << "Cannot open file: " << file.fileName() << ": " << file.errorString();
        abort();
    }
}

bool Helpers::filesAreIdentical(QString fileName_1, QString fileName_2, bool debug)
{
    if (debug) {
        qDebug() << "Comparing two files: ";
        qDebug() << "File 1: " << fileName_1;
        qDebug() << "File 2: " << fileName_2;
    }

    QFile file_1(fileName_1);
    QFile file_2(fileName_2);

    openFileOrAbort(file_1);
    openFileOrAbort(file_2);

    QByteArray blob_1 = file_1.readAll();
    QByteArray blob_2 = file_2.readAll();

    bool result = true;

    if (blob_1.length() != blob_2.length()) {
        result = false;
    }

    if (result) {
        for (int i = 0; i < blob_1.length(); i++) {
            if (blob_1[i] != blob_2[i]) {
                result = false;
            }
        }
    }

    if (debug) {
        qDebug() << QString("The files %1 identical.").arg(result ? "are": "are not");
    }

    return result;
}
