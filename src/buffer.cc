#include "h_reactor/buffer.h"
#include <algorithm>
#include <bits/types/struct_iovec.h>
#include <cstddef>
#include <sys/types.h>
#include <sys/uio.h>

namespace h_reactor {

Buffer::Buffer(size_t initial_size)
    : buffer_(initial_size)
    , reader_index_(0)
    , writer_index_(0)
{}

Buffer::~Buffer() = default;

const char* Buffer::Peek() const {
    return buffer_.data() + reader_index_;
}

size_t Buffer::ReadableBytes() const {
    return writer_index_ - reader_index_;
}

char* Buffer::BeginWrite(){
    return buffer_.data() + writer_index_;
}

size_t Buffer::WritableBytes() const {
    return buffer_.size() - writer_index_;
}

void Buffer::Retrieve(size_t len) {
    if (len < ReadableBytes()) {
        reader_index_ += len;
    } else {
        RetrieveAll();
    }
}

void Buffer::RetrieveAll() {
    writer_index_ = 0;
    reader_index_ = 0;
}

std::string Buffer::RetrieveAsString(size_t len) {
    std::string res(Peek(), len);
    Retrieve(len);
    return res;
}

std::string Buffer::RetrieveAllAsString() {
    return RetrieveAsString(ReadableBytes());
}

void Buffer::Append(const char* data, size_t len) {
    if (WritableBytes() < len) {
        buffer_.resize(writer_index_ + len);
    }
    std::copy(data, data + len, buffer_.data() + writer_index_);
    writer_index_ += len;
}

ssize_t Buffer::ReadFd(int fd) {
    char extrabuf[65536];
    iovec vec[2];

    vec[0].iov_base = BeginWrite();
    vec[0].iov_len = WritableBytes();

    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof(extrabuf);

    int iovec_cnt = (WritableBytes() > sizeof(extrabuf) ? 1 : 2);

    ssize_t n = ::readv(fd, vec, iovec_cnt);
    if (n < 0) {}
    else if (static_cast<size_t>(n) <= WritableBytes()) {
        writer_index_ += n;
    } else {
        writer_index_ = buffer_.size();
        Append(extrabuf, n - WritableBytes());
    }

    return n;
}

}