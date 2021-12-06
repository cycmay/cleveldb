#pragma once
#include <array>
#include <vector>

namespace leveldb{

class LModel
{
    // the linear regression model of learned index

public:
    LModel()=default;
    LModel(const LModel &)=delete;
    ~LModel()=default;
    

    LModel & operator=(const LModel &)=delete;

    friend bool operator<(const LModel &a, const LModel &b){
        return a.max_key<b.max_key;
    }
    friend bool operator>(const LModel &a, const LModel &b){
        return a.max_key>b.max_key;
    }
    friend bool operator<=(const LModel &a, const LModel &b){
        return a.max_key<=b.max_key;
    }
    friend bool operator>=(const LModel &a, const LModel &b){
        return a.max_key>=b.max_key;
    }

    // train parameters using data set
    void Training(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &positions);
    
    // predict position
    uint64_t Predict(const uint64_t &key) const;
    // the array of model parameters
    std::array<double, 2> weights;
    // key range
    uint64_t min_key;
    uint64_t max_key;
    // loss
    double loss;

private:
void __training(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &positions,
                    uint64_t k_begin, 
                    uint64_t k_end, 
                    uint64_t p_begin,
                    uint64_t p_end);

};

}



