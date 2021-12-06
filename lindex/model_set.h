#pragma once

#include <vector>
#include "lindex/model.h"
#include "lindex/binseca.h"
#include "lindex/interval_tree.h"

namespace leveldb{
    class LModelSet : public Interval{
        public:
        Binseca<LModel<uint64_t> > model_set;

        uint64_t min_key;
        uint64_t max_key;

        LModelSet();
        LModelSet(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &positions)；
        ~LModelSet(){
            this->model_set.clear();
        };

        friend bool operator<(const LModelSet&a,const LModelSet&b){
            return a.max_key<a.max_key;
        }
        friend bool operator<=(const LModelSet&a,const LModelSet&b){
            return a.max_key<=a.max_key;
        }

        void init(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &positions){
            this->split_model(keys, positions);
            this->min_key = this->model_set.get_min().min_key ;
            this->low = static_cast<uint64_t>(this->min_key);
            this->max_key = this->model_set.get_max().max_key;
            this->high =  static_cast<uint64_t>(this->max_key);
        }

        private:
        double error = 50.0;
        double improvement_spliting_here(uint64_t keys_begin, 
                                uint64_t keys_end,
                                uint64_t pos_begin,
                                uint64_t pos_end,
                                uint64_t point,
                                uint64_t point_p);
        void top_down_split_models(uint64_t keys_begin, 
                            uint64_t keys_end, 
                            uint64_t pos_begin,
                            uint64_t pos_end,
                            double max_error);
        void split_model(const std::vector<uint64_t> &keys, const std::vector<uint64_t> &pos);

        public:
        LModel<uint64_t>& bsearch_model_left(const uint64_t &key) const;

    };
}