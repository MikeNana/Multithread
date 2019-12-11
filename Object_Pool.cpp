//讲述如何写一个线程安全的对象池(此处是股票对象池)
//采用了shared_ptr和weak_ptr合用的技术、弱回调技术实现"如果对象活着则调用成员函数，否则忽略"的特性

#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <mutex>
#include <map>
#include <thread>
class StockFactory:public boost::enable_shared_from_this<StockFactory>,
                   boost::noncopyable
{
private:
    mutable std::mutex mutex_;                              //保证即使在const函数中也可以进行加锁的操作
    std::map<std::string, boost::weak_ptr<Stock>> stocks_;  //用智能指针的原因是保证在多线程的程序中，返回的对象并没有被销毁；如果使用shared_ptr，则包裹的对象永远无法被销毁，所以最终选择使用weak_ptr
    static void weakDeleteCallback(const boost::weak_ptr<StockFactory>& wkFactory, Stock* stock)    //静态成员函数，减少对象所占空间
    {
        boost::shared_ptr<StockFactory> factory(wkFactory.lock());
        if(factory)     //如果对象池还存在，则清除stock_中的stock对象
        {
            factory->removeStock(stock);
        }
        delete stock;
    }
    //从stock_中删除对应的stock对象
    void removeStock(Stock* stock)
    {
        if(stock)
        {
            mutex_.lock();
            stocks_.erase(stock->key());
            mutex_.unlock();
        }
    }
public:
    std::shared_ptr<Stock> get(const std::string& key)
    {
        std::shared_ptr<Stock> pstock;
        mutex_.lock();
        std::weak_ptr<Stock>& wkstock = stock_[key];
        pstock = wkstock.lock();
        if(!pstock)
        {
            pstock.reset(new Stock(key), boost::bind(&StockFactory::weakDeleteCallback, boost::weak_ptr<StockFactory>(shared_from_this()),_1));
            wkstock = pstock;
        }
        return pstock;
    }
};