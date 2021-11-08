//
// Created by user on 2021/11/3.
//

#ifndef MINIDB_LINKEDLIST_H
#define MINIDB_LINKEDLIST_H

#include <iterator>

template<typename T>
class LinkedList;

template<typename T>
class LinkedListNode {
private:
    LinkedList<T> &owner;
    T value;
public:
    LinkedListNode<T> * next_node;
    LinkedListNode<T> * previous_node;

    LinkedListNode(LinkedList<T> &list, T &value) {
        owner = list;
        this->value = value;
//        this->next_node = next_node;
//        this->previous_node = previous_node;
    }
    LinkedListNode(LinkedList<T> &list, T &value,LinkedListNode<T> *next_node,LinkedListNode<T> *previous_node) {
        owner = list;
        this->value = value;
        this->next_node = next_node;
        this->previous_node = previous_node;
    }

};

template<typename T>
class LinkedList {
private:
    LinkedListNode<T> *head_node;
    unsigned int count;
public:
    LinkedList() {
        head_node = nullptr;
        count = 0;
    }

    void push_front(T value) {
        if (count) {
            LinkedListNode<T> *new_node = new LinkedListNode<T>(*this, value, head_node, head_node->previous_node);
            head_node = new_node;
        } else {
            head_node = new LinkedListNode<T>(*this, value);
            head_node->previous_node = head_node;
            head_node->next_node = head_node;
        }
        count++;
    }


    void AddAfter(LinkedListNode<T> & node, T value) {

    }


};


#endif //MINIDB_LINKEDLIST_H
