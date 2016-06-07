#include "kmeans.h"

KMeans::KMeans()
{

}

cv::Mat KMeans::applySegmentation(cv::Mat& image, int k)
{
    this->image = image.clone();
    this->imageSize = image.size();
    NUM_CLUSTERS=k;
    bool finish = false;
    int iter;

   // image.convertTo(this->image, CV_8UC3);
 //   int size = imageSize.height*imageSize.width;

    cv::Mat img2;
    image.convertTo( img2, CV_32FC1 );

    std::srand(std::time(NULL));

    for (int i=0; i<NUM_CLUSTERS; i++)
    {
        Cluster *cluster = new Cluster(i);
        cv::Vec3f centroid;
        centroid[0]=rand()%255;
        centroid[1]=rand()%255;
        centroid[2]=rand()%255;
        cluster->setCentroid(centroid);
        clusters.push_back(*cluster);
        delete cluster;
    }


    while (!finish) {
        int exit =0;
        clearClusters();
        std::vector<cv::Vec3f> lastCentroids = getCentroids();
        assignCluster(img2);
        calculateCentroids(img2);
        std::vector<cv::Vec3f> currentCentroids = getCentroids();
    //    int distance = 0;
        for(int i = 0; i < lastCentroids.size(); i++) {
            cv::Vec3f point1 = lastCentroids[i];
            cv::Vec3f point2 = currentCentroids[i];
            if (((int)point1[0] == (int)point2[0]) && ((int)point1[1]==(int)point2[1]) && ((int)point1[2]==(int)point2[2]))
            {
                exit++;
            }
        }
        iter++;

        if (exit==k || iter >100) finish = true;

    }


    cv::Mat result( imageSize, CV_8UC3, cv::Scalar(0, 0, 0) );

    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {
        Cluster cluster = *it;
        std::vector<cv::Point> points = cluster.getPoint();
        cv::Vec3d centroid = cluster.getCentroid();


        for (std::vector<cv::Point>::iterator it2 = points.begin(); it2 != points.end(); ++it2)
        {
            cv::Point point = *it2;
            cv::Vec3b centroidNew = centroid;

            result.at<cv::Vec3b>(point)[0]= centroidNew[0];
            result.at<cv::Vec3b>(point)[1]= centroidNew[1];
            result.at<cv::Vec3b>(point)[2]= centroidNew[2];

         //   this->image.at<cv::Vec3b>(point.getX(),point.getY())[0] = this->image.at<cv::Vec3b>(centroid.getX(),centroid.getY())[0];
         //   this->image.at<cv::Vec3b>(point.getX(),point.getY())[1] = this->image.at<cv::Vec3b>(centroid.getX(),centroid.getY())[0];
         //   this->image.at<cv::Vec3b>(point.getX(),point.getY())[2] = this->image.at<cv::Vec3b>(centroid.getX(),centroid.getY())[0];
        }
   }

    return result;

}

void KMeans::clearClusters()
{
    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {
        Cluster cluster = *it;
        int id = cluster.getId();
        clusters[id].clear();
    }

}

std::vector<cv::Vec3f> KMeans::getCentroids()
{
    std::vector<cv::Vec3f> centroids;
    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {
        Cluster cluster = *it;
        cv::Vec3f bgr = cluster.getCentroid();

        centroids.push_back(bgr);

    }
    return centroids;

}

void KMeans::assignCluster(cv::Mat& image)
{
    float max = 999.9;
    float min = max;
    int cluster = 0;
    float distance = 0.0;
    for( int y = 0; y < imageSize.height; y++ ) {
        for( int x = 0; x < imageSize.width; x++ ) {
            min=max;
            cluster = 0;
            for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
            {
                Cluster c = *it;
                cv::Vec3f ClusterColor = c.getCentroid();
                distance = diff(image, x, y, ClusterColor);
                if (distance < min)
                {
                    min=distance;
                    cluster=c.getId();
                }

            }
            cv::Point point;
            point.x=x;
            point.y=y;
            clusters[cluster].addPoint(point);

        }
    }
}

float KMeans::diff(cv::Mat& rgb, int x1, int y1, cv::Vec3f ClusterColor ) {
    cv::Vec3f pix1 = rgb.at<cv::Vec3f>(y1, x1);
    cv::Vec3f pix2 = ClusterColor;
    return sqrt( (pix1 - pix2).dot((pix1 - pix2)) );
}

void KMeans::calculateCentroids(cv::Mat& image)
{
    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {
        float sumB=0;
        float sumG=0;
        float sumR=0;
        Cluster cluster = *it;
        int id = cluster.getId();
        std::vector<cv::Point> points = cluster.getPoint();
        int size = 0;
        for (std::vector<cv::Point>::iterator it2 = points.begin(); it2 != points.end(); ++it2)
        {
            cv::Point point = *it2;
            sumB += image.at<cv::Vec3f>(point)[0];
            sumG += image.at<cv::Vec3f>(point)[1];
            sumR += image.at<cv::Vec3f>(point)[2];
            size++;
        }
        cv::Vec3f centroid = cluster.getCentroid();
        if (size > 0)
        {
            float newB = sumB/size;
            float newG = sumG/size;
            float newR = sumR/size;
            centroid[0]=newB;
            centroid[1]=newG;
            centroid[2]=newR;
            clusters[id].setCentroid(centroid);
        }

    }
}
