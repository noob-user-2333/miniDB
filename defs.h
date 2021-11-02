//
// Created by user on 2021/10/29.
//

#ifndef MINIDB_DEFS_H
#define MINIDB_DEFS_H

#define DB_SUCCESS 0
#define DB_MEDIA_ERROR 1
#define DB_IO_ERROR 2
#define DB_NOT_OPEN 3
#define DB_INVALID_PARA 4
#define DB_ERROR_DATA 5
#define DB_NO_FREE_SPACE 6
#define DB_IS_NULL 7
#define  DB_NOT_FOUND 8
#define DB_EXISTS 9
#define DB_NO_MORE_CONTENT 10




#define DB_UNKNOWN_ERROR -1


#define MAX_FIELD_SIZE 32
#define MAX_WORD_SIZE  512

typedef void VOID;
typedef char CHAR;
typedef short SHORT;
typedef int INT;
typedef long LONG;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
[[maybe_unused]] typedef unsigned long ULONG;




#endif //MINIDB_DEFS_H
