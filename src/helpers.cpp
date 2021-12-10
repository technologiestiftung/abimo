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
