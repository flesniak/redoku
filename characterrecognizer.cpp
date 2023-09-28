#include "characterrecognizer.h"

#include <QImage>
#include <QDebug>
#include <QFile>

#include <sstream>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

using dlib::relu;
using dlib::con;
using dlib::max_pool;
using dlib::inception3;
using dlib::inception4;
using dlib::fc;
using dlib::loss_multiclass_log;
using dlib::input;
using dlib::matrix;

// #define INCEPTION

#ifndef INCEPTION
    // Now let's define the LeNet.  Broadly speaking, there are 3 parts to a network
    // definition.  The loss layer, a bunch of computational layers, and then an input
    // layer.  You can see these components in the network definition below.
    //
    // The input layer here says the network expects to be given matrix<unsigned char>
    // objects as input.  In general, you can use any dlib image or matrix type here, or
    // even define your own types by creating custom input layers.
    //
    // Then the middle layers define the computation the network will do to transform the
    // input into whatever we want.  Here we run the image through multiple convolutions,
    // ReLU units, max pooling operations, and then finally a fully connected layer that
    // converts the whole thing into just 10 numbers.
    //
    // Finally, the loss layer defines the relationship between the network outputs, our 10
    // numbers, and the labels in our dataset.  Since we selected loss_multiclass_log it
    // means we want to do multiclass classification with our network.   Moreover, the
    // number of network outputs (i.e. 10) is the number of possible labels.  Whichever
    // network output is largest is the predicted label.  So for example, if the first
    // network output is largest then the predicted digit is 0, if the last network output
    // is largest then the predicted digit is 9.
    using net_type = loss_multiclass_log<
                                fc<10,
                                relu<fc<84,
                                relu<fc<120,
                                max_pool<2,2,2,2,relu<con<16,5,5,1,1,
                                max_pool<2,2,2,2,relu<con<6,5,5,1,1,
                                input<matrix<unsigned char>>
                                >>>>>>>>>>>>;
    // This net_type defines the entire network architecture.  For example, the block
    // relu<fc<84,SUBNET>> means we take the output from the subnetwork, pass it through a
    // fully connected layer with 84 outputs, then apply ReLU.  Similarly, a block of
    // max_pool<2,2,2,2,relu<con<16,5,5,1,1,SUBNET>>> means we apply 16 convolutions with a
    // 5x5 filter size and 1x1 stride to the output of a subnetwork, then apply ReLU, then
    // perform max pooling with a 2x2 window and 2x2 stride.
#else
    template <typename SUBNET> using block_a1 = relu<con<10,1,1,1,1,SUBNET>>;
    template <typename SUBNET> using block_a2 = relu<con<10,3,3,1,1,relu<con<16,1,1,1,1,SUBNET>>>>;
    template <typename SUBNET> using block_a3 = relu<con<10,5,5,1,1,relu<con<16,1,1,1,1,SUBNET>>>>;
    template <typename SUBNET> using block_a4 = relu<con<10,1,1,1,1,max_pool<3,3,1,1,SUBNET>>>;

    // Here is inception layer definition. It uses different blocks to process input
    // and returns combined output.  Dlib includes a number of these inceptionN
    // layer types which are themselves created using concat layers.
    template <typename SUBNET> using incept_a = inception4<block_a1,block_a2,block_a3,block_a4, SUBNET>;

    // Network can have inception layers of different structure.  It will work
    // properly so long as all the sub-blocks inside a particular inception block
    // output tensors with the same number of rows and columns.
    template <typename SUBNET> using block_b1 = relu<con<4,1,1,1,1,SUBNET>>;
    template <typename SUBNET> using block_b2 = relu<con<4,3,3,1,1,SUBNET>>;
    template <typename SUBNET> using block_b3 = relu<con<4,1,1,1,1,max_pool<3,3,1,1,SUBNET>>>;
    template <typename SUBNET> using incept_b = inception3<block_b1,block_b2,block_b3,SUBNET>;

    // Now we can define a simple network for classifying MNIST digits.  We will
    // train and test this network in the code below.
    using net_type = loss_multiclass_log<
            fc<10,
            relu<fc<32,
            max_pool<2,2,2,2,incept_b<
            max_pool<2,2,2,2,incept_a<
            dlib::input<dlib::matrix<unsigned char>>
            >>>>>>>>;
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

    const unsigned long pred = m_net->net(dlibImage);

    return QString(QChar(ushort('A' + pred)));
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
