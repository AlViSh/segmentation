#ifndef KMEANS_H
#define KMEANS_H

#include <opencv2/opencv.hpp>
#include "cluster.h"
#include <ctime>


class KMeans
{
public:
    KMeans();

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
};

#endif // KMEANS_H
