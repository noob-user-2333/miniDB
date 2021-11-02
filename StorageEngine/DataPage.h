//
// Created by user on 2021/10/30.
//

#ifndef MINIDB_DATAPAGE_H
#define MINIDB_DATAPAGE_H

#include "DiskPageManage.h"
#include "../PublicFunction.h"
struct FragmentStruct {
    USHORT next_fragment_offset_in_page;
    USHORT fragment_size;
};


class DataPage {
private:
    DiskPage *disk_page;
    VOID PageType(USHORT page_type);

    VOID ContentAreaStart(USHORT content_area_start);

    VOID FragmentOffset(USHORT fragment_offset);

    VOID DataCount(USHORT data_count);

    INT AllocSpace(UINT length,USHORT *out_offset_in_page)
    {
        if(MaxFreeSpaceSize() <= length + sizeof(USHORT))
            return DB_NO_FREE_SPACE;
        USHORT start_offset = ContentAreaStart();
        if(start_offset == 0)
            start_offset = disk_page->Size();
        *out_offset_in_page = start_offset - length;
        ContentAreaStart(*out_offset_in_page);
        return DB_SUCCESS;
    }

    INT WriteIndex(INT index,USHORT write_num)
    {
        if(index > DataCount())
            return DB_INVALID_PARA;
        if(index == DataCount())
            DataCount(DataCount()+1);
        return  disk_page->Write(&write_num, sizeof(USHORT),HEADER_SIZE + index * sizeof(USHORT));
    }

//
//    INT FreeSpace(INT index)
//    {
//        USHORT offset;
//        ULONG length;
//        UCHAR buff[10];
//        INT read_size = 10;
//        INT status = ReadIndex(index,&offset);
//        if(status != DB_SUCCESS)
//            return status;
//        if(offset + read_size > disk_page->Size())
//            read_size= disk_page->Size() - offset;
//        disk_page->Read(buff,read_size,offset);
//        length += PublicFunction::ReadVarInt(buff,&length);
//        WriteIndex(index,0);
//
//
//
//        if(offset == ContentAreaStart())
//            ContentAreaStart(ContentAreaStart() + length);
//        else
//        {
//
//
//        }
//
//
//
//        return DB_SUCCESS;
//    }

public:
    const INT HEADER_SIZE = 12;
    const USHORT TYPE_ID = 0x08;

    DataPage(DiskPage *page) {
        disk_page = page;
    }

    INT ChangPage(DiskPage *page)
    {
        if(page == nullptr)
            return DB_INVALID_PARA;
        disk_page = page;
        if(PageType() != TYPE_ID)
            Format();
        return DB_SUCCESS;
    }
    USHORT PageType();

    UINT NextPageID();

    VOID NextPageID(UINT next_page_id);

    USHORT ContentAreaStart();

    USHORT FragmentOffset();

    USHORT DataCount();

    VOID Format() {
        PageType(TYPE_ID);
        NextPageID(0);
        ContentAreaStart(0);
        FragmentOffset(0);
        DataCount(0);
    }
    UINT MaxFreeSpaceSize();

    INT ReadIndex(INT index,ULONG *out_length,USHORT *out_offset_in_page)
    {
        if(index >= DataCount())
            return DB_INVALID_PARA;
        disk_page->Read(out_offset_in_page, sizeof(USHORT),HEADER_SIZE + index * sizeof(USHORT));
        if(*out_offset_in_page == 0)
        {
            *out_length = 0;
            return DB_SUCCESS;
        }
        UCHAR buff[10];
        UINT read_bytes =10;
        if(*out_offset_in_page + read_bytes > disk_page->Size())
            read_bytes = disk_page->Size() - *out_offset_in_page;
        disk_page->Read(buff,read_bytes,*out_offset_in_page);
        *out_offset_in_page += PublicFunction::ReadVarInt(buff,out_length);
        return DB_SUCCESS;
    }

    INT Insert(VOID* buffer,UINT length)
    {
        if(PageType() != TYPE_ID)
            Format();
        UCHAR buff[10];
        USHORT offset;
        UINT length_of_length = PublicFunction::WriteVarInt(buff,length);
        UINT need_space = length_of_length;
        if(length > disk_page->Size() >> 1)
            need_space += sizeof(UINT);
        else
            need_space += length;
        INT status = AllocSpace(need_space,&offset);
        if(status != DB_SUCCESS)
            return status;
        WriteIndex(DataCount(),offset);
        disk_page->Write(buff,length_of_length,offset);
        offset += length_of_length;
        need_space -= length_of_length;
        return disk_page->Write(buffer,need_space,offset);
    }


    explicit operator DiskPage*()
    {
        return disk_page;
    }

};
UINT DataPage::MaxFreeSpaceSize()
{
    UINT free_space = ContentAreaStart();
    if(free_space == 0)
        free_space = disk_page->Size();
    free_space -= HEADER_SIZE + DataCount() * sizeof(USHORT);
    return free_space;
}
USHORT DataPage::PageType() {
    USHORT page_id;
    disk_page->Read(&page_id, sizeof(USHORT), 0);
    return page_id;
}

UINT DataPage::NextPageID() {
    UINT next_page_id;
    disk_page->Read(&next_page_id, sizeof(UINT), 2);
    return next_page_id;
}

USHORT DataPage::ContentAreaStart() {
    USHORT result;
    disk_page->Read(&result, sizeof(USHORT), 6);
    return result;
}

USHORT DataPage::FragmentOffset() {
    USHORT result;
    disk_page->Read(&result, sizeof(USHORT), 8);
    return result;
}

USHORT DataPage::DataCount() {
    USHORT result;
    disk_page->Read(&result, sizeof(USHORT), 10);
    return result;
}


VOID DataPage::PageType(USHORT page_type) {
    disk_page->Write(&page_type, sizeof(page_type), 0);
}

VOID DataPage::NextPageID(UINT next_page_id) {
    disk_page->Write(&next_page_id, sizeof(next_page_id), 2);
}

VOID DataPage::ContentAreaStart(USHORT content_area_start) {
    disk_page->Write(&content_area_start, sizeof(content_area_start), 6);
}

VOID DataPage::FragmentOffset(USHORT fragment_offset) {
    disk_page->Write(&fragment_offset, sizeof(fragment_offset), 8);
}

VOID DataPage::DataCount(USHORT data_count) {
    disk_page->Write(&data_count, sizeof(data_count), 10);
}

#endif //MINIDB_DATAPAGE_H
