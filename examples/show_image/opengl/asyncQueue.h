#pragma once

template<class T>
class AsyncQueue
{
public:
    AsyncQueue() : waitingReaders(0) {}

    int size()
    {
        QMutexLocker locker(&mutex);
        return this->buffer.size();
    }

    void put(const T& item)
    {
        QMutexLocker locker(&mutex);
        this->buffer.push_back(item);
        if(this->waitingReaders)
            this->bufferIsNotEmpty.wakeOne();
    }

    T get()
    {
        QMutexLocker locker(&mutex);
        while(this->buffer.size() == 0)
        {
            ++(this->waitingReaders);
            this->bufferIsNotEmpty.wait(&mutex);
            --(this->waitingReaders);
        }
        T item = this->buffer.front();
        this->buffer.pop_front();
        return item;
    }

private:
    typedef QList<T> Container;
    QMutex mutex;
    QWaitCondition bufferIsNotEmpty;
    Container buffer;
    short waitingReaders;
};