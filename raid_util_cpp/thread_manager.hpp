#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include <thread>
#include <mutex>
#include <vector>
#include <atomic>
#include "pm_finder.hpp"


class ThreadManager
{
public:
    std::atomic<u64> rslt_seed;
    u64 seed;
    ThreadManager(u64 seed, PMTemplate &pm_tmpl, const PMInfo& pm_info, int thread_num);
    void runAll();
    void singleRunner(int id);
    ~ThreadManager();

private:
    volatile std::atomic<bool> exec_flag;
    int thread_num;
    PMTemplate& pm_tmpl;
    const PMInfo& pm_info;
    std::mutex mtx;
    PMFinder *finders[16];
    std::atomic<u64> search_cnt[16];

};

#endif  // THREAD_MANAGER_H
