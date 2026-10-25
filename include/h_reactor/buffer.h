#pragma once

#include <cstddef>
#include <vector>
#include <string>

namespace h_reactor {

class Buffer {
public:
    Buffer(size_t initial_size = 1024);
    ~Buffer();
    Buffer(Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;

    const char* Peek() const;
    size_t ReadableBytes() const;

    char* BeginWrite();
    size_t WritableBytes() const;
    void HasWritten(size_t);

    void Retrieve(size_t);
    void RetrieveAll();
    std::string RetrieveAsString(size_t);
    std::string RetrieveAllAsString();

    ssize_t ReadFd(int);

    void Append(const char*, size_t);

private:
    std::vector<char> buffer_;
    size_t reader_index_;
    size_t writer_index_;
};

}