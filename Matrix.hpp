#ifndef MATRIX_HPP
#define MATRIX_HPP

#include<map>
#include<vector>
#include<cmath>

namespace algebra{

    enum class StorageOrder {Column,Row};
    enum class Norm {One, Infinity, Frobenius};

    template<StorageOrder S>
    struct comp{
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
    using mappa=std::map<std::array<std::size_t,2>, T,comp<S>>;


    template<typename T, enum StorageOrder S>
    class Matrix{
        private:
            mappa<T,S> elem;

            std::vector<std::size_t> inner;
            std::vector<std::size_t> outer;
            std::vector<T> val;

            std::size_t row;
            std::size_t col;



        public:

            friend std::vector<T> operator * <> (const Matrix<T,S>& A,const std::vector<T>& b);

            template<StorageOrder V>
            friend Matrix<T,StorageOrder::Row> operator * (const Matrix<T,S>& A,const Matrix<T,V>& B);

            friend void print <>(const Matrix<T,S>& A);

            Matrix(std::size_t R,std::size_t C){
                mappa<T,S> e;
                std::vector<std::size_t> I,O;
                std::vector<T> V;
                inner=I;outer=O;elem=e;row=R;col=C;
            };
                
            T& operator () (std::size_t i,std::size_t j){
                if(i>=row ||j>=col){
                    std::cerr<<"\n\nIndex exceed bounds of the matrix\n\n"<<std::endl;
                    exit(0);
                }           
                if(is_compressed()){
                    if constexpr (S==StorageOrder::Row){
                        for(size_t k=inner[i];k<inner[i+1];++k){
                            if(outer[k]==j){
                                return val[k];
                            }
                        }
                        std::wcerr<<"\n\nCannot insert a new element in compressed form\n\n"<<std::endl;
                        exit(0);                        
                    }
                    else if constexpr (S==StorageOrder::Column){
                        for(size_t k=inner[j];k<inner[j+1];++k){
                            if(outer[k]==i){
                                return val[k];
                            }
                        }
                        std::wcerr<<"\n\nCannot insert a new element in compressed form\n\n"<<std::endl;
                        exit(0);
                    }
                }
                std::array<std::size_t,2> key={i,j};
                return elem[key];
            }

            T operator () (std::size_t i,std::size_t j) const{
                if(i>row || j>col){
                    std::cerr<<"\n\nIndex excedds matrix bounds\n\n"<<std::endl;
                    exit(0);
                }
                if(is_compressed()){
                    if constexpr (S==StorageOrder::Row){
                        if(inner[i]!=inner[inner.size()-1]){
                            for(size_t k=inner[i];k<inner[i+1];++k){
                                if(outer[k]==j){
                                    return val[k];
                                }
                            }
                        }
                        return 0;
                    }
                    else{
                        if(inner[j]!=inner[inner.size()-1]){
                            for(size_t k=inner[j];k<inner[j+1];++k){
                                if(outer[k]==i){
                                    return val[k];
                                }
                            }
                        }
                        return 0;
                    }
                }
                else{
                    std::array<std::size_t,2> key={i,j};
                    auto it=elem.find(key);
                    if(it==elem.end()){
                        return 0;
                    }
                    return it->second;                
                }
            }
            
            void compress(){
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

            void uncompress(){
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

            bool is_compressed() const{
                return !(val.empty() && inner.empty() && outer.empty());
            }

            void resize(std::size_t R,std::size_t C){
                if(is_compressed()){
                    std::cout<<"\n\nto resize the matrix needs to be in an uncompressed state\n\n"<<std::endl;
                    exit(0);
                }
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
                row=R;
                col=C;
                return;
            }

            template <enum Norm N>
            double norm () const{
                double res=0;
                if constexpr (N==Norm::Infinity){
                    if constexpr (S==StorageOrder::Row){
                        if(is_compressed()){
                            for(size_t i=0;i<inner.size()-1;++i){
                                double sumRowI=0;
                                for(size_t j=inner[i];j<inner[i+1];++j){
                                    sumRowI+=fabs(val[j]);
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
                                    sumRowI+=fabs(it->second);
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
                                        sumRowI+=val[k];
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
                                    sumRowI+=fabs(elem.at({i,j}));
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
                    if constexpr (S==StorageOrder::Row){
                        if(is_compressed()){
                            for(size_t j=0;j<col;++j){
                                double sumColJ=0;
                                for(size_t k=0;k<outer.size();++k){
                                    if(outer[k]==j){
                                        sumColJ+=fabs(val[k]);
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
                                        sumColJ+=fabs(elem.at({i,j}));                                    
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
                                    sumColI+=fabs(val[j]);
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
                                    sumColJ+=fabs(it->second);
                                }
                                if(sumColJ>res){
                                    res=sumColJ;
                                }
                            }
                        }
                    }
                }
                else if constexpr (N==Norm::Frobenius){
                    if(is_compressed()){
                        for(size_t i=0;i<val.size();++i){
                            res+=pow(fabs(val[i]),2);
                        }
                        res=sqrt(res);
                    }
                    else{
                        for(auto it: elem){
                            res+=pow(fabs(it.second),2);
                        }
                        res=sqrt(res);
                    }
                }
                else {
                    std::cout<<"\n\nType of norm not supported\n\n"<<std::endl;
                    exit(0);
                }
                return res;
            }


            void display()const{
                if(!is_compressed()){
                    for(auto it: elem){
                        std::cout<<"( "<<it.first[0]<<" , "<<it.first[1]<<")"<<it.second<<std::endl;
                    }
                    std::cout<<"\n"<<std::endl;
                }
                else{
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
    std::vector<T> operator * (const Matrix<T,S>& A,const std::vector<T>& b){
        std::vector<T> res(A.row);
        if(b.size()!=A.col){
            std::cerr<<"\n\nIncompatible size for this operation\n\n"<<std::endl;
            exit(0);
        }

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
                
/*    prova anche copiando la riga in un vettore
        for(size_t i=0;i<A.row;++i){
                T resI=0;
                for(size_t j=0; j<A.col;++j){
                    if(A.elem.contains({i,j})){
                        resI+=A.elem.at({i,j})*b[j];
                    }
                }
                res.push_back(resI);
            }*/ 
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

    template<typename T,StorageOrder V,StorageOrder S>
    Matrix<T,StorageOrder::Row> operator * (Matrix<T,S>& A,Matrix<T,V>& B){
        if(A.col!=B.row){
            std::cerr<<"\n\nIncompatible size for operation\n\n"<<std::endl;
            exit(0);
        }
        Matrix<T,StorageOrder::Row> res(A.row,B.col);
        if constexpr (V==StorageOrder::Row){

        }
        else if constexpr (V==StorageOrder::Column){


        }
    }           

    template<typename T,StorageOrder S>
    void print(const Matrix<T,S>& A){
        for(size_t i=0;i<A.row;++i){
            for(size_t j=0;j<A.col;++j){
                std::cout<<A(i,j)<<"   ";
            }
        std::cout<<std::endl;
        }
        return;
    }

    }

#endif