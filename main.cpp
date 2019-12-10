//#include <boost/thread/thread.hpp>
//#include <boost/bind.hpp>
//对于多线程的程序来说，测试用例规模太小有时候看不出问题
#include <iostream>
#include <stack>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <future>
#include <cassert>
#include <vector>
#include <exception>

using std::cout;
using std::endl;
using std::thread;
using std::mutex;
using std::timed_mutex;
using std::unique_lock;
using std::vector;
using std::defer_lock;
using std::condition_variable;
using std::string;
using std::future;
using std::promise;
using std::move;
using std::cin;
using std::packaged_task;
using std::async;
using std::atomic;
using std::atomic_flag;

mutex mutex1;
int cargo = 0;//生产者线程和消费者线程的共享变量
condition_variable cv;
bool processed = false; //；表示数据是否处理完成　
string data = "example data";
promise<int> prom;
atomic<bool> ready(false);
atomic_flag winner = ATOMIC_FLAG_INIT;
atomic_flag self = ATOMIC_FLAG_INIT;

void f(int id)
{
    for(int i = 0; i < 100; ++i)
    {
        //当标志位处于false状态时，第一次拿到锁，不会进入while循环，如果其他线程在锁已经被占据的情况下想拿锁
        //则会进入while循环
        //当拿到锁的线程clear标志位状态时，其他线程重新竞争锁，变为false,退出while循环
        //这就是自旋锁
        while(self.test_and_set(std::memory_order_acquire));    
        cout << "Output from thread " << id << endl;
        self.clear(std::memory_order_release);
    }
}
atomic<int> foo = 0;
void set_foo(int x)
{
    foo = x;
}
void print_foo()
{
    while(foo == 0)
        std::this_thread::yield();
    cout << foo << endl;
}
int main ()
{
    vector<thread> threads;

    return 0;
}