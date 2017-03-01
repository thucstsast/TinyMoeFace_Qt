//Based on http://omg-it.works/how-to-grab-video-frames-directly-from-qcamera/

#ifndef SIMPLECAMERASURFACE_H
#define SIMPLECAMERASURFACE_H

#include <QtMultimedia/QAbstractVideoSurface>

class SimpleCameraSurface : public QAbstractVideoSurface
{
    Q_OBJECT
protected:
    QList<QVideoFrame::PixelFormat> pixelFormats;
public:
    SimpleCameraSurface(QObject* parent);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;
    bool present(const QVideoFrame &frame);

signals:
    void frameAvailable(QImage frame);
};

#endif // SIMPLECAMERASURFACE_H
