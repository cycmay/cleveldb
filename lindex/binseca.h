#pragma once

#include <vector>
#include <iostream>

namespace leveldb{
    template<class T>
    class Binseca{

        public:
        Binseca(){};
        ~Binseca(){
            this->clear();
        };

        private:
        std::vector<T> arr;
        typedef typename std::vector<T>::const_iterator arr_cit_t;

        public:
        // search element in binseca, and then return the reference
        const T & bsearch_left(const T &e) const{
            uint64_t low = bsearch_left_index(e);
            return this->arr[low];
        }

        // search element in binseca, and then return the index
        uint64_t bsearch_left_index(const T &e) const{
            uint64_t low=0, high=this->arr.size();
            while(low<high){
                uint64_t mid = (high+low)/2;
                if(this->arr[mid]<e){
                    low = mid+1;
                }else{
                    high = mid;
                }
            }
            return low;
        }
        const T & binsert_left(const T e){
            this->arr.emplace_back(e);
            size_t i=this->arr.size()-1;
            while(i>0){
                if(this->arr[i-1]>=e){
                    this->arr[i]=this->arr[i-1];
                }else{
                    break;
                }
                i--;
            }
            this->arr[i]=e;
            return this->arr[i];
        }
        
        void clear(){
            this->arr.clear();
        }

        arr_cit_t cbegin(){
            return this->arr.cbegin();
        }
        arr_cit_t cend(){
            return this->arr.cend();
        }

        T get_min(){
            if(this->arr.size()>0){
                return this->arr[0];
            }else{
                return T(0);
            }
        }
        const T & get_max(){
            assert(this->arr.size()>0);
            return this->arr[this->arr.size()-1];
        }
    };
}