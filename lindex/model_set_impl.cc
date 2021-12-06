#include "lindex/model_set.h"

namespace leveldb{

LModelSet::LModelSet(){
    //
}

LModelSet::LModelSet(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &positions){
            this->init(keys, positions);
        };

double LModelSet::improvement_spliting_here(uint64_t keys_begin, 
                                uint64_t keys_end,
                                uint64_t pos_begin,
                                uint64_t pos_end,
                                uint64_t point,
                                uint64_t point_p){
    // L = abs((y1-y2)x3+(x2-x1)y3+x1y2-y1x2)/sqrt((y1-y2) ^ 2 +(x1-x2) ^ 2)
    double x1=this->keys[keys_begin].data;
    double x2=this->keys[keys_end].data;
    double y1=this->positions[pos_begin];
    double y2=this->positions[pos_end];
    double x3 = this->keys[point].data;
    double y3 = this->positions[point_p];

    return (fabs((y2 - y1) * x3 +(x1 - x2) * y3 + ((x2 * y1) -(x1 * y2)))) / (sqrt(pow(y2 - y1, 2.0) + pow(x1 - x2, 2.0)));
}

void LModelSet::top_down_split_models(uint64_t keys_begin, 
                            uint64_t keys_end, 
                            uint64_t pos_begin,
                            uint64_t pos_end,
                            double max_error){
    
    if(keys_end-keys_begin<=1){
        LModel<uint64_t> m;
        m.training(this->keys, this->positions, 
                    keys_begin, 
                    keys_end, 
                    pos_begin, 
                    pos_end);
        this->model_set.binsert_left(m);
        return;
    }
    double best_so_far = 0.0;
    uint64_t break_point = keys_begin, break_point_p = pos_begin;

    double improvement_in_approximation = 0.0;

    // find best place to split linear
    uint64_t it = keys_begin+1, pt = pos_begin+1;
    for(;it<keys_end&&pt<pos_end; it++, pt++){
        improvement_in_approximation = improvement_spliting_here(keys_begin,
                                                                keys_end,
                                                                pos_begin,
                                                                pos_end,
                                                                it,
                                                                pt
                                                                );
        if(improvement_in_approximation>best_so_far){
            break_point = it;
            break_point_p = pt;
            best_so_far = improvement_in_approximation;
        }
    }

    // Recursively split the left&right segment
    if(best_so_far>max_error){
        top_down_split_models(keys_begin, 
                                break_point,
                                pos_begin,
                                break_point_p,
                                max_error);
        top_down_split_models(break_point,
                                keys_end,
                                break_point_p,
                                pos_end,
                                max_error);
    }else{
        LModel<uint64_t> m;
        m.training(this->keys, this->positions, 
                    keys_begin, 
                    keys_end, 
                    pos_begin, 
                    pos_end);
        this->model_set.binsert_left(m);
    }
}

void LModelSet::split_model(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &pos){
    return top_down_split_models(0,
                            keys.size()-1,
                            0,
                            pos.size()-1,
                            this->error);
}

LModelSet::LModel<uint64_t>& bsearch_model_left(const uint64_t &key) const{
    LModel<uint64_t> tmp;
    tmp.max_key = key;
    return this->model_set.bsearch_left(tmp);
}

}