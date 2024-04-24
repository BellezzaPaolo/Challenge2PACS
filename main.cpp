#include <iostream>
#include "Matrix.hpp"


int main(){
    constexpr algebra::StorageOrder S=algebra::StorageOrder::Row;
    algebra::Matrix<double,S> A(4,6);

    A(0,0)=10;
    A(10,1)=20;
    A(1,1)=30;
    A(2,2)=50;
    A(1,3)=40;
    A(2,3)=60;
    A(2,4)=70;
    A(3,5)=80;

    A.print();
    A.compress();
    A.print();
    //A.uncompress();
    //A.print();
    
    return 0;
}