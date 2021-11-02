//
// Created by user on 2021/10/30.
//

#ifndef MINIDB_DATABASEHEADER_H
#define MINIDB_DATABASEHEADER_H

#include "DiskPage.h"


class DataBaseHeader {
private:

    DiskPage *page;

    VOID HeaderString(CHAR *string);

    VOID PageSize(UINT number);


    VOID MaxNotFreePageID(UINT number);

    VOID PageCacheSize(UINT number);

    VOID VersionID(UINT number);

    INT Check() {
        CHAR *headerString = HeaderString();
        for (UINT index = 0; index < 16; index++) {
            if (headerString[index] != HEADER_STRING[index]) {
                delete headerString;
                return DB_ERROR_DATA;
            }
        }
        delete headerString;
        UINT page_size = PageSize();
        if (page_size < 1024 || page_size > 4 * 4096)
            return DB_ERROR_DATA;

        return DB_SUCCESS;
    }

public:
    static const INT SIZE = 64;
    static const UINT PAGE_SIZE = 4096;
    static const UINT VERSION_ID = 0x00000100;
    const CHAR *HEADER_STRING = "miniDB           ";

    DataBaseHeader(DiskPage *page, bool need_format = false) {
        if (page == nullptr || page->PageID() != 0)
            throw "数据库头的page_id必须为0";
        this->page = page;
        if (need_format)
            Format();
        if (Check() != DB_SUCCESS) {
            Format();
            std::cout << "数据库头损坏，初始化整个数据库!!!" << std::endl;
        }
    }

    void Format() {
        page->Format();
        HeaderString((CHAR *) HEADER_STRING);
        PageSize(PAGE_SIZE);
        OverflowPageID(0);
        FreePageCount(0);
        FileChangeCount(0);
        MaxNotFreePageID(0);
        PageCacheSize(0);
        VersionID(VERSION_ID);
    }

    VOID FreePageCount(UINT number);

    VOID FileChangeCount(UINT number);

    VOID OverflowPageID(UINT number);

    CHAR *HeaderString();

    UINT PageSize();

    UINT OverflowPageID();

    UINT FreePageCount();


    UINT FileChangeCount();

    UINT MaxNotFreePageID();

    UINT PageCacheSize();

    UINT VersionID();

    explicit operator DiskPage *() {
        return page;
    }
};

CHAR *DataBaseHeader::HeaderString() {
    CHAR *result = new CHAR[16];
    page->Read(result, 16, 0);
    return result;
}

UINT DataBaseHeader::PageSize() {
    UINT result;
    page->Read(&result, sizeof(result), 16);
    return result;
}

UINT DataBaseHeader::OverflowPageID() {
    UINT result;
    page->Read(&result, sizeof(result), 20);
    return result;
}

UINT DataBaseHeader::FreePageCount() {
    UINT result;
    page->Read(&result, sizeof(result), 24);
    return result;
}

UINT DataBaseHeader::FileChangeCount() {
    UINT result;
    page->Read(&result, sizeof(result), 28);
    return result;
}

UINT DataBaseHeader::MaxNotFreePageID() {
    UINT result;
    page->Read(&result, sizeof(result), 32);
    return result;
}

UINT DataBaseHeader::PageCacheSize() {
    UINT result;
    page->Read(&result, sizeof(result), 36);
    return result;
}

UINT DataBaseHeader::VersionID() {
    UINT result;
    page->Read(&result, sizeof(result), 60);
    return result;
}


VOID DataBaseHeader::HeaderString(CHAR *string) {
    page->Write(string, 16, 0);
}

VOID DataBaseHeader::PageSize(UINT number) {
    page->Write(&number, sizeof(number), 16);
}

VOID DataBaseHeader::OverflowPageID(UINT number) {
    page->Write(&number, sizeof(number), 20);
}

VOID DataBaseHeader::FreePageCount(UINT number) {
    page->Write(&number, sizeof(number), 24);
}

VOID DataBaseHeader::FileChangeCount(UINT number) {
    page->Write(&number, sizeof(number), 28);
}

VOID DataBaseHeader::MaxNotFreePageID(UINT number) {
    page->Write(&number, sizeof(number), 32);
}

VOID DataBaseHeader::PageCacheSize(UINT number) {
    page->Write(&number, sizeof(number), 36);
}

VOID DataBaseHeader::VersionID(UINT number) {
    page->Write(&number, sizeof(number), 60);
}


#endif //MINIDB_DATABASEHEADER_H
