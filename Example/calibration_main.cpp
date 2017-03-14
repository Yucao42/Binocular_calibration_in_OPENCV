#include <vector>
#include <string>
#include"functions.hpp"

int main(int ac, char** av){
    help(av);
    vector<string> imagelist;
    cv::Size boardSize(7,7);                   //7*7 grid size
    float squareSize=8;                        //distance between circles' centers (mm)
    bool displayCorners=false, useCalibrated=false,  showRectified=false;
    zed_capture( av,imagelist);
    StereoCalib(imagelist, boardSize, squareSize, displayCorners ,  useCalibrated,  showRectified);

    return 0;
}
