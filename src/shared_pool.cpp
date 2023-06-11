#include <shared_pool.hpp>
BS::thread_pool& sharedPool() {
    static BS::thread_pool _sharedPool(std::thread::hardware_concurrency() - 2);
    return _sharedPool;
}
