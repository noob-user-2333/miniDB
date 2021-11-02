//
// Created by user on 2021/11/1.
//


#include "Paser.h"

bool IsInt(CHAR *buffer, ULONG &index, LexicalTable *table) {
    ULONG inner_index = index;
    ULONG length = 0;
    if (buffer[inner_index] < '0' || buffer[inner_index] > '9')
        return false;

    length++;
    inner_index++;
    for (; buffer[inner_index]; inner_index++, length++) {
        if (buffer[inner_index] == '.')
            return false;
        if (buffer[inner_index] >= 'a' && buffer[inner_index] <= 'z')
            throw std::exception();
        if (buffer[inner_index] >= 'A' && buffer[inner_index] <= 'Z')
            throw std::exception();
        if (buffer[inner_index] < '0' || buffer[inner_index] > '9')
            break;
    }
    if (table->AddWord(IS_INT, buffer + index, length) != DB_SUCCESS)
        throw "字段过多，已无法完成词法分析";
    index = inner_index;
    return true;
}

bool IsDouble(CHAR *buffer, ULONG &index, LexicalTable *table) {
    ULONG inner_index = index;
    ULONG length = 0;
    bool have_dot = false;
    if (buffer[inner_index] < '0' || buffer[inner_index] > '9')
        return false;
    length++;
    inner_index++;

    for (; buffer[inner_index]; inner_index++, length++) {
        if (buffer[inner_index] == '.') {
            if (have_dot)
                return false;
            have_dot = true;
            if (buffer[inner_index + 1] < '0' || buffer[inner_index + 1] > '9')
                return false;
            continue;
        }
        if (buffer[inner_index] >= 'a' && buffer[inner_index] <= 'z')
            return false;
        if (buffer[inner_index] >= 'A' && buffer[inner_index] <= 'Z')
            return false;
        if (buffer[inner_index] < '0' || buffer[inner_index] > '9')
            break;
    }
    if (have_dot) {
        if (table->AddWord(IS_DOUBLE, buffer + index, length) != DB_SUCCESS)
            throw "字段过多，已无法完成词法分析";
        index = inner_index;
    }
    return have_dot;
}

bool IsString(CHAR *buffer, ULONG &index, LexicalTable *table) {
    ULONG inner_index = index;
    ULONG length = 0;
    if (buffer[inner_index] != '\'')
        return false;
    inner_index++;
    length++;
    for (; buffer[inner_index]; inner_index++, length++) {
        if (buffer[inner_index] == '\'') {
            length++;
            inner_index++;
            if (table->AddWord(IS_STRING, buffer + index, length) != DB_SUCCESS)
                throw "字段过多，已无法完成词法分析";
            index = inner_index;
            return true;
        }
    }
    return false;
}

bool IsSymbol(CHAR *buffer, ULONG &index, LexicalTable *table) {
    ULONG inner_index = index;
    ULONG length = 0;
    if (buffer[inner_index] == '(' || buffer[inner_index] == ')') {
        symbol_is:
        inner_index++;
        length++;
        if (table->AddWord(IS_SYMBOL, buffer + index, length) != DB_SUCCESS)
            throw "字段过多，已无法完成词法分析";
        index = inner_index;
        return true;
    }

    if (buffer[inner_index] == ';')
        goto symbol_is;
    if (buffer[inner_index] == ',')
        goto symbol_is;
    if (buffer[inner_index] == '=')
        goto symbol_is;
    if(buffer[inner_index] == '*')
        goto symbol_is;

    if (buffer[inner_index] == '>' || buffer[inner_index] == '<') {
        if (buffer[inner_index + 1] == '=') {
            inner_index++;
            length++;
        }
        goto symbol_is;
    }

    return false;
}

bool IsVarName(CHAR *buffer, ULONG &index, LexicalTable *table) {
    ULONG inner_index = index;
    ULONG length = 0;

    if (buffer[inner_index] != '_') {
        if (buffer[inner_index] < 'A' || buffer[inner_index] > 'z')
            return false;
        if (buffer[inner_index] > 'Z' && buffer[inner_index] < 'a')
            return false;
    }
    inner_index++;
    length++;


    for (; buffer[inner_index]; inner_index++, length++) {
        if (buffer[inner_index] != '_') {
            if (buffer[inner_index] >= '0' && buffer[inner_index] <= '9')
                continue;
            if (buffer[inner_index] >= 'a' && buffer[inner_index] <= 'z')
                continue;
            if (buffer[inner_index] >= 'A' && buffer[inner_index] <= 'Z')
                continue;
            break;
        }
    }


    if (table->AddWord(IS_VAR_NAME, buffer + index, length) != DB_SUCCESS)
        throw "字段过多，已无法完成词法分析";
    index = inner_index;
    bool is_key_word = false;
    std::string *current_str = &table->word[table->current_word_num - 1];

    if (*current_str == "INSERT")
        is_key_word = true;

    if (*current_str == "INTO")
        is_key_word = true;

    if (*current_str == "WHERE")
        is_key_word = true;

    if (*current_str == "CREATE")
        is_key_word = true;

    if (*current_str == "TABLE")
        is_key_word = true;

    if (*current_str == "SELECT")
        is_key_word = true;

    if (*current_str == "FROM")
        is_key_word = true;

    if (*current_str == "INT")
        is_key_word = true;

    if (*current_str == "CHAR")
        is_key_word = true;

    if (*current_str == "DOUBLE")
        is_key_word = true;

    if (*current_str == "TEXT")
        is_key_word = true;

//    if (*current_str == "INSERT")
//        is_key_word = true;
//
    if (is_key_word)
        table->word_type_code[table->current_word_num - 1] = IS_KEY_WORD;
    return true;
}

LexicalTable *Paser::Tokenizer(CHAR *grammar) {
    LexicalTable *result = new LexicalTable();
    ULONG length = strlen(grammar);
    for (ULONG index = 0; index < length;) {
        if (grammar[index] == ' ') {
            index++;
            continue;
        }
        if (IsSymbol(grammar, index, result))
            continue;
        if (IsVarName(grammar, index, result))
            continue;
        if (IsDouble(grammar, index, result))
            continue;
        try {
            if (IsInt(grammar, index, result))
                continue;
        }
        catch (std::exception ex) {
            printf("lexical error happen in:%s\n", grammar + index);
            delete result;
            return nullptr;
        }
        if (IsString(grammar, index, result))
            continue;

        printf("lexical error happen in:%s\n", grammar + index);
        delete result;
        return nullptr;
    }
    result->SQL = grammar;
    return result;
}
