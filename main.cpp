#include <iostream>
#include "Matrix.hpp"


int main(){
    constexpr algebra::StorageOrder S=algebra::StorageOrder::Row;
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

    A.print();
    A.compress();
    A.print();
    std::vector<double> r=A*v;
    std::cout<<"risultato  "<<std::endl;
    for(size_t i=0;i<r.size();++i){
        std::cout<<" "<<r[i];
    }
    std::cout<<std::endl;

    A.uncompress();
    A.print();
    A.resize(4,6);
    A.print();
    
    return 0;
}