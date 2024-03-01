//
// Created by Fisher on 2024/3/1.
//


#include "Buffer.h"

Buffer::Buffer() {
    Buffer(1024);
}

Buffer::Buffer(int initial_size) {
    buffer.resize(initial_size);
    read_position = 0;
    write_position = 0;
}

size_t Buffer::writeable_bytes() const {
    return buffer.size() - write_position;
}

size_t Buffer::readable_bytes() const {
    return write_position - read_position;
}

size_t Buffer::previous_bytes() const {
    return read_position;
}

void Buffer::ensure_writable(int request_length) {
    if (writeable_bytes() < request_length)
        extend(request_length);
    assert(writeable_bytes() >= request_length);
}

const char *Buffer::peek() const {
    return get_begin_pointer() + read_position;
}

void Buffer::move_write_position(size_t len) {
    write_position += len;
}

void Buffer::move_read_position(size_t len) {
    assert(len <= readable_bytes());
    read_position += len;
}

void Buffer::move_read_position_till(const char *end_position) {
    assert(peek() <= end_position);
    move_read_position(end_position - peek());
}

void Buffer::move_read_position_to_end() {
    memset(&buffer[0], 0, buffer.size());
    read_position = 0;
    write_position = 0;
}

const char *Buffer::get_write_position() const {
    return get_begin_pointer() + write_position;
}

void Buffer::write(const char *str, size_t len) {
    assert(str);
    ensure_writable(len);
    std::copy(str, str + len, get_begin_pointer());
    move_write_position(len);
}

void Buffer::write(const std::string &str) {
    write(str.data(), str.length());
}

char *Buffer::get_begin_pointer() {
    return &*buffer.begin();
}

const char *Buffer::get_begin_pointer() const {
    return &*buffer.begin();
}

void Buffer::extend(size_t len) {
    if (writeable_bytes() + previous_bytes() < len) {
        buffer.resize(write_position + len + 1);
    } else {
        size_t readable = readable_bytes();
        std::copy(get_begin_pointer() + read_position, get_begin_pointer() + write_position, get_begin_pointer());
        read_position = 0;
        write_position = readable;
        assert(readable == readable_bytes());
    }
}
