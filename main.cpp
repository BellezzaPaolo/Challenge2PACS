#include <iostream>
#include "Matrix.hpp"


int main(){
    constexpr algebra::StorageOrder S=algebra::StorageOrder::Row;
    constexpr algebra::Norm N=algebra::Norm::One;
    algebra::Matrix<double,S> A(4,6);
    std::vector<double> v={1,1,1,1,1,1};

    A(0,0)=10;
    A(0,1)=20;
    A(1,1)=30;
    A(2,2)=50;
    A(1,3)=40;
    A(2,3)=60;
    A(2,4)=70;
    A(3,5)=80;

    A.compress();
    /*std::cout<<A.norm<algebra::Norm::One>()<<std::endl;
    std::cout<<A.norm<N>()<<std::endl;
    std::cout<<A.norm<algebra::Norm::Frobenius>()<<std::endl;*/
    print(A);
    std::vector<double> r=A*v;
    std::cout<<"risultato: ";
    for(size_t i=0;i<r.size();++i){
        std::cout<<r[i]<<"  ";
    }
    std::cout<<std::endl;

    return 0;
}