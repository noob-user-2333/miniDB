//
// Created by user on 2021/11/1.
//

#include "Paser.h"
#include <cstdlib>
bool Paser::StringCompare(LexicalTable *table, UINT index, CHAR *str) {
    if (index >= table->current_word_num)
        return false;
    return table->word[index] == str;
}

bool Paser::WordTypeCompare(LexicalTable *table, UINT index, UCHAR word_type_code) {
    if (index >= table->current_word_num)
        return false;
    return table->word_type_code[index] == word_type_code;
}

TableInfo *Paser::CreatePhraseAnalyse(LexicalTable *table) const {
    TableInfo *info = new TableInfo();
    UINT index = 0;
    std::string col_name;
    ULONG type_code;
    std::string *current_word;
    if (!StringCompare(table, index, "CREATE"))
        goto error_handle;
    index++;

    if (!StringCompare(table, index, "TABLE"))
        goto error_handle;
    index++;

    if (!WordTypeCompare(table, index, IS_VAR_NAME))
        goto error_handle;
    info->table_name = table->word[index];
    index++;

    if (!StringCompare(table, index, "("))
        goto error_handle;


    do {
        index++;
        if(!WordTypeCompare(table,index,IS_VAR_NAME))
            goto error_handle;
        col_name = table -> word[index];
        index++;
        if(!WordTypeCompare(table,index,IS_KEY_WORD))
            goto error_handle;
        current_word =&table->word[index];
        if(*current_word == "INT")
            type_code = 4;
        else if (* current_word == "DOUBLE")
            type_code = 5;
        else if(*current_word == "TEXT")
            type_code = (16 * 1024) * 2 + 12;
        else if(*current_word == "CHAR"){
            index++;
            if(!StringCompare(table,index,"("))
                goto error_handle;
            index++;
            if(!WordTypeCompare(table,index,IS_INT))
                goto error_handle;
            //确定具体长度
            type_code = std::stol(table->word[index]) * 2 + 12;
            index++;
            if(!StringCompare(table,index,")"))
                goto error_handle;
        }
        else
            goto error_handle;
        info->AddField(col_name,type_code);
        index++;
    } while (StringCompare(table, index, ",") && index < table->current_word_num);

    if(!StringCompare(table,index,")"))
        goto error_handle;
    index++;
    if (!StringCompare(table, index, ";"))
        goto error_handle;

    if (info->col_num == 0) {
        error_handle:
        delete info;
        printf("current SQL is error!!!!\n");
        return nullptr;
    }
    info->SQL = table->SQL;
    return info;
}
