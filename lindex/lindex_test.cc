#include <stdint.h>
#include <iostream>
#include <random>

#include "lindex/model.h"
#include "lindex/binseca.h"

class Key{

public:
    uint64_t data;
    Key(){};
    Key(uint64_t d):data(d){};
    Key & operator=(const Key &a){
        this->data=a.data;
        return *this;
    }
    Key(const Key &a){
        this->data=a.data;
    }

    friend bool operator<(const Key &l, const Key &r){return l.data<r.data;}
    friend bool operator>(const Key &l, const Key &r){return l.data>r.data;}
    friend bool operator<=(const Key &l, const Key &r){return l.data<=r.data;}
    friend bool operator>=(const Key &l, const Key &r){return l.data>=r.data;}

    static uint64_t EncodeStringToUint64(const char * const str){
        uint64_t dst = 0x0;
        const char *t = str;
        const uint8_t *p = nullptr;
        size_t i=0;
        while(*t!='\0' && i<8){
            p=reinterpret_cast<const uint8_t*>(t);
            dst <<= 8;
            dst = dst | (*p);
            t++;
            i++;
        }
        return dst;

    }
private:
    uint64_t _encode_data; // encode string to 64bit unsigned integer
    
};


void linear_model_test(){
    uint64_t Keys_number = 10000;
    std::vector<uint64_t> keys(Keys_number, 0);
    std::vector<uint64_t> positions(keys.size());
    std::vector<uint64_t> values(keys.size());


    leveldb::LModel test;
    std::random_device r;
    std::default_random_engine eng{r()};
    std::uniform_real_distribution<double> u(0.0, 1.0);
    

    std::uniform_int_distribution<uint64_t> rand_int64(
        0, Keys_number*2.7);

    keys.reserve(Keys_number);
    

    for (size_t i = 0; i < Keys_number; ++i) {
        keys.push_back(static_cast<uint64_t>(rand_int64(eng)));
        positions.push_back(static_cast<uint64_t>(i));
        values.push_back(1);
    }

    std::sort(keys.begin(), keys.end());

    test.Training(keys, positions);
    printf("min_key: %lld, max_key %lld\n", test.min_key, test.max_key);
    printf("weight: [%.3f, %.3f]\n", test.weights[0], test.weights[1]);
    printf("loss: %.3f\n", test.loss);

    std::uniform_int_distribution<uint64_t> rand_keynumber(0, keys.size());
    for(size_t i=0;i<5;i++){
        uint64_t p = static_cast<uint64_t>(rand_keynumber(eng));
        printf("Key [%lld] predict-position:[%lld] real-position [%lld]\n", keys[p], test.Predict(keys[p]), positions[p]);
    }
    return;
}
bool binseca_test(){
    leveldb::Binseca<int> binseca_test;
    std::random_device r;
    std::default_random_engine eng{r()};
    

    std::uniform_int_distribution<int64_t> rand_int64(
        0, 40);
    
    for(size_t i=0;i<10;i++){
        binseca_test.binsert_left(static_cast<uint64_t>(rand_int64(eng)));
    }
    for(auto it=binseca_test.cbegin(); it!=binseca_test.cend(); it++){
        std::cout<<*it<<" ";
    }
    for(auto it=binseca_test.cbegin()+1; it!=binseca_test.cend(); it++){
        // std::cout<<*it<<" ";
        if(!(*(it-1)<=(*it))){
            std::cout<<*(it-1)<<"<"<<*it<<" Failed!"<<std::endl;;
            return false;
        }
    }
    std::cout<<std::endl;
    rand_int64 = std::uniform_int_distribution<int64_t>(0, binseca_test.get_max());
    for(size_t i=0;i<3;i++){
        uint64_t p = static_cast<uint64_t>(rand_int64(eng));
        // std::cout<<p<<" "<<binseca_test.bsearch_left(p)<<std::endl;
        uint64_t index = binseca_test.bsearch_left_index(p);
        uint64_t left_index = index<=0?0:index-1;
        
        bool res = (index!=0 && p<=*(binseca_test.cbegin()+index) && p>*(binseca_test.cbegin()+left_index)) ||
                (index==0 && p<=*(binseca_test.cbegin()+index));
        if(!res){
            std::cout<<"ERROR: current find element:["<<p<<"], left element: "<<*(binseca_test.cbegin()+left_index)<<", index element: "<<
                *(binseca_test.cbegin()+index)<<std::endl;
            return false;
        }

    }
    return true;
}

int main(){
    linear_model_test();
    assert(binseca_test());
    return 0;
}