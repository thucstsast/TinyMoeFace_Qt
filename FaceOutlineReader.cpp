#include <QDebug>
#include <QFile>
#include <QXmlStreamReader>
#include "FaceOutlineReader.h"

QVector<QPointF> FaceOutlineReader::parseSvgPathAttribute(const QString &attr)
{
    QChar lastCommand;
    QVector<QPointF> ret;
    QStringList parts = attr.split(' ');
    for(int i = 0; i < parts.size(); i++)
    {
        const QString& part = parts[i];
        //qDebug() << part << part[0].isLetter();
        if(part[0].isLetter())
        {
            //qDebug() << "Update " << part;
            Q_ASSERT(part.length() == 1);
            lastCommand = part[0];
        }
        else
        {
            //qDebug() << lastCommand;
            //qDebug() << part;
            if(lastCommand == 'M' || lastCommand == 'm')
            {
                QStringList temp = part.split(',');
                Q_ASSERT(temp.size() == 2);
                QPointF p(temp[0].toFloat(), temp[1].toFloat());
                ret.append(p);
                if(lastCommand == 'm')
                    lastCommand = 'l';
                else
                    lastCommand = 'L';
            }
            else if(lastCommand == 'L' || lastCommand == 'l')
            {
                QStringList temp = part.split(',');
                Q_ASSERT(temp.size() == 2);
                QPointF p(temp[0].toFloat(), temp[1].toFloat());
                if(lastCommand == 'l')
                {
                    p += ret.last();
                }
                ret.append(p);
            }
            else if(lastCommand == 'V' || lastCommand == 'v')
            {
                QPointF p(0, part.toFloat());
                if(lastCommand == 'v')
                {
                    p += ret.last();
                }
                ret.append(p);
            }
            else if(lastCommand == 'H' || lastCommand == 'h')
            {
                QPointF p(part.toFloat(), 0);
                if(lastCommand == 'h')
                {
                    p += ret.last();
                }
                ret.append(p);
            }
            else
            {
                Q_ASSERT(lastCommand == 'Z');
            }
        }
    }
    return ret;
}

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
    svgFile->open(QIODevice::ReadOnly);
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

bool FaceOutlineReader::parseSvgOutline(float width, float height)
{
    QXmlStreamReader::TokenType tokenType;
    float viewBoxWidth = -1.0f;
    float viewBoxHeight = -1.0f;
    while(true)
    {
        tokenType = xmlReader->readNext();
        if(tokenType == QXmlStreamReader::EndDocument)
        {
            break;
        }
        if(tokenType != QXmlStreamReader::StartElement)
        {
            continue;
        }
        auto attributes = xmlReader->attributes();
        if(xmlReader->name() != "path" && xmlReader->name() != "svg")
        {
            continue;
        }
        if(xmlReader->name() == "path")
        {
            Q_ASSERT(viewBoxWidth > 0.0f && viewBoxHeight > 0.0f);
            QString outlineName = attributes.value("id").toString();
            QString outlinePath = attributes.value("d").toString();
            QVector<QPointF> outline = parseSvgPathAttribute(outlinePath);
            qDebug() << viewBoxWidth << width << viewBoxHeight << height;
            for(int i = 0; i < outline.size(); i++)
            {
                outline[i].setX(outline[i].x() / viewBoxWidth * width);
                outline[i].setY(outline[i].y() / viewBoxHeight * height);
            }
            outlines.insert(outlineName, outline);
        }
        else if(xmlReader->name() == "svg")
        {
            QString temp = attributes.value("viewBox").toString();
            QStringList viewBoxParts = temp.split(' ');
            Q_ASSERT(viewBoxParts.size() == 4);
            viewBoxWidth = viewBoxParts[2].toFloat();
            viewBoxHeight = viewBoxParts[3].toFloat();
        }
    }
}

const QMap<QString, QVector<QPointF> > &FaceOutlineReader::getOutlines()
{
    return outlines;
}
