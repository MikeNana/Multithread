//a thread_safe counter
//In a real world, atomic operations are preferred.
#include <mutex>
#include <boost/noncopyable.hpp>
#include <iostream>
using std::mutex;
using std::unique_lock;

class Counter:boost::noncopyable
{
    public:
        Counter():value_(0), lck(mutex_){}
        int value() const;
        int getAndIncrease();
    private:
        int value_;
        mutable mutex mutex_;
        mutable unique_lock<mutex> lck;
};

int Counter::value() const
{
    lck.lock();
    return value_;
}
int Counter::getAndIncrease()
{
    lck.lock();
    int ret = value_++;
    return ret;
}

int main()
{
    std::cout << 1 << std::endl;
    return 0;
}