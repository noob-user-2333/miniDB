//
// Created by user on 2021/11/1.
//

#include "Paser.h"



InsertInfo* Paser::InsertPhraseAnalyse(LexicalTable *table) const
{
    InsertInfo *info= new InsertInfo();
    UINT index = 0;
    if(!StringCompare(table,index,"INSERT"))
        goto error_handle;
    index++;
    if(!StringCompare(table,index,"INTO"))
        goto error_handle;
    index++;

    if(!WordTypeCompare(table,index,IS_VAR_NAME))
        goto error_handle;
    info->table_name  = table->word[index];
    index++;
    if(StringCompare(table,index,"("))
    {
        do{
            index++;
            if(!WordTypeCompare(table,index,IS_VAR_NAME))
                goto error_handle;
            info->AddColName(table->word[index]);
            index++;
        }while(StringCompare(table, index, ","));
        if(!StringCompare(table,index,")"))
            goto error_handle;
        index++;
    }

    if(!StringCompare(table,index,"VALUES"))
        goto error_handle;
    index++;
    if(!StringCompare(table,index,"("))
        goto error_handle;
    do{
        index++;
        if(!(WordTypeCompare(table,index,IS_DOUBLE) || WordTypeCompare(table,index,IS_INT) ||WordTypeCompare(table,index,IS_STRING)) )
            goto error_handle;
        info->AddValue(table->word_type_code[index],table->word[index]);
        index++;
    }while(StringCompare(table, index, ","));

    if(!StringCompare(table,index,")"))
        goto error_handle;
    index++;
    if(!StringCompare(table,index,";"))
        goto error_handle;
    if(info->table_name.empty() || (info->col_size && info->value_size != info->col_size))
    {
        error_handle:
        printf("error happen in insert praser \n");
        delete info;
        return nullptr;
    }
    return info;
}
