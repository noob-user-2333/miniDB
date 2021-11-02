#include <iostream>
//#include "StorageEngine/StorageEngine.h"
//#include "SQL/Paser.h"
//#include "DataProcess/DataProcess.h"
#include "miniDB.h"
#define FILE_PATH "/dev/shm/test.miniDB"
#include <stdio.h>
#include <cstdio>
CHAR* file_name = FILE_PATH;
//UCHAR buffer[4096] = {0};
UCHAR buffer2[10240];
miniDB* db = new miniDB(FILE_PATH);
VOID *result[1024];

ULONG origin;
DiskPage *page;
UINT status;
std::string input;
CHAR buffer[4096];
int main() {

    for(UINT times = 0;times <4096;times++)
        buffer[times] = times;



//    db->SQLExecute("CREATE TABLE test(id INT);");
//    db->SQLExecute("CREATE TABLE test1(id INT);");
    while(1)
    {
        printf("input SQL:");
        getline(std::cin,input);
        if(input == "EXIT")
            break;
//        input = "SELECT * FROM test;";//"CREATE TABLE test (id int,name CHAR(510))";
        db->SQLExecute((char*)input.c_str());
    }

//    Paser *pase=new Paser();
//    StorageEngine *storage = new StorageEngine(file_name);
//    DataProcess * process = new DataProcess(storage);
//    LexicalTable * table=pase->Tokenizer("CREATE TABLE table(id INT,name CHAR(50));");
//    TableInfo *info = pase->CreatePhraseAnalyse(table);
//    process->CreateTable(info);

//    table = pase->Tokenizer("INSERT INTO table VALUES (3.1415,'test succeed');");
//    InsertInfo * insert = pase->InsertPhraseAnalyse(table);
//    process->InsertData(insert);
//    DataInfo *data;
//    table = pase->Tokenizer("SELECT * FROM table;");
//    SelectInfo *info = pase->SelectPhraseAnalyse(table);
//    process->QuaryData(info,&data);
//    ULONG length=4096;
//    VOID*result = nullptr;
//    storage->Quary(2,0,&result,&length);

//    TableInfo *info_table;
//    process->QuaryTable("table",&info_table);
//    DataInfo *info_=new DataInfo(result,length,32);
//    for(int times = 0;times <4;times++)
//        storage ->Insert(1,buffer,1024);
//    db->SQLExecute("INSERT INTO table VALUES(255,'it is success');");
//    db->SQLExecute("SELECT * FROM table;");
    delete db;



//    Paser* paser = new Paser(32);
//    InsertInfo *info = paser->InsertPhraseAnalyse(table);
//    for(int times = 0;table && times < table->current_word_num;times++)
//        std::cout << table->word[times] << std::endl;
        return 0;
}
