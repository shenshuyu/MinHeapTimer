#include "TimerMgr.h"
#include <sys/time.h>

// Timer
MyTimer::MyTimer(TimerManager& manager)
    : manager_(manager)
    , m_nHeapIndex(-1)
{

}

MyTimer::~MyTimer()
{
    stop();
}

void MyTimer::stop()
{
    if (m_nHeapIndex != -1) {
        manager_.remove_timer(this);
        m_nHeapIndex = -1;
    }
}

void MyTimer::on_timer(unsigned long long now)
{
    if (timerType_ == TimerType::CIRCLE) {
        m_nExpires = m_nInterval + now;
        manager_.add_timer(this);
    } else {
        m_nHeapIndex = -1;
    }
    m_timerfunc();
}

// TimerManager
void TimerManager::add_timer(MyTimer* timer)
{
    //插到数组最后一个位置上，上浮
    timer->m_nHeapIndex = heap_.size();
    HeapEntry entry = { timer->m_nExpires, timer};
    heap_.push_back(entry);
    up_heap(heap_.size() - 1);

    std::cout << "timer counter: " << heap_.size() << std::endl;
}

void TimerManager::remove_timer(MyTimer* timer)
{
    //头元素用数组未元素替换，然后下沉
    size_t index = timer->m_nHeapIndex;
    if (!heap_.empty() && index < heap_.size()) {
        if (index == heap_.size() - 1) { //only one timer
            heap_.pop_back();
        } else { //more than one
            swap_heap(index, heap_.size() - 1);
            heap_.pop_back();

            size_t parent = (index - 1) / 2;
            if (index > 0 && heap_[index].time < heap_[parent].time)
                up_heap(index);
            else
                down_heap(index);
        }
    }
}

void TimerManager::detect_timers()
{
    unsigned long long now = get_current_millisecs();

    while (!heap_.empty() && heap_[0].time <= now) {
        MyTimer* timer = heap_[0].timer;
        remove_timer(timer);
        timer->on_timer(now);
    }
}

void TimerManager::up_heap(size_t index)
{
    //下至上，和父节点比较。如果小于父节点上浮
    size_t parent = (index - 1) / 2;
    while (index > 0 && heap_[index].time < heap_[parent].time) {
        swap_heap(index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
}

void TimerManager::down_heap(size_t index)
{
    //从上到下，算出左右子节点，和最小的交换
    size_t lchild = index * 2 + 1;
    while (lchild < heap_.size()) {
        size_t minChild = (lchild + 1 == heap_.size() || heap_[lchild].time < heap_[lchild + 1].time) ? lchild : lchild + 1;
        if (heap_[index].time < heap_[minChild].time)
            break;
        swap_heap(index, minChild);
        index = minChild;
        lchild = index * 2 + 1;
    }
}

void TimerManager::swap_heap(size_t index1, size_t index2)
{
    HeapEntry tmp = heap_[index1];
    heap_[index1] = heap_[index2];
    heap_[index2] = tmp;
    heap_[index1].timer->m_nHeapIndex = index1;
    heap_[index2].timer->m_nHeapIndex = index2;
}

unsigned long long TimerManager::get_current_millisecs()
{
    timeval tv;
    ::gettimeofday(&tv, 0);
    unsigned long long ret = tv.tv_sec;
    return ret * 1000 + tv.tv_usec / 1000;
}