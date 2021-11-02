//
// Created by user on 2021/10/22.
//

#ifndef MINIDB_HASHTABLE_H
#define MINIDB_HASHTABLE_H
#include <vector>


template <typename TKey,TValue>
struct HashNode{
    TKey key;
    TValue value;
    struct HashNode<TKey,TValue> *left_node;
    struct HashNode<TKey,TValue> *right_node;

    HashNode(TKey key,TValue value)
    {
        this->key = key;
        this->value = value;
        left_node = nullptr;
        right_node = nullptr;
    }
};

template<typename TKey,TValue>
class HashTable{
private:
    const static double LOAD_FACTOR = 0.75;

    std::vector<HashNode<TKey,TValue>> & array;



public:
    HashTable()
    {
        array = new std::vector<HashNode<TKey,TValue>>(16);
    }



};










#endif //MINIDB_HASHTABLE_H
