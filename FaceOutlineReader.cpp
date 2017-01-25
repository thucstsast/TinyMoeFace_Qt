#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include "FaceOutlineReader.h"

FaceOutlineReader::FaceOutlineReader()
{

}

bool FaceOutlineReader::openFile(const QString &fileName)
{
    if(svgFile != nullptr)
    {
        return false;
    }
    svgFile = new QFile(fileName);
    if(!svgFile->isOpen())
    {
        delete svgFile;
        svgFile = nullptr;
        return false;
    }
    xmlReader = new QXmlStreamReader(svgFile);
    return true;
}

bool FaceOutlineReader::closeFile()
{
    if(svgFile == nullptr)
    {
        return false;
    }
    delete xmlReader;
    svgFile->close();
    delete svgFile;
    return true;
}

bool FaceOutlineReader::parseSvgOutline()
{
    if(xmlReader->readNext() == QXmlStreamReader::Invalid)
    {
        return false;
    }
    qDebug() << xmlReader->tokenType();
}
