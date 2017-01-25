#ifndef FACEOUTLINEREADER_H
#define FACEOUTLINEREADER_H

class QString;
class QFile;
class QXmlStreamReader;

class FaceOutlineReader
{
protected:
    QFile *svgFile = nullptr;
    QXmlStreamReader *xmlReader;
public:
    FaceOutlineReader();
    bool openFile(const QString& fileName);
    bool closeFile();
    bool parseSvgOutline();
};

#endif // FACEOUTLINEREADER_H
