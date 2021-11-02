//
// Created by user on 2021/11/2.
//

#ifndef MINIDB_MINIDB_H
#define MINIDB_MINIDB_H
#include "StorageEngine/StorageEngine.h"
#include "SQL/Paser.h"
#include "DataProcess/DataProcess.h"
static UCHAR DB_buffer[4096 * 1024];
class miniDB{
private:
    const char *display_table = ".table";
    StorageEngine *storage;
    Paser *paser;
    DataProcess *process;
    std::string file_path;
public:
    miniDB(std::string file_path)
    {
        this->file_path = file_path;
        storage = new StorageEngine((CHAR*)file_path.c_str());
        process = new DataProcess(storage);
        paser = new Paser();
    }
    ~miniDB()
    {
        delete storage;
        delete paser;
        delete process;
    }

    UINT SQLExecute(CHAR *SQL)
    {
        UINT status;
        LexicalTable *table = paser->Tokenizer(SQL);
        if(table == nullptr)
        {
            printf("lexical error happen!!\n");
            return DB_INVALID_PARA;
        }
        if(table->word[0] == "CREATE")
        {
            TableInfo *info = paser->CreatePhraseAnalyse(table);
            if(info == nullptr) {
                printf("error SQL!!!");
                return DB_INVALID_PARA;
            }
            status = process->CreateTable(info);
            if(status == DB_SUCCESS)
            {
                printf("CREATE TABLE(%s) SUCCEED!!!\n",info->table_name.c_str());
                delete info;
            }
            delete table;
            return status;
        }
        else if(table->word[0] == "INSERT")
        {
            InsertInfo *info = paser->InsertPhraseAnalyse(table);
            if(info == nullptr) {
                printf("error SQL!!!");
                return DB_INVALID_PARA;
            }
            status = process->InsertData(info);
            if(status == DB_SUCCESS)
            {
                printf("INSERT INTO TABLE(%s) SUCCEED\n",info->table_name.c_str());
                delete info;
            }
            delete table;
            if(status != DB_SUCCESS)
                printf("INSERT failed\n");
            return status;
        }
        else if(table->word[0] == "SELECT")
        {
            DataInfo *data;
            SelectInfo *info = paser->SelectPhraseAnalyse(table);
            if(info == nullptr) {
                printf("error phrase!!!");
                return DB_INVALID_PARA;
            }
            bool flag = true;
            do {
                status = process->QuaryData(info, &data);
                if(status != DB_SUCCESS && flag)
                {
                    printf("TABLE(%s) is have not data or not exists\n",info->table_name.c_str());
                    return status;
                }
                flag = false;
                if(status == DB_SUCCESS)
                {
                    for(UINT index = 0;index < data->current_field_num;index++)
                    {
                        if(data->type_code[index] == 0)
                            std::cout << "  (NULL)  ";
                        else if(data->type_code[index] == 1)
                            printf("  %d  ",*((UCHAR*)data->data_ptr[index]));
                        else if(data->type_code[index] == 2)
                            printf("  %d  ",*((USHORT*)data->data_ptr[index]));
                        else if(data->type_code[index] == 3)
                            printf("  %d  ",*((UINT*)data->data_ptr[index]));
                        else if(data->type_code[index] == 4)
                            printf("  %ld  ",*((ULONG*)data->data_ptr[index]));
                        else if(data->type_code[index] == 5)
                            printf("  %lf  ",*((double*)data->data_ptr[index]));
                        else
                        {
                            ULONG length = (data->type_code[index] - 12) / 2;
                            memcpy(DB_buffer,data->data_ptr[index],length);
                            DB_buffer[length] = 0;
                            printf("  %s  ",DB_buffer);
                        }
                    }
                    std::cout <<std::endl;
                    delete data;
                }
            }while(status == DB_SUCCESS);
            delete table;
            return DB_SUCCESS;
        }
        else
        {
            delete table;
            printf("phrase error!!\n");
            return DB_INVALID_PARA;
        }
    }

};
#endif //MINIDB_MINIDB_H
