#include "DisjointSetForest.h"
#include <algorithm>



DisjointSetForest::DisjointSetForest() {
}


DisjointSetForest::DisjointSetForest( int no_of_elements ) {
    init( no_of_elements );
}


// инициализация
void DisjointSetForest::init( int no_of_elements ) {
    this->elements.clear();
    this->elements.resize( no_of_elements );
    this->num = no_of_elements;

    for( int i = 0; i < no_of_elements; i++ ) {
        elements[i].rank    = 0;
        elements[i].size    = 1;
        elements[i].parent  = i;
    }
}


DisjointSetForest::~DisjointSetForest() {
}


// найти элемент
int DisjointSetForest::find( int x ) {
    int y = x;
    while( y != elements[y].parent )
        y = elements[y].parent;
    elements[x].parent = y;
    return y;
}


// объединить наборы
 void DisjointSetForest::join( int x, int y ) {
     if ( elements[x].rank > elements[y].rank ) {
         elements[y].parent  = x;
         elements[x].size    += elements[y].size;
     }
     else {
         elements[x].parent  = y;
         elements[y].size    += elements[x].size;

         if( elements[x].rank == elements[y].rank )
             elements[y].rank++;
     }
     num--;
 }


// размер набора
 int DisjointSetForest::size( int x ) {
     return elements[x].size;
 }


// общее колличество наборов внутри леса
 int DisjointSetForest::noOfElements() {
     return num;
 }


// сегментирование графа с учетом весов
 void DisjointSetForest::segmentGraph( int no_of_vertices, int no_of_edges, vector<Edge>& edges, float c ) {
     init( no_of_vertices );

     sort( edges.begin(), edges.end(), []( Edge& a, Edge& b){
         return a.weight < b.weight;
     });

     vector<float> thresholds( no_of_vertices, c );


     for( Edge& edge: edges ){
         int a = this->find( edge.a );
         int b = this->find( edge.b );

         if( a != b ) {


             if( edge.weight <= thresholds[a] && edge.weight <= thresholds[b] ) {
                 this->join( a, b );
                 a = this->find( a );
                 thresholds[a] = edge.weight + c / this->size( a );
             }
         }
     }
 }
