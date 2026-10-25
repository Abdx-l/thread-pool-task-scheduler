#include "pool.hpp"
#include <atomic>
#include <cassert>
int main(){
 std::atomic<int> count{0};
 {ThreadPool pool(4);std::vector<std::thread> producers;
  for(int p=0;p<8;++p)producers.emplace_back([&]{for(int i=0;i<12500;++i)pool.submit([&]{++count;});});
  for(auto&t:producers)t.join();
  auto failure=pool.submit([]()->int{throw std::runtime_error("expected");});
  bool caught=false;try{failure.get();}catch(const std::runtime_error&){caught=true;}assert(caught);
 }assert(count==100000);
 std::promise<void> gate;auto ready=gate.get_future().share();
 ThreadPool one(1);auto blocking=one.submit([ready]{ready.wait();});
 std::vector<int> order;auto a=one.submit([&]{order.push_back(1);},1);auto b=one.submit([&]{order.push_back(2);},9);
 gate.set_value();a.get();b.get();assert((order==std::vector<int>{2,1}));
}
