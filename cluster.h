#ifndef CLUSTER_H
#define CLUSTER_H

#include <opencv2/opencv.hpp>

class Cluster
{
public:
    Cluster(int id)
    {
        this->id=id;
    }

    Cluster();
    ~Cluster()
    {

    }


    std::vector<cv::Point> getPoint()
    {
        return points;
    }
    void addPoint(cv::Point point)
    {
        points.push_back(point);
    }
    void setPoint(std::vector<cv::Point> points)
    {
        this->points=points;
    }
    cv::Vec3f getCentroid()
    {
        return this->centroid;
    }
    void setCentroid(cv::Vec3f centroid)
    {
        this->centroid=centroid;
    }
    int getId()
    {
        return this->id;
    }
    void clear()
    {
        points.clear();
    }

private:
    std::vector<cv::Point> points;
    cv::Vec3f centroid;
    int id;

};

#endif // CLUSTER_H
