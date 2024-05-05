#include <iostream>
#include <chrono>
#include <complex>
#include "Matrix.hpp"


int main(){
    constexpr algebra::StorageOrder S=algebra::StorageOrder::Row;
    constexpr algebra::Norm N=algebra::Norm::Infinity;
    algebra::Matrix<double,S> A(4,6),B(6,4);
    std::vector<double> v(131,1.0);
    using time=std::chrono::time_point<std::chrono::steady_clock>;

    A(0,0)=10.0;
    A(0,1)=20.0;
    A(1,1)=30.0;
    A(2,2)=50;
    A(1,3)=40;
    A(2,3)=60;
    A(2,4)=70;
    A(3,5)=80;
    //read("lnsp_131.mtx",A);
    
    //print(A);
    /*const time start0= std::chrono::steady_clock::now();
    std::cout<<"norma 1          "<<A.norm<algebra::Norm::One>()<<std::endl;
    std::cout<<"norma infinito   "<<A.norm<algebra::Norm::Infinity>()<<std::endl;
    std::cout<<"norma frobenius  "<<A.norm<algebra::Norm::Frobenius>()<<std::endl;
    const time end0=std::chrono::steady_clock::now();
    std::cout<<"\nComputation norms took: "<< std::chrono::duration_cast<std::chrono::microseconds>(end0-start0).count() <<std::endl;*/

    //print(A);
    /*const time start1= std::chrono::steady_clock::now();
    std::vector<double> r=A*v;
    const time end1=std::chrono::steady_clock::now();

    std::cout<<"\nMatrix*vector took: "<< std::chrono::duration_cast<std::chrono::microseconds>(end1-start1).count() <<std::endl;
*/

    /*const time start3= std::chrono::steady_clock::now();
    std::cout<<"norma 1          "<<A.norm<algebra::Norm::One>()<<std::endl;
    std::cout<<"norma infinito   "<<A.norm<N>()<<std::endl;
    std::cout<<"norma frobenius  "<<A.norm<algebra::Norm::Frobenius>()<<std::endl;
    const time end3=std::chrono::steady_clock::now();
    std::cout<<"\ncomputation norms compressed took: "<< std::chrono::duration_cast<std::chrono::microseconds>(end3-start3).count() <<std::endl;
*/
    /*const time start2= std::chrono::steady_clock::now();
    std::vector<double> R=A*v;
    const time end2=std::chrono::steady_clock::now();
    std::cout<<"\nMatrix*vector compressed took: "<< std::chrono::duration_cast<std::chrono::microseconds>(end2-start2).count() <<std::endl;*/
    print(A);
    //algebra::Matrix<double,algebra::StorageOrder::Row> r=A*B;
    //print(r);

    return 0;
}