///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016, STEREOLABS.
//
// All rights reserved.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////


/****************************************************************************************************
 ** This sample demonstrates how to grab images and depth map with the ZED SDK in parallel threads **
 ***************************************************************************************************/


#include <stdio.h>
#include <string.h>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>


#include "functions.hpp"
#include <zed/Camera.hpp>
#include <zed/utils/GlobalDefine.hpp>

using namespace cv;
using namespace sl::zed;
using namespace std;
// Exchange structure

Camera* zed;
image_buffer* buffer;
SENSING_MODE dm_type = STANDARD;
bool stop_signal;
int count_run = 0;
bool newFrame = false;
long long camera_ts = 0;

void grab_run() {

    uchar* p_left;
    uchar* p_right;


    while (!stop_signal) {

        if (!zed->grab(dm_type)) {

            p_left = zed->retrieveImage(SIDE::RIGHT).data; // Get the pointer
            // Fill the buffer


            buffer->mutex_input_image.lock();
            memcpy(buffer->data_im, p_left, buffer->width * buffer->height * buffer->im_channels * sizeof (uchar));
            //memcpy(buffer->data_imr, p_right, buffer->width * buffer->height * sizeof (uchar)*buffer->im_channels);
        buffer->mutex_input_image.unlock();

        p_right = zed->retrieveImage(SIDE::LEFT).data; // Get the pointer
        buffer->mutex_input_imgr.lock(); // To prevent from data corruption
            memcpy(buffer->data_imr, p_right, buffer->width * buffer->height * sizeof (uchar)*buffer->im_channels);
            buffer->mutex_input_imgr.unlock();

            newFrame = true;
            count_run++;
        } else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

        // Stop loop if no more frame to grab
        if (zed->getSVOPosition() == zed->getSVONumberOfFrames() - 2)
            stop_signal = true;
    }
}

void zed_capture(char** av, vector<string> &imagelist) {
    stop_signal = false;
    ZEDResolution_mode model=VGA;
    switch (int(av[1][0])){
//    case("1") model=VGA;
    case '2': model=HD720;
    case '3': model=HD1080;
    default:;
    }
        zed = new Camera(model, 30);
//    else // Use in SVO playback mode
//        zed = new Camera(argv[1]);


    InitParams parameters;
    parameters.mode = PERFORMANCE;
    parameters.unit = MILLIMETER;
    parameters.verbose = 1;

    ERRCODE err = zed->init(parameters);
    cout << errcode2str(err) << endl;
    if (err != SUCCESS) {
        delete zed;
        return ;
    }

    int width = zed->getImageSize().width;
    int height = zed->getImageSize().height;

    // allocate data
    buffer = new image_buffer();
    buffer->height = height;
    buffer->width = width;
    buffer->im_channels = 4;
    buffer->data_imr = new uchar[buffer->height * buffer->width* buffer->im_channels];
    buffer->data_im = new uchar[buffer->height * buffer->width * buffer->im_channels];

    cv::Mat left(height, width, CV_8UC4, buffer->data_im, buffer->width * buffer->im_channels * sizeof (uchar));
    cv::Mat right(height, width, CV_8UC4, buffer->data_imr, buffer->width * buffer->im_channels * sizeof (uchar));


    // Run thread
    std::thread grab_thread(grab_run);
    char key = ' ';

    std::cout << "Press 'q' to exit" << std::endl;

int n =0;
    // loop until 'q' is pressed
    while (key != 'q' && !stop_signal) {

        if (newFrame) {
            newFrame = false; //indicates that we take care of this frame... next frame will be told by the grabbin thread.

            // Retrieve data from buffer


            buffer->mutex_input_image.lock();
            memcpy(left.data, buffer->data_im, buffer->width * buffer->height * buffer->im_channels * sizeof (uchar));
        //memcpy(right.data, buffer->data_imr, buffer->width * buffer->height * buffer->im_channels * sizeof (uchar));
            buffer->mutex_input_image.unlock();

            buffer->mutex_input_imgr.lock();
            memcpy(right.data, buffer->data_imr, buffer->width * buffer->height * buffer->im_channels * sizeof (uchar));
            buffer->mutex_input_imgr.unlock();
        // Do stuff

            cv::imshow("Left", left);
            cv::imshow("Right", right);
                    char key = (char)cv::waitKey(5); //delay N millis, usually long enough to display and capture input
                    char filename[30];char filename1[30];
                    switch (key) {
                    case 'q':cout<<"Done acquizition"<<endl;
                    case 'Q':
                    case 27: //escape key
                        return ;
                    case ' ': //Save an image
                        double start = double(getTickCount());
                        sprintf(filename,"Left%.2d.png",n);
                        cv::imwrite(filename,left);
                        imagelist.push_back(filename);
                        cout << "Saved " << filename << endl;
                        sprintf(filename1,"Right%.2d.png",n++);
                        cv::imwrite(filename1,right);
                        cout << "Saved " << filename1 << endl;
                        imagelist.push_back(filename);
                        continue;
                    }

        } else
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    // Stop the grabbing thread
    stop_signal = true;
    grab_thread.join();

    delete[] buffer->data_imr;
    delete[] buffer->data_im;
    delete buffer;
    delete zed;
}


 void help(char** av)
{
  cout << "\nThis program calibrates a ZED binocular camera using a pattern with circle grids. Default is a 7*7 pattern with distance between centers at 8mm.\n"
      "usage:\n./" << av[0] << " resolution_number\n"
      << "1 for VGA\n"
      << "2 for HD720\n"<< "3 for HD1080\n"<<"When acquiring images, press space ' ' to save a pair of images.\n"<<"After having enough pairs, press 'q' to quit and it will calibrate automatically.\n" << endl;
}
