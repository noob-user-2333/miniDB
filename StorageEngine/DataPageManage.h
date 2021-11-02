//
// Created by user on 2021/10/30.
//

#ifndef MINIDB_DATAPAGEMANAGE_H
#define MINIDB_DATAPAGEMANAGE_H
#include "DataPage.h"

class DataPageManage{
private:
    IPageAccess* page_access;
    UINT page_size;
    DataPage* current_page = new DataPage(nullptr);

    INT OverflowPageWrite(VOID *buffer,UINT length,UINT *out_first_page_id)
    {
        UINT need_pages = length / (page_size - sizeof(UINT));
        if(length % (page_size - sizeof(UINT)))
            need_pages++;
        DiskPage *page = nullptr;
        DiskPage *pre_page=nullptr;
        UINT page_id;
        INT status;
        UINT write_bytes;
        for(;need_pages && length;need_pages--)
        {
            status = page_access->AllocPage(&page);
            write_bytes = page_size - sizeof(UINT);
            if(status != DB_SUCCESS)
                return status;
            if(pre_page)
            {
                page_id = page->PageID();
                pre_page->Write(&page_id,sizeof(UINT),0);
            }
            else
                *out_first_page_id = page->PageID();
            if(write_bytes > length)
                write_bytes = length;
            page->Write(buffer,write_bytes,sizeof(UINT));

            buffer = (UCHAR*)buffer + write_bytes;
            length -= write_bytes;
            pre_page = page;
        }
        return DB_SUCCESS;
    }

    INT OverflowRead(UINT start_page_id,VOID *buffer,UINT length)
    {
        UINT read_bytes  =  page_size - sizeof(UINT);
        UINT page_count = length / read_bytes;
        if(length%read_bytes)
            page_count++;
        if(start_page_id < 2)
            return DB_INVALID_PARA;
        DiskPage *page = nullptr;
        INT status;
        for(;page_count;page_count--)
        {
            read_bytes = page_size - sizeof(UINT);
            status = page_access->GetPage(start_page_id,&page);
            if(status!=DB_SUCCESS)
                return status;
            if(read_bytes > length)
                read_bytes = length;
            page->Read(buffer,read_bytes, sizeof(UINT));
            page->Read(&start_page_id, sizeof(UINT),0);
            length -= read_bytes;
            buffer = (UCHAR*)buffer + read_bytes;
        }
        return DB_SUCCESS;
    }


public:
    DataPageManage(IPageAccess* page_access,UINT page_size) {
        assert(page_access);
        this->page_access = page_access;
        this->page_size = page_size;
        assert(page_size >= 1024 && page_size <= 4096 * 4);
        DiskPage *page;
        assert(page_access->GetPage(1,&page) ==  DB_SUCCESS);
        current_page->ChangPage(page);
    }

    ~DataPageManage()
    {
        delete current_page;
    }

    UINT  FormatPage(DiskPage *page)
    {
        current_page->ChangPage(page);
        current_page->Format();
        return DB_SUCCESS;
    }
    UINT Insert(UINT page_id,VOID *buffer,UINT length)
    {
        if(page_id == 0)
            return DB_INVALID_PARA;
        DiskPage *page;
        INT status = page_access->GetPage(page_id,&page);
        current_page->ChangPage(page);
        if(status != DB_SUCCESS)
            return status;

        if(length > page_size >> 1)
        {
            UINT first_page_id = 0;
            status = OverflowPageWrite(buffer,length,&first_page_id);
            if(status != DB_SUCCESS)
                return status;
            if(current_page->Insert(&first_page_id,length) != DB_SUCCESS) {
                status = page_access->AllocPage(&page);
                if (status != DB_SUCCESS)
                    return status;
                page->ExchangeContent((DiskPage *)(*current_page));
                current_page->Insert(&first_page_id,length);
                current_page->NextPageID(page->PageID());
                return DB_SUCCESS;
            }
        }
        if(current_page->Insert(buffer,length) != DB_SUCCESS)
        {
            status = page_access->AllocPage(&page);
            if(status != DB_SUCCESS)
                return status;
            page->ExchangeContent((DiskPage*)*current_page);
            status =  current_page->Insert(buffer,length) ;
            if(status != DB_SUCCESS)
                return status;
            current_page->NextPageID(page->PageID());
        }
        return DB_SUCCESS;
    }

//    UINT Delete()
//    {
//        return DB_SUCCESS;
//    }
//
    UINT Quary(UINT page_id,INT index,VOID **out_buffer,ULONG *out_length,UINT *out_next_page_id = nullptr)
    {
        if(page_id == 0 || out_buffer == nullptr || out_length == nullptr)
            return DB_INVALID_PARA;
        ULONG length;
        USHORT  offset;
        DiskPage*page;
        if(out_next_page_id)
            *out_next_page_id = 0;
        INT status = page_access->GetPage(page_id,&page);
        if(status != DB_SUCCESS)
            return status;
        current_page->ChangPage(page);
        status = current_page->ReadIndex(index,&length,&offset);
        if(status  != DB_SUCCESS)
            return status;
        if(out_next_page_id)
            *out_next_page_id = current_page->NextPageID();
        VOID *buffer = malloc(length);
        *out_buffer = buffer;
        *out_length = length;
//        return DB_SUCCESS;
//
        if(length > page_size >> 1) {
            page->Read(&page_id, sizeof(UINT), offset);
            return OverflowRead(page_id,*out_buffer,length);
//            return DB_SUCCESS;
        }
        return page->Read(*out_buffer,length,offset);
    }
};

#endif //MINIDB_DATAPAGEMANAGE_H
