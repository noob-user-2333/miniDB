//
// Created by user on 2021/10/29.
//

#ifndef MINIDB_STORAGEENGINE_H
#define MINIDB_STORAGEENGINE_H

#include <string>
#include "DiskPageManage.h"
#include "DataPageManage.h"

class IDataAccess{
public:
    virtual UINT Insert(UINT page_id,VOID *buffer,UINT length) = 0;
    virtual UINT Quary(UINT page_id,INT index,VOID **out_buffer,ULONG *out_length,UINT *out_next_page_id=nullptr) = 0;
    virtual UINT AllocDataPage(UINT *out_page_id) = 0;
};


class StorageEngine:public IDataAccess {
private:
    const UINT page_size = 4096;
    const CHAR* END_WITH = ".miniDB";
    std::string file_name;
    DiskPageManage* disk_page_manage;
    DataPageManage* data_page_manage;





public:
    StorageEngine(CHAR *file_name) {
        this->file_name = file_name;
        ULONG name_length = this->file_name.size();
        if (name_length < 7) {
            std::cout << "文件名过短,必须以.miniDB结尾" << std::endl;
            _exit(0);
        }
        name_length -= 7;
        for (ULONG times = 0; times < 7; times++) {
            if (END_WITH[times] != file_name[name_length + times]) {
                std::cout << "文件名必须以.miniDB结尾" << std::endl;
                _exit(0);
            }
        }
        disk_page_manage = new DiskPageManage(file_name, page_size);
        data_page_manage = new DataPageManage(disk_page_manage,page_size);
    }

    ~StorageEngine()
    {
        delete data_page_manage;
        delete disk_page_manage;
    }

    UINT Insert(UINT page_id,VOID *buffer,UINT length)
    {
        UINT status = data_page_manage->Insert(page_id,buffer,length);
        if(status != DB_SUCCESS)
            return status;
        return disk_page_manage->Flush();
    }

    UINT Quary(UINT page_id,INT index,VOID **out_buffer,ULONG *out_length,UINT *out_next_page_id = nullptr)
    {
        return data_page_manage->Quary(page_id,index,out_buffer,out_length,out_next_page_id);
    }

    UINT AllocDataPage(UINT *out_page_id)
    {
        DiskPage *page;
        UINT status = disk_page_manage->AllocPage(&page);
        if(status != DB_SUCCESS)
            return status;
        status = data_page_manage->FormatPage(page);
        *out_page_id = page->PageID();
        return status;
    }


};

#endif //MINIDB_STORAGEENGINE_H
