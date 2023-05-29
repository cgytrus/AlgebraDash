#include <includes.hpp>

#include <features/base.hpp>

#ifdef OPTIMIZATIONS
#include <features/optimizations/loading.hpp>
#include <features/optimizations/rendering.hpp>
#endif

static BS::thread_pool* _sharedPool;
BS::thread_pool* sharedPool() { return _sharedPool; }

$on_mod(Loaded) {
    _sharedPool = new BS::thread_pool(std::thread::hardware_concurrency() - 2);
#ifdef OPTIMIZATIONS
    static ad::LoadingOptimization loading;
    static ad::RenderingOptimization rendering;

    ad::features.push_back(&loading);
    ad::features.push_back(&rendering);

    for(auto feat : ad::features) {
        feat->init();
    }
#endif
}

$on_mod(Unloaded) {
    delete _sharedPool;
}
