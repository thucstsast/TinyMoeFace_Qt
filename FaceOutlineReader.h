#ifndef FACEOUTLINEREADER_H
#define FACEOUTLINEREADER_H

#include <QMap>
#include <QVector>
#include <QPointF>

class QString;
class QFile;
class QXmlStreamReader;
class QImage;

class FaceOutlineReader
{
protected:
    QFile *svgFile = nullptr;
    QXmlStreamReader *xmlReader;
    QMap<QString, QVector<QPointF> > outlines;
    QVector<QPointF> parseSvgPathAttribute(const QString& attr);
public:
    FaceOutlineReader();
    bool openFile(const QString& fileName);
    bool closeFile();
    bool parseSvgOutline(float width, float height);
    const QMap<QString, QVector<QPointF> > &getOutlines();
};

#endif // FACEOUTLINEREADER_H
