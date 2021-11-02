//
// Created by user on 2021/10/31.
//

#ifndef MINIDB_PASER_H
#define MINIDB_PASER_H

#include <string>
#include <cstring>
#include <stdio.h>
#include "../defs.h"

enum WordTypeCode {
    IS_VAR_NAME = 0,
    IS_SYMBOL = 1,
    IS_INT = 2,
    IS_DOUBLE = 3,
    IS_STRING = 4,
    IS_KEY_WORD
};


struct LexicalTable {
    std::string SQL;
    UINT current_word_num;
    UCHAR word_type_code[MAX_WORD_SIZE];
    std::string word[MAX_WORD_SIZE];

    LexicalTable() {
        current_word_num = 0;
    }


    INT AddWord(UCHAR type_code, CHAR *buffer, UINT length) {
        if (current_word_num >= MAX_WORD_SIZE)
            return DB_NO_FREE_SPACE;
        word_type_code[current_word_num] = type_code;
        for (; length; length--, buffer++)
            word[current_word_num] += *buffer;
        current_word_num++;
        return DB_SUCCESS;
    }

};

struct TableInfo {
    UINT col_num;

    std::string SQL;
    std::string table_name;

    std::string col_name[MAX_FIELD_SIZE];
    ULONG type_code[MAX_FIELD_SIZE];
    UINT root_page_id;


    TableInfo() {
        table_name.clear();
        col_num = 0;
        root_page_id = 0;
    }


    INT AddField(std::string &filed_name, ULONG type_code) {
        if (col_num >= MAX_FIELD_SIZE)
            return DB_NO_FREE_SPACE;
        col_name[col_num] = filed_name;
        this->type_code[col_num] = type_code;
        col_num++;
        return DB_SUCCESS;
    }

};

struct SelectInfo {
    UINT col_name_num;
    UINT limit_num;

    std::string table_name;
    std::string col_name[MAX_FIELD_SIZE];
    std::string limit[MAX_FIELD_SIZE];

    SelectInfo() {
        table_name.clear();
        col_name_num = 0;
        limit_num = 0;
    }


    INT AddColName(std::string &col_name) {
        if (col_name_num >= MAX_FIELD_SIZE)
            return DB_NO_FREE_SPACE;
        this->col_name[col_name_num] = col_name;
        col_name_num++;
        return DB_SUCCESS;
    }

    INT AddLimit(std::string &limit) {
        if (limit_num >= MAX_FIELD_SIZE)
            return DB_NO_FREE_SPACE;
        this->limit[limit_num] = limit;
        limit_num++;
        return DB_SUCCESS;
    }
};


struct InsertInfo {
    UINT col_size;
    UINT value_size;
    std::string table_name;
    std::string col_name[MAX_FIELD_SIZE];
    UCHAR value_type[MAX_FIELD_SIZE];
    std::string value[MAX_FIELD_SIZE];

    InsertInfo() {
        col_size = 0;
        value_size = 0;
        table_name.clear();
    }

    INT AddColName(std::string &col_name) {
        if (col_size >= MAX_FIELD_SIZE)
            return DB_NO_FREE_SPACE;
        this->col_name[col_size] = col_name;
        col_size++;
        return DB_SUCCESS;
    }

    INT AddValue(UCHAR type_code, std::string &value) {
        if (value_size >= MAX_FIELD_SIZE)
            return DB_NO_FREE_SPACE;
        this->value[value_size] = value;
        this->value_type[value_size] = type_code;
        value_size++;
        return DB_SUCCESS;
    }
};


class Paser {
private:
    UINT max_field_num;
    UINT max_word_num;

    static bool StringCompare(LexicalTable *table, UINT index, CHAR *str);

    static bool WordTypeCompare(LexicalTable *table, UINT index, UCHAR word_type_code);

public:
    Paser(UINT max_word_num = MAX_WORD_SIZE, UINT max_field_num = MAX_FIELD_SIZE) {
        this->max_field_num = max_field_num;
        this->max_word_num = max_word_num;
    }

    LexicalTable *Tokenizer(CHAR *grammar);

    TableInfo *CreatePhraseAnalyse(LexicalTable *table) const;

    SelectInfo *SelectPhraseAnalyse(LexicalTable *table) const;

    InsertInfo*InsertPhraseAnalyse(LexicalTable *table) const;
};

#endif //MINIDB_PASER_H
