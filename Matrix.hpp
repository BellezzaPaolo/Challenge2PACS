#ifndef MATRIX_HPP
#define MATRIX_HPP

#include<map>
#include<vector>

namespace algebra{
    template<typename T>
    using mappa=std::map<std::array<std::size_t,2>, T>;

    enum class StorageOrder {Column,Row};

    template<typename T, enum StorageOrder S>
    class Matrix{
        private:
            mappa<T> elem;

            std::vector<std::size_t> inner;
            std::vector<std::size_t> outer;
            std::vector<T> val;

            std::size_t row;
            std::size_t col;

    /*        bool operator < (std::array<size_t,2> a,std::array<size_t,2> b){
                if constexpr (S==StorageOrder::Column){
                    if(a[1]==b[1]){
                        return a[0]<b[0];
                    }
                    return a[1]<b[1];
                }
                else if constexpr (S==StorageOrder::Row){
                    default;
                }
            }
*/
        public:
            Matrix(std::size_t R,std::size_t C){
                mappa<T> e;
                std::vector<std::size_t> I,O;
                std::vector<T> V;
                inner=I;outer=O;elem=e;row=R;col=C;
            };
                
            T& operator () (std::size_t i,std::size_t j){
                if(i>=row ||j>=col){
                    std::wcerr<<"\n\nIndex exceed bounds of the matrix\n\n"<<std::endl;
                }           
                if(is_compressed()){
                    if constexpr (S==StorageOrder::Row){
                        for(size_t k=inner[i];k<inner[i+1];++k){
                            if(outer[k]==j){
                                return elem[{i,j}];
                            }
                        }
                        std::wcerr<<"\n\nCannot insert a new element in compressed form\n\n"<<std::endl;                        
                    }
                    else if constexpr (S==StorageOrder::Column){

                    }
                }
                std::array<std::size_t,2> key={i,j};
                return elem[key];
            }

            T operator () (std::size_t i,std::size_t j) const{
                std::array<std::size_t,2> key={i,j};
                auto it=elem.find(key);
                if(it==elem.end()){
                    return 0;
                }
                return it->second;
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

                }
            }

            bool is_compressed() const{
                return !(val.empty() && inner.empty() && outer.empty());
            }

            void resize(std::size_t R,std::size_t C){
                row=R;
                col=C;

                
            }

            void print()const{
                if(!is_compressed()){
                    for(std::size_t i=0;i<row;++i){
                        for(std::size_t j=0;j<col;++j){
                            std::array<std::size_t,2> key={i,j};
                            if(elem.find(key)!=elem.end())
                                std::cout<<(elem.find(key))->second<<"  ";
                            else
                                std::cout<<"0   ";
                        }
                        std::cout<<std::endl;
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

}

#endif