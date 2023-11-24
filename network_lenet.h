#include <dlib/dnn.h>
#include <dlib/data_io.h>

using dlib::relu;
using dlib::con;
using dlib::max_pool;
using dlib::fc;
using dlib::loss_multiclass_log;
using dlib::input;
using dlib::matrix;

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
