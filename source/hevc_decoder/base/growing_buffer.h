#ifndef _GROWINGBUFFER_H_
#define _GROWINGBUFFER_H_

#include <memory>

template <class T> class GrowingBuffer
{
public:
    GrowingBuffer(int pre_allocation_size);
    virtual ~GrowingBuffer() {}

    void SetSize(int size);

    int GetSize() const { return size_; }
    T* GetBuffer() const { return buffer_.get(); }

private:
    std::unique_ptr<T[]> buffer_;
    int size_;
};

template <class T> GrowingBuffer<T>::GrowingBuffer(int pre_allocation_size)
    : buffer_(new T[pre_allocation_size])
    , size_(pre_allocation_size)
{
}

template <class T> void GrowingBuffer<T>::SetSize(int size)
{
    if (size > size_) 
    {
        std::unique_ptr<T[]> temp_buffer(new T[size_]);
        memcpy(temp_buffer.get(), buffer_.get(), size_);
        buffer_.reset(new T[size]);
        memcpy(buffer_.get(), temp_buffer.get(), size_);
        size_ = size;
    }
}

#endif