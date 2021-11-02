//
// Created by user on 2021/10/30.
//

#ifndef MINIDB_PUBLICFUNCTION_H
#define MINIDB_PUBLICFUNCTION_H
#include "defs.h"
class PublicFunction{
public:
    //最高位为0表示为最高位字节
    //最高位为1表示非最高位字节
    static INT ReadVarInt(VOID*buffer,ULONG *dest_ptr)
    {
        ULONG result = 0;
        UCHAR *buff = (UCHAR*)buffer;
        INT times = 0;
        for(;times < 10 && (*buff & 0x80);times++)
        {
            result += (*buff & 0x7F) << (times * 7);
            buff++;
        }
        result += (*buff & 0x7F) << (times * 7);
        *dest_ptr = result;
        times++;
        return times;
    }

    static INT WriteVarInt(VOID *buffer,ULONG number)
    {
        UCHAR *buff = (UCHAR*)buffer;
        INT times = 0;
        for(;times < 10 && number / 0x80;times++)
        {
            *buff = (number & 0x7F) + 0x80;
            number >>= 7;
            buff ++;
        }
        times++;
        *buff = number;
        return times;
    }


};
#endif //MINIDB_PUBLICFUNCTION_H
