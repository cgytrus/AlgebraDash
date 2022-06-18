#include <Tracy.hpp>
#include <Geode.hpp>
USE_GEODE_NAMESPACE();

#include "../ThreadPool.hpp"

GEODE_API bool GEODE_CALL geode_load(Mod* mod) {
    initThreadPool();
    return true;
}

GEODE_API void GEODE_CALL geode_unload() {
    deinitThreadPool();
}
