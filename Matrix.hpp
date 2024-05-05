#ifndef MATRIX_HPP
#define MATRIX_HPP

#include<map>
#include<vector>
#include<cmath>
#include<string>
#include<iostream>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace algebra{ //definition of namespace

    enum class StorageOrder {Column,Row};  //definition of the 2 ways it can be stored: column-wise or row-wise respectively
    enum class Norm {One, Infinity, Frobenius}; //definition of the 3 possible norms that can compute: norm One, norm Infinity or Frobenius norm respectively

    template<StorageOrder S>
    struct comp{   //struct that implement the comparator for the storage ordering of the map
        bool operator () (const std::array<size_t,2> a,const std::array<size_t,2> b) const{
            if constexpr (S==StorageOrder::Column){ 
                if(a[1]==b[1]){
                    return a[0]<b[0];
                }
                return a[1]<b[1];
            }
            else if constexpr (S==StorageOrder::Row){
                if(a[0]==b[0]){
                    return a[1]<b[1];
                }
                return a[0]<b[0];
            }
        }
    };


    template<typename T,enum StorageOrder S>
    using mappa=std::map<std::array<std::size_t,2>, T,comp<S>>;  //definiton of the namespace used to indicate the map used in the implementation



    /*BEGINNING OF THE DEFINITION OF THE CLASS Matrix THAT STORES ELEMENT OF TYPE T (double, int, float, std::complex<double>,...) IN S (row-wise or column-wise) ORDERING*/


    template<typename T, enum StorageOrder S>  
    class Matrix{
        private:
            mappa<T,S> elem;  //the map used to stor the matrix in the dynamic form

            std::vector<std::size_t> inner;  //the 3 vector used to store the matrix in the compressed form
            std::vector<std::size_t> outer;
            std::vector<T> val;

            std::size_t row;  //stores the number of rows of the matrix
            std::size_t col;  //stores the number of columns of the matrix



        public:

            friend std::vector<T> operator * <> (const Matrix<T,S>& A,const std::vector<T>& b); //friend operator * that implements the matrix*vector product

            /*template<StorageOrder V>
            friend Matrix<T,StorageOrder::Row> operator * (const Matrix<T,S>& A,const Matrix<T,V>& B);  //friend operator * that implements the matrix*matrix product*/
            
            friend void print <>(const Matrix<T,S>& A);  //friend function that prints the matrix in the usual form

            friend void read <>(const std::string& filename,Matrix<T,S>& A);  //friend function for reading the matrix from a file

            Matrix(std::size_t R,std::size_t C){  //constructor that initialize every private member as empty, the column as the input C and the row as the input R
                mappa<T,S> e;
                std::vector<std::size_t> I,O;
                std::vector<T> V;
                inner=I;outer=O;elem=e;row=R;col=C;
            };

            Matrix(const std::string& filename){  //constructor that initialize the matrix as explained in the file in input
                mappa<T,S> e;
                std::vector<std::size_t> I,O;
                std::vector<T> V;
                inner=I;outer=O;elem=e;
                read(filename,*this);
            };
                
            T& operator () (std::size_t i,std::size_t j){ //call operator that acceds at the element in the i-th row, j-th column and returns a pointer to that element 
                if(i>=row ||j>=col){ //in case the indexes aren't valid the program stops
                    std::cerr<<"\n\nIndex exceed bounds of the matrix\n\n"<<std::endl;
                    exit(0);
                }           
                if(is_compressed()){  //acceds when the matrix is in the compressed form
                    if constexpr (S==StorageOrder::Row){
                        for(size_t k=inner[i];k<inner[i+1];++k){//exctract he element
                            if(outer[k]==j){
                                return val[k];
                            }
                        }
                        //when the matrix is in the compressed form any new element can be add but only change alreaddy present
                        //in that case the program stops
                        std::wcerr<<"\n\nCannot insert a new element in compressed form\n\n"<<std::endl;
                        exit(0);                        
                    }
                    else if constexpr (S==StorageOrder::Column){
                        for(size_t k=inner[j];k<inner[j+1];++k){  //extract he element
                            if(outer[k]==i){
                                return val[k];
                            }
                        }
                        //when the matrix is in the compressed form any new element can be add but only change alreaddy present
                        //in that case the program stops
                        std::wcerr<<"\n\nCannot insert a new element in compressed form\n\n"<<std::endl;
                        exit(0);
                    }
                }
                //if the computation reaches this point the matrix is uncompressed and acceds at the right element of the map,
                //in case of non existence a new one is added
                std::array<std::size_t,2> key={i,j};
                return elem[key];
            }

            T operator () (std::size_t i,std::size_t j) const{ //const call operator that returns the element in position in the i-th row, j-th column
                if(i>row || j>col){ //if any index exceds the bound the program stops
                    std::cerr<<"\n\nIndex excedds matrix bounds\n\n"<<std::endl;
                    exit(0);
                }
                if(is_compressed()){
                    if constexpr (S==StorageOrder::Row){
                        if(inner[i]!=inner[inner.size()-1]){//search of the element
                            for(size_t k=inner[i];k<inner[i+1];++k){
                                if(outer[k]==j){
                                    return val[k];
                                }
                            }
                        }
                        //if the computation reaches this point the element is not memorized so is null
                        return 0;
                    }
                    else{
                        if(inner[j]!=inner[inner.size()-1]){
                            for(size_t k=inner[j];k<inner[j+1];++k){//search of the element
                                if(outer[k]==i){
                                    return val[k];
                                }
                            }
                        }
                        //if the computation reaches this point the element is not memorized so is null
                        return 0;
                    }
                }
                else{
                    std::array<std::size_t,2> key={i,j};
                    auto it=elem.find(key); //search of the element
                    if(it==elem.end()){//if not found it means that is null
                        return 0;
                    }
                    return it->second;                
                }
            }
            
            void compress(){ //this method compress the matrix (from uncompressed form to compressed form)
                if(is_compressed()){
                    //prints an error and then stops in case it's just compressed
                    std::cout<<"\n\n it's already compressed"<<std::endl;
                    exit(0);
                }
                //inizialise every vector properly based on the storage ordering and then cleans the map
                if constexpr(S==StorageOrder::Row){
                    inner.resize(row+1,elem.size());
                    for(auto it:elem){
                        val.push_back(it.second);
                        outer.push_back(it.first[1]);
                        if(inner[it.first[0]]>val.size()-1){
                            inner[it.first[0]]=val.size()-1;
                        }
                    }
                    elem.clear();
                }
                else if constexpr(S==StorageOrder::Column){
                    inner.resize(col+1,elem.size());
                    for(auto it:elem){
                        val.push_back(it.second);
                        outer.push_back(it.first[0]);
                        if(inner[it.first[1]]>val.size()-1){
                            inner[it.first[1]]=val.size()-1;
                        }
                    }
                    elem.clear();

                }
            }

            void uncompress(){  //this method uncompress the matrix (from compressed form to the uncompressed one)
                if(!is_compressed()){
                    //if already uncompressed prints an error and stops
                    std::cout<<"\n\n it's already uncompressed"<<std::endl;
                    exit(0);
                }
                //initialise the map properly and then clear all vectors
                if constexpr (S==StorageOrder::Row){
                    for(size_t i=0;i<inner.size()-1;++i){
                        if(inner[i]!=inner[inner.size()-1]){
                            std::array<size_t,2> key;
                            key[0]=i;
                            for(size_t k=inner[i];k<inner[i+1];++k){
                                key[1]=outer[k];
                                elem.insert(std::make_pair(key,val[k]));
                            }
                        }
                    }
                    inner.clear();
                    outer.clear();
                    val.clear();
                }
                else if constexpr (S==StorageOrder::Column){
                    for(size_t i=0;i<inner.size()-1;++i){
                        if(inner[i]!=inner[inner.size()-1]){
                            std::array<size_t,2> key;
                            key[1]=i;
                            for(size_t k=inner[i];k<inner[i+1];++k){
                                key[0]=outer[k];
                                elem.insert(std::make_pair(key,val[k]));
                            }
                        }
                    }
                    inner.clear();
                    outer.clear();
                    val.clear();
                }
            }

            bool is_compressed() const{  //check if the matrix is in the compressed form (returns TRUE) or not (returns FALSE)
                return !(val.empty() && inner.empty() && outer.empty());
            }

            void resize(std::size_t R,std::size_t C){ //this method makes the resize of the matrix becoming a matrix with R rows and C columns
                if(is_compressed()){
                    //if the matrix is compressed it's impossible to resize so prints and error and stops
                    std::cout<<"\n\nto resize the matrix needs to be in an uncompressed state\n\n"<<std::endl;
                    exit(0);
                }
                //deletes elements if the new matrix has less rows
                if(R<row){
                    if constexpr(S==StorageOrder::Row){
                        auto it=elem.lower_bound({R,0});
                        elem.erase(it,elem.end());
                    }
                    else{
                        for(size_t i=R;i<row;++i){
                            for(size_t j=0;j<col;++j){
                                elem.erase({i,j});
                            }
                        }
                    }
                }
                //deletes elements if the new matrix has less columns
                if(C<col){
                    if constexpr (S==StorageOrder::Column){
                        auto it=elem.lower_bound({0,C});
                        elem.erase(it,elem.end());
                    }
                    else{
                        for(size_t j=C;j<col;++j){
                            for(size_t i=0;i<row;++i){
                                elem.erase({i,j});
                            }
                        }
                    }
                }
                //updates the dimension properly
                row=R;
                col=C;
                return;
            }

            template <enum Norm N>
            double norm () const{ //this method coputes the norm of type N of the matrix and returns the result
                double res=0;
                if constexpr (N==Norm::Infinity){  
                    //computation of the Infinity norm in the proper way depending on the way it's stored
                    if constexpr (S==StorageOrder::Row){
                        if(is_compressed()){
                            for(size_t i=0;i<inner.size()-1;++i){
                                double sumRowI=0;
                                for(size_t j=inner[i];j<inner[i+1];++j){
                                    sumRowI+=abs(val[j]);
                                }
                                if(sumRowI>res){
                                    res=sumRowI;
                                }
                            }
                        }
                        else{
                            for(size_t i=0;i<row;++i){
                                double sumRowI=0;
                                for(auto it=elem.lower_bound({i,0});it!=elem.upper_bound({i,col});++it){
                                    sumRowI+=abs(it->second);
                                }
                                if(sumRowI>res){
                                    res=sumRowI;
                                }
                            }
                        }
                    }
                    else{
                        if(is_compressed()){
                            for(size_t i=0;i<row;++i){
                                double sumRowI=0;
                                for(size_t k=0;k<val.size();++k){
                                    if(outer[k]==i){
                                        sumRowI+=abs(val[k]);
                                    }
                                }
                                if(sumRowI>res){
                                    res=sumRowI;
                                }
                            }

                        }
                        else{
                            for(size_t i=0;i<row;++i){
                                double sumRowI=0;
                                for(size_t j=0;j<col;++j){
                                    if(elem.contains({i,j})){
                                    sumRowI+=abs(elem.at({i,j}));
                                    }
                                }
                                if(sumRowI>res){
                                    res=sumRowI;
                                }
                            }
                        }
                    }
                }
                else if constexpr (N==Norm::One){
                    //computation of the One norm in the proper way depending on the way it's stored
                    if constexpr (S==StorageOrder::Row){
                        if(is_compressed()){
                            for(size_t j=0;j<col;++j){
                                double sumColJ=0;
                                for(size_t k=0;k<outer.size();++k){
                                    if(outer[k]==j){
                                        sumColJ+=abs(val[k]);
                                    }
                                }
                                if(sumColJ>res){
                                    res=sumColJ;
                                }
                            }
                        }   
                        else{
                            for(size_t j=0;j<col;++j){
                                double sumColJ=0;
                                for(size_t i=0;i<row;++i){
                                    if(elem.contains({i,j})){
                                        sumColJ+=abs(elem.at({i,j}));                                    
                                    }
                                }
                                if(sumColJ>res){
                                    res=sumColJ;
                                }
                            }
                        }                 
                    }
                    else{
                        if(is_compressed()){
                            for(size_t i=0;i<inner.size()-1;++i){
                                double sumColI=0;
                                for(size_t j=inner[i];j<inner[i+1];++j){
                                    sumColI+=abs(val[j]);
                                }
                                if(sumColI>res){
                                    res=sumColI;
                                }
                            }
                        }
                        else{
                            for(size_t j=0;j<col;++j){
                                double sumColJ=0;
                                for(auto it=elem.lower_bound({0,j});it!=elem.upper_bound({row,j});++it){
                                    sumColJ+=abs(it->second);
                                }
                                if(sumColJ>res){
                                    res=sumColJ;
                                }
                            }
                        }
                    }
                }
                else if constexpr (N==Norm::Frobenius){
                    //computation of the Frobenius norm in the proper way depending on the way it's stored
                    if(is_compressed()){
                        for(size_t i=0;i<val.size();++i){
                            res+=pow(abs(val[i]),2);
                        }
                        res=sqrt(res);
                    }
                    else{
                        for(auto it: elem){
                            res+=pow(abs(it.second),2);
                        }
                        res=sqrt(res);
                    }
                }
                else {
                    //if the computation arrives here there is a bug so it stops
                    std::cout<<"\n\nType of norm not supported\n\n"<<std::endl;
                    exit(0);
                }
                return res;
            }


            void display()const{ //shows the internal storage of the matrix
                if(!is_compressed()){
                    for(auto it: elem){ //prints the map
                        std::cout<<"( "<<it.first[0]<<" , "<<it.first[1]<<")"<<it.second<<std::endl;
                    }
                    std::cout<<"\n"<<std::endl;
                }
                else{  //prints the 3 vectors
                    for(size_t i=0;i<val.size();++i)
                        std::cout<<val[i]<<"  ";
                    std::cout<<"\n"<<std::endl;

                    for(size_t i=0;i<outer.size();++i)
                        std::cout<<outer[i]<<"  ";
                    std::cout<<"\n"<<std::endl;

                    for(size_t i=0;i<inner.size();++i)
                        std::cout<<inner[i]<<"  ";
                    std::cout<<"\n"<<std::endl;
                }
            }



    };

    template <typename T,StorageOrder S>
    std::vector<T> operator * (const Matrix<T,S>& A,const std::vector<T>& b){ //computes the matrix*vector product
        std::vector<T> res(A.row);
        if(b.size()!=A.col){
            //check of the dimension compatibility, in case of incopatibility prints an error and stops
            std::cerr<<"\n\nIncompatible size for this operation\n\n"<<b.size()<<" and "<<A.col<<std::endl;
            exit(0);
        }
        //computation of the product according the way the matrix is stored
        if constexpr (S==StorageOrder::Row){
            if(A.is_compressed()){
                for(size_t i=0;i<A.inner.size()-1;++i){
                    for(size_t j=A.inner[i];j<A.inner[i+1];++j){
                        res[i]+=A.val[j]*b[A.outer[j]];
                    }
                }
            }
            else{
                for(size_t i=0;i<A.row;++i){
                    for(auto it=A.elem.lower_bound({i,0});it!=A.elem.end() && it!=A.elem.lower_bound({i+1,0});++it){
                        res[i]+=it->second*b[it->first[1]];
                    }
                }
            }                    
        }
        else {
            if(A.is_compressed()){
                for(size_t j=0;j<A.inner.size()-1;++j){
                    for(size_t i=A.inner[j];i<A.inner[j+1];++i){
                        res[A.outer[i]]+=A.val[i]*b[A.outer[i]];
                    }
                }
            }
            else{
                for(size_t j=0;j<A.col;++j){
                    for(auto it=A.elem.lower_bound({0,j});it!=A.elem.end() && it!=A.elem.lower_bound({0,j+1});++it){
                        res[it->first[0]]+=b[it->first[0]]*it->second;
                    }
                }
            }
        }

        return res;
    }


//has a problem in the linking and time is running over so I leave it commented moreover it's not efficient at all but in my opinion should work
//a more effiecient way was in develping but the time wasn't enough
   /*template<typename T,StorageOrder V,StorageOrder S>   
    Matrix<T,StorageOrder::Row> operator * (const Matrix<T,S>& A,const Matrix<T,V>& B){
        if(A.col!=B.row){
            std::cerr<<"\n\nIncompatible size for operation\n\n"<<std::endl;
            exit(0);
        }
        Matrix<T,StorageOrder::Row> res(A.row,B.col);

        for(size_t i=0;i<res.row;++i){
            for(size_t j=0;j<res.col;++j){
                T Res=0;
                for(size_t k=0;k<A.col;++k){
                    Res+=A(i,k)*B(k,j);
                }
                if(Res!=0){
                    res(i,j)=Res;
                }
            }
        }
        return res;
    }*/    

    template<typename T,StorageOrder S>
    void print(const Matrix<T,S>& A){  //prints the matrix in the most classic way
        for(size_t i=0;i<A.row;++i){
            for(size_t j=0;j<A.col;++j){
                std::cout<<A(i,j)<<"    ";
            }
        std::cout<<std::endl;
        }
        std::cout<<"\n"<<std::endl;
        return;
    }

    template<typename T,StorageOrder S>
    void read(const std::string& filename,Matrix<T,S>& A){ //reads and initialize the matrix form a file
        if(A.is_compressed()){
            //prints the error and stops because the reading can be done only in the uncompressed form
            std::cout<<"Cannot insert elements in compressed form"<<std::endl;
            exit(0);
        }
        A.elem.clear(); //clears the map
        size_t i=0,j=0;
        T k=0;
        std::ifstream File(filename);
        std::string line="%";
        while(line[0]=='%'){ //ignores all the commented lines of the file (that starts with %)
            getline(File,line);
        }
        
        std::istringstream util(line);
        util>>A.row>>A.col; //reads the number of rows and the number of column

        while(getline(File,line)){ //reads and stores all the new elements
            std::istringstream utile(line);
            utile>>i>>j>>k;
            A(i-1,j-1)=k;
        }
        return;
    }

}

#endif