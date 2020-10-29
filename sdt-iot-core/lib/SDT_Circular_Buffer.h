#ifndef __SDT_CIRCULAR_BUFFER_H__
#define __SDT_CIRCULAR_BUFFER_H__

#include <mbed.h>
#include "platform/mbed_critical.h"
// #include <stdio.h>
// #include <string.h>

typedef enum
{
    SDT_CIRCULAR_SUCCESS = 0,
    SDT_CIRCULAR_PUSH_NOT_ENOUGH_SPACE = -1,
    SDT_CIRCULAR_PUSH_NOT_ENOUGH_MEMORY = -2,
    SDT_CIRCULAR_POP_NO_DATA = -3,
    SDT_CIRCULAR_POP_INVALID_PARAMETER = -4
} SDT_Circular_State;

template <class T>
class SDT_Circular_Buffer
{
private:
    T *_buffer;
    unsigned int _capacity;
    unsigned int _head;
    unsigned int _tail;
    unsigned int _used;
    bool _auto_resize;
    size_t _size;
    Mutex _mutex;

public:
    SDT_Circular_Buffer(unsigned int capacity, bool auto_resize = false)
        : _buffer(NULL),
          _capacity(capacity),
          _head(0),
          _tail(0),
          _used(0),
          _auto_resize(auto_resize)
    {
        _size = sizeof(T);
        _buffer = new T[capacity];
        memset(_buffer, 0, _size * _capacity);
    }

    ~SDT_Circular_Buffer()
    {
        if (_buffer != NULL)
        {
            delete _buffer;
            _buffer = NULL;
        }
    }

    T *get_buffer()
    {
        _mutex.lock();
        T *temp = _buffer;
        _mutex.unlock();
        return temp;
    }

    unsigned int get_remain()
    {
        _mutex.lock();
        unsigned int remain = _capacity - _used;
        _mutex.unlock();
        return remain;
    }

    unsigned int get_capacity()
    {
        _mutex.lock();
        unsigned int capacity = _capacity;
        _mutex.unlock();
        return capacity;
    }

    unsigned int get_used()
    {
        _mutex.lock();
        unsigned int used = _used;
        _mutex.unlock();
        return used;
    }

    void reset()
    {
        _mutex.lock();
        _head = 0;
        _tail = 0;
        _used = 0;
        memset(_buffer, 0, _size * _capacity);
        _mutex.unlock();
    }

    bool resize(unsigned int capacity)
    {
        bool rc = true;

        _mutex.lock();
        if (_head < _tail)
        {
            _tail -= _head;
            //const unsigned int before_size = _tail - _head;
            T *temp = new T[_tail];
            memcpy(temp, _buffer + _head, _size * _tail);

            delete _buffer;
            _capacity = capacity;
            _head = 0;
            _buffer = new T[_capacity];
            memset(_buffer, 0, _size * _capacity);
            memcpy(_buffer, temp, _size * _tail);
        }
        else if (_head >= _tail)
        {
            const unsigned int head_temp_size = _capacity - _head;
            T *head_temp = new T[head_temp_size];
            memcpy(head_temp, _buffer + _head, _size * head_temp_size);

            T *tail_temp = new T[_tail];
            memcpy(tail_temp, _buffer, _size * _tail);

            delete _buffer;
            _capacity = capacity;
            _head = 0;
            _buffer = new T[_capacity];
            memset(_buffer, 0, _size * _capacity);

            memcpy(_buffer, head_temp, _size * head_temp_size);
            memcpy(_buffer + head_temp_size, tail_temp, _size * _tail);
            _tail += head_temp_size;
        }
        else
        {
            rc = false;
        }
        _mutex.unlock();
        return rc;
    }

    int push(T src[], unsigned int src_offset, unsigned int count)
    {
        unsigned int remain = get_remain();
        if (count > remain)
        {
            if (_auto_resize)
            {
                unsigned int resize_size = _capacity + _capacity;
                if (!resize(resize_size))
                {
                    return SDT_CIRCULAR_PUSH_NOT_ENOUGH_MEMORY;
                }
            }
            else
            {
                return SDT_CIRCULAR_PUSH_NOT_ENOUGH_SPACE;
            }
        }

        _mutex.lock();
        unsigned int max_index = _tail + count;
        unsigned int second_loop = max_index > _capacity ? max_index - _capacity : 0;
        unsigned int first_loop = count - second_loop;

        memcpy(_buffer + _tail, src + src_offset, _size * first_loop);
        _tail += first_loop;

        if (second_loop > 0)
        {
            memcpy(_buffer, src + src_offset + first_loop, _size * second_loop);
            _tail = second_loop;
        }

        _used += count;
        _mutex.unlock();
        return SDT_CIRCULAR_SUCCESS;
    }

    int pop(T dst[], unsigned int dst_offset, unsigned int count)
    {
        return pop(dst, _head, dst_offset, count);
    }

    int pop(T dst[], unsigned int src_offset, unsigned int dst_offset, unsigned int count)
    {
        unsigned int real_count = count > _used ? _used : count;
        if (count == 0 || src_offset > _capacity)
        {
            return SDT_CIRCULAR_POP_INVALID_PARAMETER;
        }
        else if (_used < 0)
        {
            return SDT_CIRCULAR_POP_NO_DATA;
        }
        // else if (count > _used)
        // {
        //     return SDT_CIRCULAR_POP_NO_DATA;
        // }

        if (dst == NULL)
        {
            dst = new T[count];
        }

        _mutex.lock();
        memset(dst, dst_offset, _size * count);
        _head = src_offset;

        //unsigned int real_count = count > _used ? _used : count;
        unsigned int max_index = _head + real_count;
        unsigned int second_loop = max_index % _capacity;
        unsigned int first_loop = real_count - second_loop;

        memcpy(dst + dst_offset, _buffer + _head, _size * real_count);
        _head += first_loop;
        if (second_loop > 0)
        {
            memcpy(dst + dst_offset + first_loop, _buffer, _size * second_loop);
            _head = second_loop;
        }

        _used -= real_count;
        _mutex.unlock();
        return first_loop + second_loop;
    }
};

#endif