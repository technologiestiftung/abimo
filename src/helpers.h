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
    static QString removeFileExtension(QString);
    static QString singleQuote(QString);
};

#endif // HELPERS_H
