#ifndef DISJOINTSETFOREST_H
#define DISJOINTSETFOREST_H


#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;




// Система непересекающихся множеств

 struct SetNode {
     int rank;
     int parent;
     int size;
 };


// Взвешенные ребра

 struct Edge {
     int a;
     int b;
     float weight;

     bool operator<( const Edge& other ) {
         return weight < other.weight;
     }
 };



 class DisjointSetForest{
 public:
     DisjointSetForest();
     DisjointSetForest( int no_of_elements );
     ~DisjointSetForest();

     void init( int no_of_elements );
     int find( int x );
     void join( int x, int y );
     int size( int x );
     int noOfElements();

     void segmentGraph( int no_of_vertices, int no_of_edges, vector<Edge>& edges, float c );

 private:
     vector<SetNode> elements;
     int num;
 };

#endif // DISJOINTSETFOREST_H
