#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <stdexcept>
class ThreadPool {
 struct Job {int priority;size_t order;std::function<void()> run;};
 struct Compare {bool operator()(const Job&a,const Job&b)const{return a.priority==b.priority?a.order>b.order:a.priority<b.priority;}};
 std::priority_queue<Job,std::vector<Job>,Compare> jobs;
 std::mutex mutex;std::condition_variable changed;std::vector<std::thread> threads;
 bool stopping=false;size_t sequence=0;
public:
 explicit ThreadPool(size_t count=std::thread::hardware_concurrency()){
  if(!count)count=1;
  for(size_t i=0;i<count;++i)threads.emplace_back([this]{
   for(;;){Job job;
    {std::unique_lock<std::mutex> lock(mutex);changed.wait(lock,[this]{return stopping||!jobs.empty();});
     if(stopping&&jobs.empty())return;job=jobs.top();jobs.pop();}
    job.run();
   }
  });
 }
 ThreadPool(const ThreadPool&)=delete;ThreadPool&operator=(const ThreadPool&)=delete;
 template<class F> auto submit(F function,int priority=0)->std::future<decltype(function())>{
  using T=decltype(function());auto task=std::make_shared<std::packaged_task<T()>>(std::move(function));auto future=task->get_future();
  {std::lock_guard<std::mutex> lock(mutex);if(stopping)throw std::runtime_error("pool stopped");jobs.push({priority,sequence++,[task]{(*task)();}});}
  changed.notify_one();return future;
 }
 ~ThreadPool(){
  {std::lock_guard<std::mutex> lock(mutex);stopping=true;}changed.notify_all();
  for(auto&t:threads)t.join();
 }
};
