#ifndef GMM_H
#define GMM_H
#define _USE_MATH_DEFINES

#include <opencv2/opencv.hpp>
#include "cluster.h"
#include <cmath>
#include <ctime>

class GMM
{
public:
    GMM();

    cv::Mat applySegmentation(cv::Mat &image, int k);
private:
    int NUM_CLUSTERS;
    cv::Mat image;
    cv::Size imageSize;
    std::vector<Cluster> clusters;
    void clearClusters();
    std::vector<cv::Vec3f> getCentroids();
    void assignCluster(cv::Mat& image);
    inline float diff(cv::Mat& rgb, int x1, int y1, cv::Vec3f ClusterColor);
    void calculateCentroids(cv::Mat& image);
    int size;
    long double *wight;
    long double **covariance;
    std::vector<cv::Vec3f> mean;
    long double GaussianDistribution(cv::Vec3f pix, int k);
};


#endif // GMM_H
