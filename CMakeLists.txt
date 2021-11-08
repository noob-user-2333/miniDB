cmake_minimum_required(VERSION 3.20)
project(miniDB)

set(CMAKE_CXX_STANDARD 17)

add_executable(miniDB main.cpp StorageEngine/Disk.h DataStruct/HashTable.h defs.h StorageEngine/DiskPage.h StorageEngine/DiskPageManage.h "Algorithm /ClockAlgorithm.h" StorageEngine/StorageEngine.h StorageEngine/DataPage.h StorageEngine/DataPageManage.h PublicFunction.h StorageEngine/DataBaseHeader.h StorageEngine/OverflowPage.h SQL/Paser.h SQL/Paser.cpp SQL/CreatePhraseAnalyse.cpp SQL/SelectPhraseAnalyse.cpp SQL/InsertPhraseAnalyse.cpp DataProcess/DataProcess.h miniDB.h DataStruct/LinkedList.h)
