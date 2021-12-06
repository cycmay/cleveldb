#include "db/version_set.h"
#include "db/version_edit.h"
#include "util/mutexlock.h"
#include "db/filename.h"
#include "leveldb/table.h"
#include "leveldb/db.h"
#include "db/dbformat.h"
#include "table/block.h"
#include "table/filter_block.h"
#include "lindex/lindex.h"
#include <iostream>

namespace leveldb{

LIndex::LIndex(const std::string& indexName){
    //pass
}



Status LIndex::Build(DBImpl * impl){

    Status s;

    ReadOptions __options=ReadOptions();

    MutexLock l(&(impl->mutex_));

    Version * current = impl->versions_->current();
    current->Ref();

    {
        impl->mutex_.Unlock();

        FileMetaData *f = nullptr;
        std::string fname = "";
        RandomAccessFile * file = nullptr;
        Table * table = nullptr;

        Iterator * iiter = nullptr;

        size_t count=0;


        BlockHandle _bhandle;
        Slice _input;
        
        for(int level=0;level<config::kNumLevels; level++){
            for(uint32_t i=0; i<current->files_[level].size(); i++){
                f = current->files_[level][i];

                char buf[sizeof(f->number)];
                EncodeFixed64(buf, f->number);

                fname = TableFileName(impl->dbname_, f->number);
                file = nullptr;
                table = nullptr;

                s = impl->env_->NewRandomAccessFile(fname, &file);
                if(s.ok()){
                    s = Table::Open(impl->options_, file, f->file_size, &table);
                }
                if(!s.ok()){
                    assert(table == nullptr);
                    delete file;
                    continue;
                }

                iiter = table->rep_->index_block->NewIterator(table->rep_->options.comparator);
                iiter->SeekToFirst();
                while(iiter->Valid()){
                    _input = iiter->value();
                    s = _bhandle.DecodeFrom(&_input);
                    if(!s.ok()){
                        iiter->Next();
                    }
                    Iterator * block_iter = Table::BlockReader(table, __options, iiter->value());
                    block_iter->SeekToFirst();
                    while(block_iter->Valid()){
                        ParsedInternalKey parsed_key;
                        ParseInternalKey(block_iter->key(), &parsed_key);
                        std::cout<<" Key: "<<parsed_key.user_key.ToString()<<
                            ", Value: "<<block_iter->value().ToString()<<
                            ", SequenceNumber: "<<parsed_key.sequence<<
                            ", FileNumber: "<<f->number<<
                            ", Level: "<<level<<
                            ", FileSequence: "<<i<<
                            ", BlockOffset: "<<_bhandle.offset()<<
                            ", RestartPoint: "<<static_cast<Block::Iter *>(block_iter)->GetCurrentRestartPoint()<<
                            ", RestartIndex: "<<static_cast<Block::Iter *>(block_iter)->GetCurrentRestartIndex()<<
                            ", Current: "<<static_cast<Block::Iter *>(block_iter)->GetCurrent()<<std::endl;

                        // std::cout<<"Key: "<<block_iter->key().data()<<", Value: "<<block_iter->value().data()<<std::endl;
                        block_iter->Next();
                        count++;
                    }
                    s = block_iter->status();
                    delete block_iter;
                    iiter->Next();
                }
                if(s.ok()){
                    s = iiter->status();
                }
                delete iiter;
            }
        }
        std::cout<<"Count: "<<count<<std::endl;
        impl->mutex_.Lock();
    }
    current->Unref();
    return s;

}

}