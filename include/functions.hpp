#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <string.h>
#include <ctime>
#include <thread>
#include<opencv2/opencv.hpp>
#include <mutex>
using namespace std;


typedef struct image_bufferStruct {
    unsigned char* data_im;
    std::mutex mutex_input_image;

    unsigned char* data_imr;
    std::mutex mutex_input_imgr;

    int width, height, im_channels;
} image_buffer;


void grab_run();
void zed_capture(char** av,vector<string>& imagelist);

 void StereoCalib(vector<string>& imagelist, cv::Size boardSize, float squareSize, bool displayCorners , bool useCalibrated, bool showRectified);
 void help(char** av);
#endif // FUNCTIONS_HPP

