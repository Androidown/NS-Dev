#include "thread_manager.hpp"

ThreadManager::ThreadManager(u64 seed, PMTemplate &pm_tmpl, const PMInfo& pm_info, int thread_num)
    :seed(seed), pm_tmpl(pm_tmpl), pm_info(pm_info), thread_num(std::min(16, thread_num))
{
    u64 base_seed = BASE_SEED * thread_num;
    for (int i = 0; i < thread_num; i++)
    {
        finders[i] = new PMFinder(seed + i*BASE_SEED, base_seed, pm_tmpl, pm_info);
        search_cnt[i].store(0);
    }
}

void ThreadManager::runAll()
{
    exec_flag.store(true);
    std::thread threads[thread_num];

    for (int i = 0; i < thread_num; i++)
    {
        // threads[i] = std::thread([this, i] () {singleRunner(i);});
        threads[i] = std::thread(singleRunner, this, i);
    }
    for (auto& t:threads)
    {
        t.join();
    }

}

void ThreadManager::singleRunner(int id)
{
    u64 cnt = search_cnt[id];
    PMFinder* finder = finders[id];

    while(!finder->foundPM() && exec_flag)
    {
        if (++cnt % 100000000 == 0)
        {
            mtx.lock();
            std::cout << "[thread-" << id <<"] Searched " << cnt << " frames." << std::endl;
            mtx.unlock();
        }
    }
    if (exec_flag)
    {
        mtx.lock();
        std::cout << "[thread-" << id <<"] Seed found at " << cnt*thread_num + id << " frames." << std::endl;
        mtx.unlock();
        exec_flag.store(false);
        rslt_seed.store(finder->xoro.next_seed - thread_num*BASE_SEED);
    }
    search_cnt[id] = cnt;
}

ThreadManager::~ThreadManager()
{
    for (int i=0; i < thread_num; i++)
    {
        delete finders[i];
    }
}
