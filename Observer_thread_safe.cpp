#include <boost/noncopyable.hpp>
#include <shared_mutex>
#include <memory>
#include <mutex>
#include <vector>
#include <iterator>
#include <iostream>
#include <thread>

using std::weak_ptr;
using std::iterator;
using std::vector;
using std::mutex;
using std::shared_ptr;
using std::cout;
using std::unique_lock;

class observer
{
    private:
        std::shared_ptr<observable> subject_;
        void observe(std::shared_ptr<observable>);
    public:
        virtual ~observer();
        virtual void update() = 0;
};
class observable
{
public:
    void register_(const weak_ptr<observer>& obs);
    void notifyobservers();
private:
    mutable mutex mutex_;
    vector<weak_ptr<observer>> observer_;
    typedef std::vector<weak_ptr<observer>>::iterator Iterator;

};

void observable::register_(const weak_ptr<observer>& obs)
{
    unique_lock<mutex> lck(mutex_);
    Iterator it = observer_.begin();
    while(it != observer_.end())
    {
        //将weak_ptr提升到shared_ptr，如果为空说明对象已经销毁
        shared_ptr<observer> obj(it->lock());
        if(!obj)
        {
            obj->update();
            ++it;
        }
        else
            it = observer_.erase(it);
    }
}

void observable::notifyobservers()
{
    cout << std::endl;
}

