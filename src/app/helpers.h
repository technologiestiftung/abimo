#ifndef HELPERS_H
#define HELPERS_H

#include <cmath> // for round
#include <vector>

#include <QCommandLineParser>
#include <QFile>
#include <QHash>
#include <QString>
#include <QStringList>

namespace helpers
{
    QString nowString();
    QString positionalArgOrNULL(QCommandLineParser*, int);
    QString singleQuote(QString);
    QString patternDbfFile();
    QString patternXmlFile();
    QString defaultOutputFileName(QString inputFileName);
    QString defaultLogFileName(QString outputFileName);
    bool containsAll(QHash<QString, int> hash, QStringList keys);
    void openFileOrAbort(QFile& file, QIODevice::OpenModeFlag mode = QIODevice::ReadOnly);
    bool filesAreIdentical(QString file_1, QString file_2, bool debug = true, int maxDiffs = 5);
    void abortIfNoSuchFile(QString filePath, QString context = "");
    bool stringsAreEqual(QString* strings_1, QString* strings_2, int n, int maxDiffs = 5, bool debug = false);
    int stringToInt(QString string, QString context, bool debug = false);
    float stringToFloat(QString string, QString context, bool debug = false);
    int index(float xi, const std::vector<float> &x, float epsilon = 0.0001F);
    float interpolate(float xi, const float *x, const float *y, int n);
    QString removeFileExtension(QString);

    inline float min(float a, float b) {
        return (a < b) ? a : b;
    }

    inline float max(float a, float b) {
        return (a > b) ? a : b;
    }

    inline int roundToInteger(float x) {
        return (int) round(x);
    }
};

#endif // HELPERS_H
