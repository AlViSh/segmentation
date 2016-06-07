#ifndef EGBS_H
#define EGBS_H


#include <iostream>
#include <opencv2/core/core.hpp>
#include "DisjointSetForest.h"


using namespace cv;
using namespace std;




class EGBS {
 public:
     EGBS();
     ~EGBS();

     int applySegmentation( Mat& image, float sigma, float threshold, int min_component_size );
     Mat recolor( bool random_color = false );
     int noOfConnectedComponents();

 protected:
     Mat image;
     Size imageSize;
     DisjointSetForest forest;
     inline float diff( Mat& rgb, int x1, int y1, int x2, int y2 );
};


#endif // EGBS_H
