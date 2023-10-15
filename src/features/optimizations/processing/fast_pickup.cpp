#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <Geode/modify/GJBaseGameLayer.hpp>
struct FastPickup : Modify<FastPickup, GJBaseGameLayer> {
    void collectItem(int id, int count) {
        if (count == 0)
            return;
        if (id < 0)
            id = 0;
        else if (id > 1100)
            id = 1100;
        int value = m_effectManager->m_itemValues[id] + count;
        m_effectManager->m_itemValues[id] = value;
        m_effectManager->countChangedForItem(id);
        this->updateCounters(id, value);
    }
};
