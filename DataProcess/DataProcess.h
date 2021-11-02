//
// Created by user on 2021/11/1.
//

#ifndef MINIDB_DATAPROCESS_H
#define MINIDB_DATAPROCESS_H

#include "../StorageEngine/StorageEngine.h"
#include "../SQL/Paser.h"

struct DataInfo {
    UCHAR *start_ptr;
    ULONG length;
    UINT current_field_num;

    ULONG type_code[MAX_FIELD_SIZE];
    VOID *data_ptr[MAX_FIELD_SIZE];

    DataInfo(VOID *buffer, ULONG length) {
        this->length = length;
        start_ptr = (UCHAR *) buffer;
        current_field_num = 0;
        Format();
    }

    ~DataInfo() {
        delete start_ptr;
    }

    VOID Format() {
        UCHAR *buffer = start_ptr;
        ULONG header_length;
        buffer += PublicFunction::ReadVarInt(buffer, &header_length);
        for (UINT read_header_bytes = 0; read_header_bytes < header_length;) {
            read_header_bytes += PublicFunction::ReadVarInt(buffer + read_header_bytes, type_code + current_field_num);
            current_field_num++;
        }
        buffer += header_length;
        for (UINT index = 0; index < current_field_num; index++) {
            data_ptr[index] = buffer;
            switch (type_code[index]) {
                case 0: {
                    data_ptr[index] = nullptr;
                    break;
                }
                case 1: {
                    buffer += 1;
                    break;
                }
                case 2: {
                    buffer += 2;
                    break;
                }
                case 3: {
                    buffer += 4;
                    break;
                }
                case 4:
                case 5: {
                    buffer += 8;
                    break;
                }
                default: {
                    if (type_code[index] & 0x01) {
                        if (type_code[index] >= 13)
                            buffer += (type_code[index] - 13) / 2;
                    } else {
                        if (type_code[index] >= 12)
                            buffer += (type_code[index] - 12) / 2;
                    }
                }
            }
        }
    }
};


struct DataWrite {
    UINT current_field_num;

    VOID *write_ptr;
    ULONG type_code[MAX_FIELD_SIZE];
    VOID *source_ptr[MAX_FIELD_SIZE];

    DataWrite() {
        current_field_num = 0;
        write_ptr = nullptr;
    }


    UINT AddData(ULONG type_code, VOID *buffer) {
        if (current_field_num >= MAX_FIELD_SIZE)
            return DB_NO_FREE_SPACE;
        this->type_code[current_field_num] = type_code;
        this->source_ptr[current_field_num] = buffer;
        current_field_num++;
        return DB_SUCCESS;
    }

    VOID Clear() {
        for (UINT index = 0; index < MAX_FIELD_SIZE; index++) {
            type_code[index] = 0;
            source_ptr[index] = nullptr;
        }
        current_field_num = 0;
        write_ptr = nullptr;
    }

    ULONG WriteToBuffer(VOID *dest_ptr = nullptr) {
        if (dest_ptr == nullptr && write_ptr == nullptr)
            return DB_INVALID_PARA;
        if (dest_ptr)
            write_ptr = dest_ptr;
        UCHAR *buffer = (UCHAR *) write_ptr;
        ULONG header_length = 0;
        for (UINT times = 0; times < current_field_num; times++)
            header_length += PublicFunction::WriteVarInt(buffer, type_code[times]);
        buffer += PublicFunction::WriteVarInt(buffer, header_length);
        for (UINT times = 0; times < current_field_num; times++)
            buffer += PublicFunction::WriteVarInt(buffer, type_code[times]);
        ULONG bytes = 0;
        for (UINT index = 0; index < current_field_num; index++) {
            bytes = 0;
            switch (type_code[index]) {
                case 1: {
                    bytes += 1;
                    break;
                }
                case 2: {
                    bytes += 2;
                    break;
                }
                case 3: {
                    bytes += 4;
                    break;
                }
                case 4:
                case 5: {
                    bytes += 8;
                    break;
                }
                default: {
                    if (type_code[index] & 0x01) {
                        if (type_code[index] >= 13)
                            bytes += (type_code[index] - 13) / 2;
                    } else {
                        if (type_code[index] >= 12)
                            bytes += (type_code[index] - 12) / 2;
                    }
                }
            }
            memcpy(buffer, source_ptr[index], bytes);
            buffer += bytes;
        }
        return (ULONG) buffer - (ULONG) write_ptr;
    }
};

struct SelectCache {
    SelectInfo *info;
    TableInfo *table;
    UINT current_page_id;
    UINT index;
    UINT col_num;
    UINT map_col[MAX_FIELD_SIZE];

    SelectCache(SelectInfo *info, TableInfo *table) {
        this->info = info;
        this->table = table;
        current_page_id = table->root_page_id;
        index = 0;
        col_num = info->col_name_num;
        if (info->col_name_num == 1 && info->col_name[0] == "*") {
            col_num = table->col_num;
            for (UINT _index = 0; _index < col_num; _index++)
                map_col[_index] = _index;
        } else {
            for (UINT _index = 0; _index < col_num; _index++) {
                for (UINT inner_index = 0; inner_index < table->col_num; inner_index++) {
                    if (info->col_name[_index] == table->col_name[inner_index]) {
                        map_col[_index] = inner_index;
                        break;
                    }
                    if(table->col_num == inner_index)
                        throw "filed name error ,check it";
                }
            }
        }
    }


};

static UCHAR data_precess_buffer[4096 * 1024];

class DataProcess {
private:
    const static UINT max_table_cache = 10;
    IDataAccess *access;
    UCHAR *buffer = data_precess_buffer;

    std::list<std::string> sub_str;
    std::list<SelectCache *> cache;

    DataWrite writer;
    DataInfo *reader = nullptr;
    Paser paser;

public:
    DataProcess(IDataAccess *access) {
        assert(access);
        this->access = access;
    }

    DataInfo *DataFormat(VOID *buffer, UINT length) {
        return new DataInfo(buffer, length);
    }

    /*
     * master (
     *  name TEXT
     *  type TEXT
     *  SQL  TEXT
     *  next_page_ID INT
     * )
     */
    UINT CreateTable(TableInfo *info) {
        UINT page_id = 0;
        TableInfo *info_temp;
        if (QuaryTable(info->table_name, &info_temp) == DB_SUCCESS){
            delete info_temp;
            printf("the table of %s is exists\n", info->table_name.c_str());
            return DB_EXISTS;
        }

        INT status = access->AllocDataPage(&page_id);
        if (status != DB_SUCCESS)
            return status;
        writer.Clear();
        writer.AddData(info->table_name.size() * 2 + 12, (VOID *) info->table_name.c_str());
        writer.AddData(22, (VOID *) "TABLE");
        writer.AddData(info->SQL.size() * 2 + 12, (VOID *) info->SQL.c_str());
        writer.AddData(3, &page_id);
        ULONG length = writer.WriteToBuffer(buffer);
        return access->Insert(1, buffer, length);
//        return DB_SUCCESS;
    }

    UINT QuaryTable(std::string table_name, TableInfo **out_info) {
        UINT next_page_id = 0;
        UINT current_page_id = 1;
        UINT index = 0;
        VOID *buffer;
        ULONG length;
        ULONG name_length;
        do {
            index = 0;
            next_page_id = 0;
            while (access->Quary(current_page_id, index, &buffer, &length, &next_page_id) == DB_SUCCESS) {
                index++;
                if (reader != nullptr)
                    delete reader;
                reader = new DataInfo(buffer, length);
                name_length = (reader->type_code[0] - 12) / 2;
                if (name_length == table_name.length()) {
                    UINT times;
                    for (times = 0; times < name_length; times++) {
                        if (((UCHAR *) reader->data_ptr[0])[times] != table_name[times])
                            break;
                    }

                    if (times == name_length) {
                        memcpy(buffer, reader->data_ptr[2], (reader->type_code[2] - 12) / 2);
                        ((CHAR *) buffer)[(reader->type_code[2] - 12) / 2] = 0;
//                        std::string* sql = new std::string((CHAR*)buffer);
                        LexicalTable *table = paser.Tokenizer((CHAR *) buffer);
                        *out_info = paser.CreatePhraseAnalyse(table);
                        memcpy(&((*out_info)->root_page_id), reader->data_ptr[3], sizeof(UINT));
                        delete table;
//                        delete sql;
                        return DB_SUCCESS;
                    }
                }
            }
            current_page_id = next_page_id;
        } while (next_page_id);

        return DB_NOT_FOUND;
    }

    UINT InsertData(InsertInfo *info) {
        if (info == nullptr)
            return DB_INVALID_PARA;
        TableInfo *table;
        UINT status = QuaryTable(info->table_name, &table);
        if (status != DB_SUCCESS)
            return status;
        writer.Clear();
        ULONG long_buffer[32] = {0};
        UINT long_buffer_use_size = 0;
        double double_buffer[32] = {0};
        UINT double_buffer_use_size = 0;
        ULONG type_code = 0;
        sub_str.clear();
        bool flag = false;
        if (info->col_size) {
            for (UINT index = 0; index < info->col_size; index++) {
                UINT inner_index;
                flag = false;
                for (inner_index = 0; inner_index < table->col_num; inner_index++) {
                    if (info->col_name[index] == table->col_name[inner_index]) {
                        if (writer.type_code[inner_index]) {
                            printf("错误的插入语句！！\n");
                            delete table;
                            return DB_INVALID_PARA;
                        }
                        switch (info->value_type[index]) {
                            case IS_INT: {
                                if (table->type_code[inner_index] != 4)
                                    break;
                                long_buffer[long_buffer_use_size] = std::stol(info->value[index]);
//                                if ((long_buffer[long_buffer_use_size] >> (sizeof(UCHAR) * 8)) == 0)
//                                    type_code = 1;
//                                else if (long_buffer[long_buffer_use_size] >> (sizeof(USHORT) * 8) == 0)
//                                    type_code = 2;
//                                else if (long_buffer[long_buffer_use_size] >> (sizeof(UINT) * 8) == 0)
//                                    type_code = 3;
//                                else
                                type_code = 4;


                                writer.type_code[inner_index] = type_code;
                                writer.source_ptr[inner_index] = long_buffer + long_buffer_use_size;
                                long_buffer_use_size++;
                                flag = true;
                                break;
                            }
                            case IS_DOUBLE: {
                                if (table->type_code[inner_index] != 5)
                                    break;
                                double_buffer[double_buffer_use_size] = atof(info->value[index].c_str());
                                type_code = 5;
                                writer.type_code[inner_index] = type_code;
                                writer.source_ptr[inner_index] = double_buffer + double_buffer_use_size;
                                double_buffer_use_size++;
                                flag = true;
                                break;
                            }
                            case IS_STRING: {
                                sub_str.push_front(info->value[index].substr(1, info->value[index].size() - 2));
                                type_code = sub_str.front().size() * 2 + 12;
                                if (type_code > table->type_code[inner_index])
                                    break;
                                writer.type_code[inner_index] = type_code;
                                writer.source_ptr[inner_index] = (VOID *) sub_str.front().c_str();
                                flag = true;
                            }
                        }

                        if (flag)
                            break;
                    }
                }
                if (inner_index == table->col_num) {
                    printf("当前插入数据与表(%s)的格式不符!!!\n", table->table_name.c_str());
                    delete table;
                    return DB_INVALID_PARA;
                }
            }
        } else {
            if (info->value_size != table->col_num) {
                type_error:
                printf("当前插入数据与表(%s)的格式不符!!!\n", table->table_name.c_str());
                delete table;
                return DB_INVALID_PARA;
            }
            for (UINT index = 0; index < info->value_size; index++) {
                switch (info->value_type[index]) {
                    case IS_INT: {
                        if (table->type_code[index] != 4)
                            goto type_error;
                        long_buffer[long_buffer_use_size] = std::stol(info->value[index]);
                        if ((long_buffer[long_buffer_use_size] >> (sizeof(UCHAR) * 8)) == 0)
                            type_code = 1;
                        else if (long_buffer[long_buffer_use_size] >> (sizeof(USHORT) * 8) == 0)
                            type_code = 2;
                        else if (long_buffer[long_buffer_use_size] >> (sizeof(UINT) * 8) == 0)
                            type_code = 3;
                        else
                            type_code = 4;

                        writer.AddData(type_code, long_buffer + long_buffer_use_size);
                        long_buffer_use_size++;
                        break;
                    }
                    case IS_DOUBLE: {
                        if (table->type_code[index] != 5)
                            goto type_error;
                        double_buffer[double_buffer_use_size] =  atof(info->value[index].c_str());
                        type_code = 5;
                        writer.AddData(type_code, double_buffer + double_buffer_use_size);
                        double_buffer_use_size++;
                        break;
                    }
                    case IS_STRING: {
                        sub_str.push_front(info->value[index].substr(1, info->value[index].size() - 2));
                        type_code = sub_str.front().size() * 2 + 12;
                        if (type_code > table->type_code[index])
                            goto type_error;
                        writer.AddData(type_code, (VOID *) sub_str.front().c_str());
                        break;
                    }

                }
            }
        }
        writer.current_field_num = table->col_num;
        ULONG length = writer.WriteToBuffer(buffer);
        status = access->Insert(table->root_page_id, buffer, length);
        delete table;
        return status;
    }

    UINT QuaryData(SelectInfo *info, DataInfo **out_data) {
        if (info == nullptr)
            return DB_INVALID_PARA;
        SelectCache *cache_info = nullptr;
        for (auto it = cache.begin(); it != cache.end(); it++)
            if ((*it)->info == info)
                cache_info = *it;

        if (cache_info == nullptr) {
            TableInfo *table;
            UINT status = QuaryTable(info->table_name, &table);
            if (status != DB_SUCCESS) {
                return status;
            }
            try {
                cache_info = new SelectCache(info, table);
            }
            catch (const char *string)
            {
                printf("%s\n", string);
                return DB_INVALID_PARA;
            }
            cache.push_front(cache_info);
        }
        UINT status;
        VOID *result;
        ULONG length;
        UINT next_page_id;
        status = access->Quary(cache_info->current_page_id, cache_info->index, &result, &length, &next_page_id);
        if (status != DB_SUCCESS) {
            if (next_page_id == 0) {
                cache.remove(cache_info);
                return DB_NO_MORE_CONTENT;
            }
            cache_info->current_page_id = next_page_id;
            status = access->Quary(cache_info->current_page_id, cache_info->index, &result, &length, &next_page_id);
            if (status != DB_SUCCESS)
                return status;
        }
        cache_info->index++;
        *out_data = new DataInfo(result, length);
        //对data进行处理以满足select的要求
        VOID *data_ptr[MAX_FIELD_SIZE];
        ULONG type_code[MAX_FIELD_SIZE];

        for (UINT index = 0; index < (*out_data)->current_field_num; index++) {
            type_code[index] = (*out_data)->type_code[index];
            data_ptr[index] = (*out_data)->data_ptr[index];
        }

        (*out_data)->current_field_num = cache_info->col_num;
        for (UINT index = 0; index < cache_info->col_num; index++) {
            (*out_data)->type_code[index] = type_code[cache_info->map_col[index]];
            (*out_data)->data_ptr[index] = data_ptr[cache_info->map_col[index]];
        }


        return DB_SUCCESS;
    }

};

#endif //MINIDB_DATAPROCESS_H
