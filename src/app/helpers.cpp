#include <QCommandLineParser>
#include <QDateTime>
#include <QDebug>
#include <QDir>
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

bool Helpers::filesAreIdentical(QString fileName_1, QString fileName_2, bool debug, int maxDiffs)
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

    bool result = true;

    if (file_1.size() != file_2.size()) {
        result = false;
    }

    if (result) {

        QByteArray blob_1 = file_1.readAll();
        QByteArray blob_2 = file_2.readAll();

        int i = 0;
        int n_diffs = 0;

        while(i < blob_1.length() && n_diffs < maxDiffs) {
            if (blob_1[i] != blob_2[i]) {
                n_diffs++;
                qDebug() << QString("%1. byte difference at index %2").arg(
                    QString::number(n_diffs),
                    QString::number(i)
                );
            }
            i++;
        }

        result = (n_diffs == 0);
    }

    if (debug) {
        qDebug() << QString("The files %1 identical.").arg(result ? "are": "are not");
    }

    return result;
}

void Helpers::abortIfNoSuchFile(QString filePath, QString context)
{
    if (!QFile::exists(filePath)) {
        qDebug() << "File does not exist: " << filePath;
        qDebug() << "Current directory: " << QDir::currentPath();

        if (!context.isEmpty()) {
            qDebug() << context;
        }

        abort();
    }
}

bool Helpers::stringsAreEqual(QString* strings_1, QString* strings_2, int n, int maxDiffs)
{
    int n_diffs = 0;
    int i = 0;

    while (i < n && n_diffs < maxDiffs) {
        if (*strings_1 != *strings_2) {
            n_diffs++;
            qDebug() << QString(
                "%1. string mismatch at index %2:").arg(
                QString::number(n_diffs),
                QString::number(i)
            );
            qDebug() << QString("  string 1: '%1'").arg(*strings_1);
            qDebug() << QString("  string 2: '%1'").arg(*strings_2);
        }
        strings_1++;
        strings_2++;
        i++;
    }

    return (n_diffs == 0);
}
