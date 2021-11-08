#include <iostream>

#include "miniDB.h"
#define FILE_PATH "/dev/shm/test.miniDB"
#include <stdio.h>
#include <cstdio>
char* file_name = FILE_PATH;
miniDB* db;
std::string input;

int main() {
        db = new miniDB(file_name);
        do{
            std::getline(std::cin,input);
            if(input == "EXIT")
                break;
            db->SQLExecute((CHAR*)input.c_str());
        }while(1);

        return 0;
}
