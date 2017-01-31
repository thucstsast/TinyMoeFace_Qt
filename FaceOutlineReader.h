#ifndef FACEOUTLINEREADER_H
#define FACEOUTLINEREADER_H

#include <QVector>
#include <QPointF>

class QString;
class QFile;
class QXmlStreamReader;

class FaceOutlineReader
{
protected:
    QFile *svgFile = nullptr;
    QXmlStreamReader *xmlReader;
    QVector<QPointF> parseSvgPathAttribute(const QString& attr);
public:
    FaceOutlineReader();
    bool openFile(const QString& fileName);
    bool closeFile();
    bool parseSvgOutline();
};

#endif // FACEOUTLINEREADER_H
