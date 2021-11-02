//
// Created by user on 2021/10/20.
//

#ifndef MINIDB_DISK_H
#define MINIDB_DISK_H

#include "../defs.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
struct FileInfo{
    long length;
    time_t change_time;
    time_t access_time;
};


class Disk{
private:
    std::string disk_name;
    bool is_open;
    bool is_create;
    int fd;

    FileInfo fileInfo;
    void GetFileInfo()
    {
        struct stat info;
        if (fd > 0 && is_open)
            stat(disk_name.c_str(), &info);
        fileInfo.length =  info.st_size;
        fileInfo.access_time = info.st_atim.tv_sec;
        fileInfo.change_time = info.st_ctim.tv_sec;
    }

public:
    Disk(std::string fileName)
    {
        disk_name = fileName;
        is_open = false;
        is_create = false;
        fd = 0;
    }
    ~Disk()
    {
        Close();
    }
    LONG Length()
    {
        GetFileInfo();
        return fileInfo.length;
    }
    bool IsCreate(){
        return is_create;
    }
    INT Open()
    {
        fd = open(disk_name.c_str(), O_RDWR);
        if(fd <= 0) {
            is_create = true;
            fd = open(disk_name.c_str(),O_CREAT | O_RDWR,0666);
        }
        if(fd <= 0)
            return DB_MEDIA_ERROR;
        is_open = true;
        return DB_SUCCESS;
    }
    INT Read(VOID *buffer,LONG length,LONG offset)
    {
        if(is_open)
        {
            if(length + offset > Length() || buffer == nullptr)
                return DB_INVALID_PARA;
            LONG size = 0;
            lseek(fd,offset,SEEK_SET);
            size = read(fd,buffer,length);
            if(size != length)
                return DB_IO_ERROR;
            return DB_SUCCESS;
        }
        return DB_NOT_OPEN;
    }
    INT Write(VOID *buffer,LONG length,LONG offset)
    {
        if(is_open)
        {
            if(offset > Length() || buffer == nullptr)
                return DB_INVALID_PARA;
            LONG size = 0;
            lseek(fd,offset,SEEK_SET);
            size = write(fd,buffer,length);
            if(size != length)
                return  DB_IO_ERROR;
            return DB_SUCCESS;
        }
        return DB_NOT_OPEN;
    }
    void Close()
    {
        if(fd > 0 && is_open)
            close(fd);
        is_open = false;
    }
};






#endif //MINIDB_DISK_H
