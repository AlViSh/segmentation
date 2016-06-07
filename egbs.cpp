#include "EGBS.h"
#include <map>


 EGBS::EGBS() {

 }


 EGBS::~EGBS() {

 }


 /**
  * Вычисляем разность между 2 пикселями (3 канала)
  * взяв евклидову норму этой разности.
  */
 float EGBS::diff( Mat& rgb, int x1, int y1, int x2, int y2 ) {
     Vec3f pix1 = rgb.at<Vec3f>(y1, x1);
     Vec3f pix2 = rgb.at<Vec3f>(y2, x2);
     return sqrt( (pix1 - pix2).dot((pix1 - pix2)) );
 }


 /**
  * Выполнение сегментации
  */
 int EGBS::applySegmentation( Mat& image, float sigma, float threshold, int min_component_size ) {
     this->image = image.clone();
     this->imageSize = image.size();

     /* Размытие по Гаусу */
     Mat smoothed;
     image.convertTo( smoothed, CV_32FC1 );
     GaussianBlur( smoothed, smoothed, Size(5,5), sigma );

     vector<Edge> edges( imageSize.area() * 4 );
     int no_of_edges = 0;

     /* Создание ребер между каждыми пикселями , с весом */
     for( int y = 0; y < imageSize.height; y++ ) {
         for( int x = 0; x < imageSize.width; x++ ) {
             if( x < imageSize.width - 1 ){
                 edges[no_of_edges].a        = y * imageSize.width + x;
                 edges[no_of_edges].b        = y * imageSize.width + (x + 1);
                 edges[no_of_edges].weight   = diff( smoothed, x, y, x + 1, y );
                 no_of_edges++;
             }


             if( y < imageSize.height - 1 ) {
                 edges[no_of_edges].a        = y       * imageSize.width + x;
                 edges[no_of_edges].b        = (y + 1) * imageSize.width + x;
                 edges[no_of_edges].weight   = diff( smoothed, x, y, x, y + 1 );
                 no_of_edges++;
             }


             if( (x < imageSize.width - 1) && (y < imageSize.height - 1) ) {
                 edges[no_of_edges].a        = y       * imageSize.width + x;
                 edges[no_of_edges].b        = (y + 1) * imageSize.width + (x + 1);
                 edges[no_of_edges].weight   = diff( smoothed, x, y, x + 1, y + 1 );
                 no_of_edges++;
             }


             if( (x < imageSize.width - 1) && (y > 0) ) {
                 edges[no_of_edges].a        = y       * imageSize.width + x;
                 edges[no_of_edges].b        = (y - 1) * imageSize.width + (x + 1);
                 edges[no_of_edges].weight   = diff( smoothed, x, y, x + 1, y - 1 );
                 no_of_edges++;
             }
         }
     }


     edges.resize( no_of_edges );

     /* выполнить сегментацию */
     forest.segmentGraph( imageSize.height * imageSize.width, no_of_edges, edges, threshold );


     /* Объединяем все маленькие элементы */
     for( Edge& edge: edges ) {
         int a = forest.find( edge.a );
         int b = forest.find( edge.b );
         if( (a != b) && (( forest.size(a) < min_component_size) || (forest.size(b) < min_component_size)) ) {
             forest.join( a, b );
         }
     }

     return forest.noOfElements();
 }




 int EGBS::noOfConnectedComponents() {
     return forest.noOfElements();
 }


 /**
  * изменение цвета
  */
 Mat EGBS::recolor( bool random_color) {
     Mat result( imageSize, CV_8UC3, Scalar(0, 0, 0) );
     map<int, Vec3f> colors;

     if( !random_color ){
         map<int, int> count;

         /* если цвет не случайный то цвет основан на среднем значении каждого кластера */
         for(int y = 0; y < imageSize.height; y++ ) {
             Vec3b * ptr = image.ptr<Vec3b>(y);

             for(int x = 0; x < imageSize.width; x++ ) {
                 int component = forest.find( y * imageSize.width + x );
                 colors[component] += ptr[x];
                 count[component]++;
             }
         }

         /* средний цыет */
         for( auto itr : colors )
             colors[itr.first] /= count[itr.first];
     }
     else {
         /* случайный цвет */
         for(int y = 0; y < imageSize.height; y++ ) {
             for(int x = 0; x < imageSize.width; x++ ) {
                 int component = forest.find( y * imageSize.width + x );
                 if( colors.count( component ) == 0 )
                     colors[component] = Vec3f( rand() % 255, rand() % 255, rand() % 255 );
             }
         }
     }

     /* изменение цвета */
     for(int y = 0; y < imageSize.height; y++ ) {
         Vec3b * ptr = result.ptr<Vec3b>(y);

         for(int x = 0; x < imageSize.width; x++ ) {
             int component = forest.find( y * imageSize.width + x );
             Vec3f color = colors[component];
             ptr[x] = Vec3b( color[0], color[1], color[2] );
         }
     }




     return result;
 }
