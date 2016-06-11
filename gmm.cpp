#include "gmm.h"

GMM::GMM()
{

}


cv::Mat GMM::applySegmentation(cv::Mat& image, int k)
{
    this->image = image.clone();
    this->imageSize = image.size();
    NUM_CLUSTERS=k;
    bool finish = false;
//    int iter;

// //   image.convertTo(this->image, CV_8UC3);
//   // int size = imageSize.height*imageSize.width;

//    cv::Mat img2;
//    image.convertTo( img2, CV_32FC1 );

//  //  std::srand(std::time(NULL));

//    for (int i=0; i<NUM_CLUSTERS; i++)
//    {
//        Cluster *cluster = new Cluster(i);
//        cv::Vec3f centroid;
//        centroid[0]=rand()%255;
//        centroid[1]=rand()%255;
//        centroid[2]=rand()%255;
//        cluster->setCentroid(centroid);
//        clusters.push_back(*cluster);
//        delete cluster;

//    }


    float Ra, Rb;
    Ra=1.2;
    Rb=1.5;
    std::vector<float> pt;
    float mountin=0;

    cv::Mat img2;
    image.convertTo( img2, CV_32FC1 );

    std::srand(std::time(NULL));
    int RED=255;
    int GREEN=255;
    int BLOO=255;

    for(int b =0; b<RED; b+=50) {
        for(int g=0; g<GREEN; g+=50) {
            for(int r=0; r<BLOO; r+=50) {
                mountin=0;
                for( int z = 0; z < imageSize.height; z++ ) {
                    for( int c = 0; c < imageSize.width; c++ ) {
                        cv::Vec3f pix2 = img2.at<cv::Vec3f>(z, c);
                        float zzz = -(sqrt(pow(pix2[0]-b,2)+pow(pix2[1]-g,2)+pow(pix2[2]-r,2)))/(Ra*Ra);
                        mountin=mountin+exp(-(sqrt(pow(pix2[0]-b,2)+pow(pix2[1]-g,2)+pow(pix2[2]-r,2)))/(Ra*Ra));

                    }
                }
                pt.push_back(mountin);
            }
        }
    }

    for (int a=0; a<NUM_CLUSTERS; a++)
    {
        float max=0;
        int index1=0;
        int index2=0;
        for (std::vector<float>::iterator it = pt.begin(); it!=pt.end(); ++it) {
            if (*it>max)
            {
                max=*it;
                index1=index2;
            }
            index2++;

        }



        cv::Vec3f bufpix;
        Cluster *cluster = new Cluster(a);
        index2=0;

        for(int b =0; b<RED; b+=50) {
            for(int g=0; g<GREEN; g+=50) {
                for(int r=0; r<BLOO; r+=50) {
                    if (index2==index1)
                    {
                        cv::Vec3f centroid;
                        centroid[0]=b;
                        centroid[1]=g;
                        centroid[2]=r;

                        cluster->setCentroid(centroid);
                        clusters.push_back(*cluster);

                    }
                    index2++;
                }
            }
        }


        index2=0;
        //pt.clear();
        for(int b =0; b<RED; b+=50) {
            for(int g=0; g<GREEN; g+=50) {
                for(int r=0; r<BLOO; r+=50) {
                    float buf;
                    buf = max*exp(- (sqrt(pow(clusters.back().getCentroid()[0]-b,2)+pow(clusters.back().getCentroid()[1]-g,2)+pow(clusters.back().getCentroid()[2]-r,2)))/(Rb*Rb));
                    pt[index2]=pt[index2]-buf;

                    index2++;


                }
            }
        }
    }

    int iter=0;




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

        if (exit==k || iter >10) finish = true;

    }



    //вычислим коэффициенты смешивания (веса) и дисперсию

    size =imageSize.height*imageSize.width;
    wight = new long double [NUM_CLUSTERS];
    int cluster_i=0;
    covariance = new long double *[NUM_CLUSTERS];

    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {

        Cluster cluster = *it;
        std::vector<cv::Point> points = cluster.getPoint();
        cv::Vec3d centroid = cluster.getCentroid();

        wight[cluster_i]= points.size() /(double)this->size;
     //   wight[cluster_i]=1/(long double)NUM_CLUSTERS;
        covariance[cluster_i] = new long double[3];
        double B=0;
        double G=0;
        double R=0;
        for (std::vector<cv::Point>::iterator it2 = points.begin(); it2 != points.end(); ++it2)
        {
            cv::Point point = *it2;

            B += pow(img2.at<cv::Vec3f>(point)[0] - centroid[0],2);
            G += pow(img2.at<cv::Vec3f>(point)[1] - centroid[1],2);
            R += pow(img2.at<cv::Vec3f>(point)[2] - centroid[2],2);

        }
        covariance[cluster_i][0] = B/ (long double)this->size;
        covariance[cluster_i][1] = G/ (long double)this->size;
        covariance[cluster_i][2] = R/ (long double)this->size;
//        covariance[cluster_i][0]=1.0;
//        covariance[cluster_i][1]=1.0;
//        covariance[cluster_i][2]=1.0;


        cluster_i++;


   }



    mean = getCentroids();


    //EM alg

    iter=0;
    long double **P = new long double *[size];
    for (int i=0; i<size; i++)
    P[i] = new long double [NUM_CLUSTERS];

    finish = false;

    long double delta;


    //

    while (!finish) {

        int count=0;
        long double likelihoodOld=0;
        long double likelihoodNew=0;

        //E
        for( int y = 0; y < imageSize.height; y++ ) {
            for( int x = 0; x < imageSize.width; x++ ) {
                for (int j=0; j<NUM_CLUSTERS; j++)
                {
                    cv::Vec3f pix = img2.at<cv::Vec3f>(y, x);
                    long double numerator = wight[j]*GaussianDistribution(pix,j);
                    long double denominator=0;
                    for (int i=0; i<NUM_CLUSTERS; i++)
                    {
                        denominator+=wight[i]*GaussianDistribution(pix,i);
                    }

                    P[count][j] = numerator/denominator;


                    likelihoodOld+=log10(denominator);



                }
                count++;
            }
        }

        //M

        for(int i = 0; i < mean.size(); i++) {
            mean[i][0]=0;
            mean[i][1]=0;
            mean[i][2]=0;
        }

        long double *Nk = new long double[NUM_CLUSTERS];
        for (int j=0; j<NUM_CLUSTERS; j++)
        {
            Nk[j]=0;
        }
        count =0;

        for (int j=0; j<NUM_CLUSTERS; j++)
        {
            for( int y = 0; y < imageSize.height; y++ ) {
                for( int x = 0; x < imageSize.width; x++ ) {
                    Nk[j]+=P[count][j];
                    count++;
                }
            }
            count=0;

        }


        long double *responsibilitie= new long double[3];

        // центры кластеров
        for (int j=0; j<NUM_CLUSTERS; j++)
        {

            for (int i=0; i<3; i++)
            {
                responsibilitie[i]=0;
            }
            int count2=0;
            for( int y = 0; y < imageSize.height; y++ ) {
                for( int x = 0; x < imageSize.width; x++ ) {

                    cv::Vec3f pix = img2.at<cv::Vec3f>(y, x);
                    responsibilitie[0]+= pix[0]*P[count2][j];
                    responsibilitie[1]+= pix[1]*P[count2][j];
                    responsibilitie[2]+= pix[2]*P[count2][j];
                    count2++;
                }
            }
            mean[j][0]=responsibilitie[0]/Nk[j];
            mean[j][1]=responsibilitie[1]/Nk[j];
            mean[j][2]=responsibilitie[2]/Nk[j];

        }





        //матрица ковариаций
        for (int j=0; j<NUM_CLUSTERS; j++)
        {

            for (int i=0; i<3; i++)
            {
                responsibilitie[i]=0;
            }
            int count3=0;
            for( int y = 0; y < imageSize.height; y++ ) {
                for( int x = 0; x < imageSize.width; x++ ) {
                    cv::Vec3f pix = img2.at<cv::Vec3f>(y, x);
                    long double buf1 = (pix[0]-mean[j][0])*(pix[0]-mean[j][0]);
                    long double buf2 = (pix[1]-mean[j][1])*(pix[1]-mean[j][1]);
                    long double buf3 = (pix[2]-mean[j][2])*(pix[2]-mean[j][2]);

                    responsibilitie[0]+= buf1*P[count3][j];
                    responsibilitie[1]+= buf2*P[count3][j];
                    responsibilitie[2]+= buf3*P[count3][j];
                    count3++;
                }
            }
            covariance[j][0]=responsibilitie[0]/Nk[j];
            covariance[j][1]=responsibilitie[1]/Nk[j];
            covariance[j][2]=responsibilitie[2]/Nk[j];

        }

        for (int j=0; j<NUM_CLUSTERS; j++) {
            for (int z=0; z<3; z++)
            {
                if (covariance[j][z]==0) {
                    covariance[j][z]=rand()%255;
                    mean[j][z]=rand()%255;
                }
            }
        }




        delete [] responsibilitie;

        for (int j=0; j<NUM_CLUSTERS; j++)
        {
            wight[j]=Nk[j]/(double)this->size;
        }

        delete [] Nk;


        for( int y = 0; y < imageSize.height; y++ ) {
            for( int x = 0; x < imageSize.width; x++ ) {
                for (int j=0; j<NUM_CLUSTERS; j++)
                {
                    cv::Vec3f pix = img2.at<cv::Vec3f>(y, x);

                    long double denominator=0;
                    for (int i=0; i<NUM_CLUSTERS; i++)
                    {
                        denominator+=wight[i]*GaussianDistribution(pix,i);
                    }


                    likelihoodNew+=log10(denominator);



                }

            }
        }



        delta = (likelihoodNew-likelihoodOld)/(long double)std::abs(likelihoodOld);

        iter++;

        if (iter >100 || delta < 5e-4) finish = true;



    }

    clearClusters();
    int count=0;


    for( int y = 0; y < imageSize.height; y++ ) {
        for( int x = 0; x < imageSize.width; x++ ) {
            double distance = 0.0;
            int cluster = 0;
            for (int j=0; j<NUM_CLUSTERS; j++)
            {
                distance = P[count][j];
                if (distance>0.5)
                {
                    cv::Point point;
                    point.x=x;
                    point.y=y;
                    clusters[j].addPoint(point);

                }

            }
            count++;
        }
    }


    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {
        delete covariance[cluster_i];

    }
    delete [] covariance;

    delete [] wight;

    for (int i=0; i<size; i++)
    delete []P[i];
    delete []P;


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
     clusters.clear();
     std::vector<Cluster>().swap(clusters);
     mean.clear();
     std::vector<cv::Vec3f>().swap(mean);

     medianBlur(result, result, 7);

    return result;

}

void GMM::clearClusters()
{
    for (std::vector<Cluster>::iterator it = clusters.begin() ; it != clusters.end(); ++it)
    {
        Cluster cluster = *it;
        int id = cluster.getId();
        clusters[id].clear();
    }

}

std::vector<cv::Vec3f> GMM::getCentroids()
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

void GMM::assignCluster(cv::Mat& image)
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

float GMM::diff(cv::Mat& rgb, int x1, int y1, cv::Vec3f ClusterColor ) {
    cv::Vec3f pix1 = rgb.at<cv::Vec3f>(y1, x1);
    cv::Vec3f pix2 = ClusterColor;
    return sqrt( (pix1 - pix2).dot((pix1 - pix2)) );
}

void GMM::calculateCentroids(cv::Mat& image)
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

long double GMM::GaussianDistribution(cv::Vec3f pix, int k)
{
    long double N, NN;
  //  double power=1/2;
    cv::Vec3f difference; // разница векторов x и mean
    difference[0]= pix[0]-mean[k][0];
    difference[1]= pix[1]-mean[k][1];
    difference[2]= pix[2]-mean[k][2];


    N = exp(-(pow(difference[0],2))/(2*covariance[k][0]))/(sqrt(2*M_PI)*(sqrt(covariance[k][0])))*exp(-(pow(difference[1],2))/(2*covariance[k][1]))/(sqrt(2*M_PI)*(sqrt(covariance[k][1])))*exp(-(pow(difference[2],2))/(2*covariance[k][2]))/(sqrt(2*M_PI)*(sqrt(covariance[k][2])));




    NN = exp(-(pow(difference[0],2))/(2*covariance[k][0])-(pow(difference[1],2))/(2*covariance[k][1])-(pow(difference[2],2))/(2*covariance[k][2]))/(2*M_PI*sqrt(covariance[k][0])*sqrt(covariance[k][1])*sqrt(covariance[k][2]));

    return NN;
}
