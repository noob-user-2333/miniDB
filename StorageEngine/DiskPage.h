//
// Created by user on 2021/10/29.
//

#ifndef MINIDB_DISKPAGE_H
#define MINIDB_DISKPAGE_H

#include "../defs.h"
#include <memory.h>
#include <any>
#include <memory>
#include <unistd.h>

class DiskPage {
protected:
    UINT page_id;
    ULONG size;
    UCHAR* buffer;

public:
    bool valid;
    bool dirty;


    DiskPage(UINT page_id, UCHAR *buffer, ULONG size) {
        if(buffer == nullptr)
        {
            std::cout << "DiskPage对应Buffer不可为空"<<std::endl;
            _exit(0);
        }
        this->buffer = buffer;
        this->page_id = page_id;
//        this->buffer = (UCHAR*)buffer;
        this->size = size;
        valid = true;
        dirty = false;
    }

    ~DiskPage() {
        delete buffer;
    }

    ULONG Size() const {
        return size;
    }


    UINT PageID();


    VOID Format() {
        memset(buffer, 0, size);
    }

    VOID ExchangeContent(DiskPage *page) {
        UCHAR *buff = buffer;
//        buffer->swap(*(page->buffer));
        buffer = page->buffer;
        page->buffer = buff;
        dirty = true;
        page->dirty = true;
    }

    INT ChangePage(UINT page_id,VOID*buffer)
    {
        if(buffer == nullptr)
            return DB_INVALID_PARA;
        this->page_id = page_id;
        this->buffer = (UCHAR*)buffer;
        return DB_SUCCESS;
    }

    VOID const*Buffer()
    {
        return buffer;
    }
    INT Read(VOID *dest_buffer, INT length, INT offset) {
        if (offset + length > size || length < 0 || offset < 0)
            return DB_INVALID_PARA;
        memcpy(dest_buffer, buffer + offset, length);
//        memcpy(dest_buffer, ptr + offset, length);
        return DB_SUCCESS;
    }

    INT Write(VOID *source_buffer, INT length, INT offset) {
        if (offset + length > size || length < 0 || offset < 0)
            return DB_INVALID_PARA;
        memcpy(buffer + offset, source_buffer, length);
        dirty = true;
        return DB_SUCCESS;
    }

    explicit operator VOID *() { return buffer; }

};

//VOID *DiskPage::Buffer() {
//    return buffer;
//}

UINT DiskPage::PageID() {
    return page_id;
}

#endif //MINIDB_DISKPAGE_H
