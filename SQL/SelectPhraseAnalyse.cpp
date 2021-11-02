//
// Created by user on 2021/11/1.
//


#include "Paser.h"


SelectInfo *Paser::SelectPhraseAnalyse(LexicalTable *table) const {
    SelectInfo *info = new SelectInfo();
    UINT index = 0;
    if (!StringCompare(table, index, "SELECT"))
        goto error_handle;
    index++;
    if (StringCompare(table, index, "*")) {
        info->AddColName(table->word[index]);
        index++;
    } else {
        index--;
        do{
            index++;
            if(!WordTypeCompare(table,index,IS_VAR_NAME))
                goto error_handle;
            info->AddColName(table -> word[index]);
            index++;
        }while(StringCompare(table, index, ","));
    }
    if(!StringCompare(table,index,"FROM"))
        goto error_handle;
    index++;
    if(!WordTypeCompare(table,index,IS_VAR_NAME))
        goto error_handle;
    info->table_name = table-> word[index];
    index++;
    if(StringCompare(table,index,"WHERE"))
    {
        std::string limit;
        do {
            index++;
            if(!(WordTypeCompare(table,index,IS_INT) || WordTypeCompare(table,index,IS_DOUBLE) || WordTypeCompare(table,index,IS_VAR_NAME)))
                goto error_handle;
            limit = table -> word[index];
            index++;
            if(!(StringCompare(table,index,"=") || StringCompare(table,index,"<")||StringCompare(table,index,">")
            || StringCompare(table, index, ">=") || StringCompare(table, index, "<=")))
                goto error_handle;
            limit += table -> word[index];
            index++;
            if(!(WordTypeCompare(table,index,IS_INT) || WordTypeCompare(table,index,IS_DOUBLE) || WordTypeCompare(table,index,IS_VAR_NAME)))
                goto error_handle;
            limit += table -> word[index];
            index++;
            info->AddLimit(limit);
        }while(StringCompare(table, index, ","));
    }
    if(!StringCompare(table,index,";"))
        goto error_handle;
    if (info->table_name.empty() || info->limit_num == 0 && info->col_name_num == 0) {
        error_handle:
        printf("current phrase is not select\n");
        delete info;
        return nullptr;
    }
    return info;
}
