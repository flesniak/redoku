#include "characterrecognizer.h"

#include <QImage>
#include <QDebug>
#include <QFile>

#include <sstream>

// #define INCEPTION
#ifdef INCEPTION
    #include "network_inception.h"
#else
    #include "network_lenet.h"
#endif

//struct Network : public net_type { } *CharacterRecognizer::m_net;
struct Network
{
    net_type net;
};

CharacterRecognizer *CharacterRecognizer::instance()
{
    static CharacterRecognizer inst;
    return &inst;
}

QString CharacterRecognizer::recognize(const QImage &image)
{
    if (image.width() != 28 || image.height() != 28) {
        qWarning() << "Invalid image geometry" << image.size();
        return "!";
    }
    if (image.format() != QImage::Format_Grayscale8) {
        qWarning() << "Invalid format" << image.format();
        return "-";
    }

    dlib::matrix<unsigned char> dlibImage(28, 28);
    for (int y=0; y<image.height(); y++) {
        const uchar *line = image.scanLine(y);
        for (int x=0; x<image.width(); x++) {
            dlibImage(x, y) = line[x];
        }
    }
    for (int y=0; y<image.height(); y++) {
        for (int x=0; x<image.width(); x++) {
            std::cout << (int)dlibImage(x,y) << " ";
        }
        std::cout << std::endl;
    }

    const unsigned long pred = m_net->net(dlibImage);
    qDebug() << "prediction:" << pred;

    return QString(QChar(ushort('0' + pred)));
}

CharacterRecognizer::CharacterRecognizer(QObject *parent) : QObject(parent)
{
    m_net = new Network;
    QFile netFile(":/net.dat");
    if (!netFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to load net file";
        return;
    }
    std::string netData = netFile.readAll().toStdString();
    std::istringstream istr(netData);
    dlib::deserialize(m_net->net, istr);
    qDebug() << "Network ready";
}
