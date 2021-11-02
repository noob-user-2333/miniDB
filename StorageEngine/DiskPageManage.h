//
// Created by user on 2021/10/29.
//

#ifndef MINIDB_DISKPAGEMANAGE_H
#define MINIDB_DISKPAGEMANAGE_H

#include "DiskPage.h"
#include "Disk.h"
#include "DataBaseHeader.h"
#include "OverflowPage.h"
#include <list>


class IPageAccess {
public:
    virtual  INT GetPage(UINT page_id, DiskPage **dest_page) = 0;

    virtual INT AllocPage(DiskPage **page) = 0;

    virtual INT FreePage(DiskPage *page) = 0;
};


class DiskPageManage : public IPageAccess {
private:
    const INT MAX_CAPACITY = 0x4000;
    std::string file_path;
    UINT page_size;
    Disk *disk;

    std::list<DiskPage *> *buffer;
    DataBaseHeader *database_header;
    OverflowPage *free_page_record;

    INT WritePage(DiskPage &page,bool force_write = false) {
        if (force_write || (page.valid && page.dirty) ) {
            INT status = disk->Write((VOID *) page, page_size, page_size * page.PageID());
            if (status == DB_SUCCESS)
                page.dirty = false;
            return status;
        }
        return DB_SUCCESS;
    }

    INT ReadPage(UINT page_id, DiskPage **out_page) {
        UCHAR *buff = (UCHAR*)malloc(page_size);
        INT status = disk->Read(buff, page_size, page_id * page_size);
        if (status != DB_SUCCESS)
            return status;
        *out_page = new DiskPage(page_id, buff, page_size);

        buffer->push_back(*out_page);
        return DB_SUCCESS;
    }

    INT AllocPhysicPage(DiskPage **out_page) {
        UINT page_id = disk->Length() / page_size;
        *out_page = new DiskPage(page_id, new UCHAR[page_size], page_size);
        buffer->push_front(*out_page);
        return WritePage(**out_page,true);
    }

public:
    DiskPageManage(CHAR* file_path, const UINT page_size) {
        this->file_path = file_path;
        this->page_size = page_size;
        buffer = new std::list<DiskPage *>();
        buffer->clear();
        if (page_size < 1024 || page_size > 4096 * 4) {
            std::cout << "数据页大小必须是1024~4096 * 4" << std::endl;
            _exit(0);
        }

        disk = new Disk(this->file_path);
        if (disk->Open() != DB_SUCCESS) {
            std::cout << "设备发生未知错误使数据库文件无法被打开" << std::endl;
            _exit(0);
        }

        DiskPage *root_page = nullptr;
        if (GetPage(0, &root_page) != DB_SUCCESS) {
            std::cout << "无法读取数据库头，程序退出！！！" << std::endl;
            _exit(0);
        }
        database_header = new DataBaseHeader(root_page, IsCreate());
        DiskPage *page = nullptr;

        if(database_header->FreePageCount()) {
            if(GetPage(database_header->OverflowPageID(), &page) == DB_SUCCESS)
                free_page_record = new OverflowPage(page);
            else
            {
                std::cout << "无法正常读取OverFlowPage,程序退出！！"<<std::endl;
                _exit(0);
            }
        }
        else
            free_page_record = nullptr;
    }

    ~DiskPageManage() {
        Flush();
        delete disk;
    }

    bool IsCreate();

    INT Flush()
    {
        UINT status;
        DiskPage *page = nullptr;
        for(auto it = buffer->begin();it != buffer->end();it++)
        {
            page = *it;
            status = WritePage(*page);
            if(status != DB_SUCCESS)
                return status;
        }
        return DB_SUCCESS;
    }

    INT AllocPage(DiskPage ** out_page) final {
        if(free_page_record == nullptr)
            return AllocPhysicPage(out_page);
        UINT free_page_id;
        if(free_page_record->TopOfPageID(&free_page_id) != DB_SUCCESS)
        {
            free_page_id = free_page_record->PageID();
            UINT next_page_id = free_page_record->NextPageID();
            database_header->OverflowPageID(next_page_id);
            if(next_page_id == 0) {
                delete free_page_record;
                free_page_record = nullptr;
            }
            else
            {
                GetPage(next_page_id,&free_page_record->page);
            }
        }
        else
            free_page_record->PageIDPop();
        database_header->FreePageCount(database_header->FreePageCount() - 1);
        return DB_SUCCESS;
    }

    INT FreePage(DiskPage *page) final{
        if(free_page_record == nullptr)
        {
            free_page_record= new OverflowPage(page);
            free_page_record->Format();
            database_header->OverflowPageID(page->PageID());
        }
        else
        {
            if(free_page_record->PageIDPush(page->PageID()) != DB_SUCCESS)
            {
                UINT next_page_id = free_page_record->NextPageID();
                page->Format();
                page->Write(&next_page_id, sizeof(UINT),0);
                free_page_record->NextPageID(page->PageID());
            }
        }
        database_header->FreePageCount(database_header->FreePageCount() + 1);
        return DB_SUCCESS;
    }

    INT GetPage(UINT page_id, DiskPage **out_dest_page) final{
        if (out_dest_page == nullptr)
            return DB_INVALID_PARA;
        if (page_id == disk->Length() / page_size)
            return AllocPhysicPage(out_dest_page);

        for (auto it = buffer->begin(); it != buffer->end(); it++) {
            if ((*it)->PageID() == page_id) {
                *out_dest_page = *it;
                return DB_SUCCESS;
            }
        }
        return ReadPage(page_id,out_dest_page);
    }

};


bool DiskPageManage::IsCreate() {
    return disk->IsCreate();
}

#endif //MINIDB_DISKPAGEMANAGE_H
