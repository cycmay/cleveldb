#include <iostream>
#include <math.h>

#include "lindex/model.h"


namespace leveldb{


void LModel::Training(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &positions)
{
    __training(keys, positions, 0, keys.size()-1, 0, positions.size()-1);
    return;
}


void LModel::__training(const std::vector<uint64_t> &keys, 
                        const std::vector<uint64_t> &positions,
                        uint64_t k_begin, 
                        uint64_t k_end, 
                        uint64_t p_begin,
                        uint64_t p_end){
    if(k_end-k_begin<0){
        return;
    }
    if(k_end-k_begin<1){
        this->min_key = keys[k_begin];
        this->max_key = keys[k_end];
        this->weights[0] = 1.0;
        this->weights[1] = static_cast<double>(positions[p_begin] - keys[k_begin]);
        this->loss=0.0;
        return;
    }
    
    double t1=0.0, t2=0.0, t3=0.0, t4=0.0;
    for(auto i=k_begin, j=p_begin; i<=k_end&&j<=p_end; i++,j++){
        double _key = keys[i];
        t1 += _key*_key;
        t2 += _key;
        t3 += _key*static_cast<double>(positions[j]);
        t4 += static_cast<double>(positions[j]);
    }
    this->weights[0] = (t3*static_cast<double>(k_end-k_begin+1) - t2*t4) / (t1*static_cast<double>(k_end-k_begin+1) - t2*t2); 
    this->weights[1] = (t1*t4 - t2*t3) / (t1*static_cast<double>(k_end-k_begin+1) - t2*t2);

    // calculate loss
    loss = 0.0;
    for(auto it=k_begin, jt=p_begin; it<=k_end&&jt<=p_end; it++, jt++){
        loss += pow((weights[0]*static_cast<double>(keys[it])+weights[1])-static_cast<double>(positions[jt]), 2.0)/abs(static_cast<double>(positions[p_end]-positions[p_begin]));
    }

    // save min & max key
    this->min_key = keys[k_begin];
    this->max_key = keys[k_end];
    return;
}


uint64_t LModel::Predict(const uint64_t &key) const{
    for(size_t i=0;i<this->weights.size();i++){
        assert(this->weights[i]);
    }
    return static_cast<uint64_t>(this->weights[0]*static_cast<double>(key)+this->weights[1]);
}


}




