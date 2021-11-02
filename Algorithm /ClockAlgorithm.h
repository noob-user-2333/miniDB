//
// Created by user on 2021/10/29.
//

#ifndef MINIDB_CLOCKALGORITHM_H
#define MINIDB_CLOCKALGORITHM_H
#include <assert.h>
#include <list>
template <typename T>
class ClockAlgorithm{
private:
    const INT capacity;
    const INT (*delete_notify)(T);
    std::list<T> list = new std::list<T>();
    std::list<T>::iterator it = list.begin();
public:
    ClockAlgorithm(INT capacity,INT (*delete_notify)(T))
    {
        this->capacity = capacity;
        this->delete_notify = delete_notify;
        assert(capacity > 0);
        assert(delete_func);
    }













};






#endif //MINIDB_CLOCKALGORITHM_H
