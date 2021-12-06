#ifndef STORAGE_LEVELDB_DB_LINDEX_H_
#define STORAGE_LEVELDB_DB_LINDEX_H_

#include "leveldb/status.h"
#include "leveldb/db.h"
#include "db/db_impl.h"

namespace leveldb{

struct Rep;

class LIndex{
public:
    LIndex(const std::string & indexName);

    LIndex(const LIndex &)=delete;
    LIndex &operator = (const LIndex &) = delete;

    ~LIndex(){};

    Status Build(DBImpl *);

}; 
}

#endif