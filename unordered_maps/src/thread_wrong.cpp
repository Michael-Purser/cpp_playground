#include <iostream>
#include <chrono>
#include <cstdlib>
#include <mutex>
#include <string>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <vector>

enum class Selector
{
  BASH_THREAD,
  CPP_THREAD
};

struct threadPIDVector
{
  void initialize(int size);
  int getPID(int index);
  void setPID(int index, int PID);

  std::vector<int> PID_vector;
  std::mutex mutex;
};

void
threadPIDVector::initialize(int size)
{
  std::lock_guard<std::mutex> guard{mutex};
  return PID_vector.assign(size, 0);
}

int
threadPIDVector::getPID(int index)
{
  std::lock_guard<std::mutex> guard{mutex};
  return PID_vector.at(index);
}

void
threadPIDVector::setPID(int index, int PID)
{
  std::lock_guard<std::mutex> guard{mutex};
  PID_vector.at(index) = PID;
}

threadPIDVector thread_PID_vector;

void
func(const int index, const Selector selector, std::string bash_script)
{
  thread_PID_vector.setPID(index, static_cast<int>(getpid()));
  std::cout << "[Thread " << index << "] \t PID: " << thread_PID_vector.getPID(index) << std::endl;
  switch(selector)
  {
    case Selector::BASH_THREAD: 
      system(bash_script.c_str());
      break;

    case Selector::CPP_THREAD:
      while(true)
      {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
      break;
  };
  std::cout << "[Thread " << index << "] \t Exiting" << std::endl;
}

int
main(int argc, char** argv)
{
  // Configs:
  int number_of_threads = 3;
  Selector selector = Selector::BASH_THREAD;
  std::string bash_script = "/home/michael/Documents/CODING/unordered_maps/test.sh > /dev/null";

  std::cout << "[Main thread] \t PID: " << static_cast<int>(getpid()) << std::endl;

  thread_PID_vector.initialize(number_of_threads);
  std::unordered_map<int, std::thread> map2;
  for(int i = 0; i < number_of_threads; ++i)
  {
    map2.insert({i, std::thread(func, i, selector, bash_script)});
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  for(auto& pair : map2)
  {
    std::cout << "Joining thread at key: " << pair.first << "..." << std::endl;
    pair.second.join();
    std::cout << "Joined thread at key: " << pair.first << std::endl;
  }

  return 0;
}