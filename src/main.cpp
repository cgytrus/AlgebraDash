#include <includes.hpp>

static BS::thread_pool* _sharedPool;
BS::thread_pool* sharedPool() { return _sharedPool; }

$on_mod(Loaded) {
    _sharedPool = new BS::thread_pool(std::thread::hardware_concurrency() - 2);
}

$on_mod(Unloaded) {
    delete _sharedPool;
}
