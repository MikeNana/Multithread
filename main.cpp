#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <stack>
 
 
using namespace std;
class buffer  
{  
    private:  
        boost::mutex mu, io_mu; //互斥变量，配合条件变量来使用的
		
        boost::condition_variable_any cond_get;  //写入条件变量
	    boost::condition_variable_any cond_put;  //读取条件变量
		
        std::stack<int> stk;                    //栈容器
        int un_read, capacity;  
  
        bool is_full()                         //满条件
        {  
            return un_read == capacity;  
        }  
        bool is_empty()                       //栈为空
        {  
            return un_read == 0;  
        }  
	
    public:  
        buffer(size_t n):un_read(0), capacity(n){}  
        void put(int x)  
        {  
            {  										
               boost:: mutex::scoped_lock lock(mu);                     //锁住后,进行条件变量的检测
                while(is_full())  					//判断条件，为啥要用while ？ 不用if?
                {  
                    {  								
                        boost::mutex::scoped_lock lock(io_mu);  
                        std::cout << "full waiting..." << std::endl;  
                    }  
                    cond_put.wait(mu);  
                }                     
                stk.push(x);  //stack容器不满的时候,push进去
                {  
                    boost::mutex::scoped_lock lock(io_mu);  
                    std::cout << "put " << x << std::endl;  
                }  
                ++un_read;  //push 增加一个计数
            }  
            cond_get.notify_one();  //唤醒某个被阻塞的（如果有）消费者线程来读取数
        }  
        void get(int& x)  
        {  
            {  
                boost::mutex::scoped_lock lock(mu);  
  
                while(is_empty())  
                {  
                    {  
                        boost::mutex::scoped_lock lock(io_mu);  
                        std::cout << "empty waiting..." << std::endl;  
                    }  
  
                    cond_get.wait(mu);  
                }  
                --un_read;  
                x = stk.top();  
                {  
                    boost::mutex::scoped_lock lock(io_mu);  
                    std::cout << "get " << x << std::endl;  
                }  
                stk.pop();  
            }  
            cond_put.notify_one();  //只通知其中的某一个线程
        }  
};  
void producer(int n, buffer& buf)  
{  
    for(int i = 0; i < n; ++i)  
    {  
        buf.put(i);  
    }  
}
void consumer(int n, buffer& buf)  
{  
    int x;  
    for(int i = 0; i < n; ++i)  
    {  
        buf.get(x);  
    }  
}  
  
int main()  
{  
   buffer buf(5);  
  
   boost::thread t1(producer, 10, ref(buf));  
   boost::thread t2(producer, 10, ref(buf));
   boost::thread t3(consumer, 20, ref(buf));  
  
   t1.join();  
   t2.join();  
   t3.join();  
  
   return 0;  
}  