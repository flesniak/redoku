// The contents of this file are in the public domain. See LICENSE_FOR_EXAMPLE_PROGRAMS.txt
/*
    This is an example illustrating the use of the deep learning tools from the
    dlib C++ Library.  In it, we will train the venerable LeNet convolutional
    neural network to recognize hand written digits.  The network will take as
    input a small image and classify it as one of the 10 numeric digits between
    0 and 9.

    The specific network we will run is from the paper
        LeCun, Yann, et al. "Gradient-based learning applied to document recognition."
        Proceedings of the IEEE 86.11 (1998): 2278-2324.
    except that we replace the sigmoid non-linearities with rectified linear units.

    These tools will use CUDA and cuDNN to drastically accelerate network
    training and testing.  CMake should automatically find them if they are
    installed and configure things appropriately.  If not, the program will
    still run but will be much slower to execute.
*/

#include <iostream>
#include <sstream>
#include <fstream>

#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <dlib/cuda/cuda_dlib.h>

using namespace std;
using namespace dlib;

// #define INCEPTION
#ifdef INCEPTION
    #include "network_inception.h"
#else
    #include "network_lenet.h"
#endif

int main(int argc, char** argv) try
{
    int n = cuda::get_num_devices();
    cout << n << " cuda devices" << endl;
    for (int i=0; i<n; i++) {
        cout << "device " << i << ": " << cuda::get_device_name(i) << endl;
    }
    char* cuda_dev = secure_getenv("CUDA_DEVICE");
    int cuda_dev_idx = 0;
    if (cuda_dev != nullptr)
        cuda_dev_idx = atoi(cuda_dev);
    cuda::set_device(cuda_dev_idx);
    cout << "using cuda device " << cuda_dev_idx << ": " << cuda::get_device_name(cuda_dev_idx) << endl;

    // This example is going to run on the MNIST dataset.
    if (argc != 2)
    {
        cout << "This example needs the MNIST dataset to run!" << endl;
        cout << "You can get MNIST from http://yann.lecun.com/exdb/mnist/" << endl;
        cout << "Download the 4 files that comprise the dataset, decompress them, and" << endl;
        cout << "put them in a folder.  Then give that folder as input to this program." << endl;
        return 1;
    }

    // MNIST is broken into two parts, a training set of 60000 images and a test set of
    // 10000 images.  Each image is labeled so that we know what hand written digit is
    // depicted.  These next statements load the dataset into memory.
    std::vector<matrix<unsigned char>> training_images;
    std::vector<unsigned long>         training_labels;
    std::vector<matrix<unsigned char>> testing_images;
    std::vector<unsigned long>         testing_labels;

    // std::ifstream data(argv[1]);
    // std::string line;
    // while(std::getline(data,line)) {
    //     std::stringstream lineStream(line);
    //     std::string cell;


    //     if (!std::getline(lineStream, cell, ',')) {
    //         cerr << "failed to get first cell" << endl;
    //         return 1;
    //     }

    //     int letter = stoi(cell);
    //     if (letter > 25) {
    //         cerr << "invalid letter " << letter << endl;
    //         return 1;
    //     }

    //     matrix<unsigned char, 28, 28> picture;
    //     int col = 0;
    //     int row = 0;
    //     while(std::getline(lineStream, cell, ',')) {
    //         if (row > 27) {
    //             cerr << "invalid row " << training_images.size() << endl;
    //             return 1;
    //         }
    //         picture(col, row) = stoi(cell);
    //         col++;
    //         if (col > 27) {
    //             col = 0;
    //             row++;
    //         }
    //     }
    //     training_images.push_back(picture);
    //     training_labels.push_back(letter);
    // }
    // std::cout << training_images.size() << std::endl;

    // randomize_samples(training_images, training_labels);
    // for (size_t i=0; i<training_images.size() / 3; i++) {
    //     testing_images.push_back(training_images[training_images.size() - i - 1]);
    //     testing_labels.push_back(training_labels[training_images.size() - i - 1]);
    // }
    // training_images.resize(training_images.size() - testing_images.size());
    // training_labels.resize(training_labels.size() - testing_labels.size());


    load_mnist_dataset(argv[1], training_images, training_labels, testing_images, testing_labels);


    // So with that out of the way, we can make a network instance.
    net_type net;
    // And then train it using the MNIST data.  The code below uses mini-batch stochastic
    // gradient descent with an initial learning rate of 0.01 to accomplish this.
    dnn_trainer<net_type> trainer(net);
    trainer.set_iterations_without_progress_threshold(3000);
    trainer.set_learning_rate(0.01);
    trainer.set_min_learning_rate(0.00001);
    trainer.set_mini_batch_size(512);
    trainer.be_verbose();
    // Since DNN training can take a long time, we can ask the trainer to save its state to
    // a file named "mnist_sync" every 20 seconds.  This way, if we kill this program and
    // start it again it will begin where it left off rather than restarting the training
    // from scratch.  This is because, when the program restarts, this call to
    // set_synchronization_file() will automatically reload the settings from mnist_sync if
    // the file exists.
    trainer.set_synchronization_file("mnist_sync", std::chrono::seconds(20));
    // Finally, this line begins training.  By default, it runs SGD with our specified
    // learning rate until the loss stops decreasing.  Then it reduces the learning rate by
    // a factor of 10 and continues running until the loss stops decreasing again.  It will
    // keep doing this until the learning rate has dropped below the min learning rate
    // defined above or the maximum number of epochs as been executed (defaulted to 10000).
    trainer.train(training_images, training_labels);

    // At this point our net object should have learned how to classify MNIST images.  But
    // before we try it out let's save it to disk.  Note that, since the trainer has been
    // running images through the network, net will have a bunch of state in it related to
    // the last batch of images it processed (e.g. outputs from each layer).  Since we
    // don't care about saving that kind of stuff to disk we can tell the network to forget
    // about that kind of transient data so that our file will be smaller.  We do this by
    // "cleaning" the network before saving it.
    net.clean();
    serialize("mnist_network.dat") << net;
    // Now if we later wanted to recall the network from disk we can simply say:
    // deserialize("mnist_network.dat") >> net;


    // Now let's run the training images through the network.  This statement runs all the
    // images through it and asks the loss layer to convert the network's raw output into
    // labels.  In our case, these labels are the numbers between 0 and 9.
    std::vector<unsigned long> predicted_labels = net(training_images);
    int num_right = 0;
    int num_wrong = 0;
    // And then let's see if it classified them correctly.
    for (size_t i = 0; i < training_images.size(); ++i)
    {
        if (predicted_labels[i] == training_labels[i])
            ++num_right;
        else
            ++num_wrong;

    }
    cout << "training num_right: " << num_right << endl;
    cout << "training num_wrong: " << num_wrong << endl;
    cout << "training accuracy:  " << num_right/(double)(num_right+num_wrong) << endl;

    // Let's also see if the network can correctly classify the testing images.  Since
    // MNIST is an easy dataset, we should see at least 99% accuracy.
    predicted_labels = net(testing_images);
    num_right = 0;
    num_wrong = 0;
    for (size_t i = 0; i < testing_images.size(); ++i)
    {
        if (predicted_labels[i] == testing_labels[i])
            ++num_right;
        else
            ++num_wrong;

    }
    cout << "testing num_right: " << num_right << endl;
    cout << "testing num_wrong: " << num_wrong << endl;
    cout << "testing accuracy:  " << num_right/(double)(num_right+num_wrong) << endl;


    // Finally, you can also save network parameters to XML files if you want to do
    // something with the network in another tool.  For example, you could use dlib's
    // tools/convert_dlib_nets_to_caffe to convert the network to a caffe model.
    net_to_xml(net, "lenet.xml");
}
catch(std::exception& e)
{
    cout << e.what() << endl;
}

