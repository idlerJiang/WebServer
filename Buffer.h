//
// Created by Fisher on 2024/3/1.
//

#ifndef WEBSERVER_BUFFER_H
#define WEBSERVER_BUFFER_H


#include <cstring>
#include <iostream>
#include <vector>
#include <atomic>
#include <cassert>

class Buffer {
public:
    Buffer();
    Buffer(int);
    size_t writeable_bytes() const;
    size_t readable_bytes() const;
    size_t previous_bytes() const;

    void ensure_writable(int);

    const char* peek() const;
    void move_write_position(size_t);
    void move_read_position(size_t);
    void move_read_position_till(const char*);
    void move_read_position_to_end();

    const char* get_write_position() const;

    void write(const char*, size_t);
    void write(const std::string &);

private:
    char * get_begin_pointer();
    const char * get_begin_pointer() const;
    void extend(size_t);

    std::vector<char> buffer;
    std::atomic<std::size_t> read_position;
    std::atomic<std::size_t> write_position;
};


#endif //WEBSERVER_BUFFER_H
