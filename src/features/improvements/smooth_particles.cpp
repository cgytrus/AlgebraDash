#include <Tracy.hpp>
#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <Geode/modify/CCParticleSystem.hpp>

// ported from https://github.com/cgytrus/SmoothParticles

static constexpr float tooFast = 50.f;

// TODO: mac addresses for update and initParticle
#ifdef GEODE_IS_WINDOWS
struct SmoothParticleSystem : geode::Modify<SmoothParticleSystem, CCParticleSystem> {
    bool m_firstTime = true;
    CCPoint m_prevPosition{0.f, 0.f};
    size_t m_newCount = 0;
    size_t m_newIndex = 0;
    float m_xDiff = 0.f;
    float m_yDiff = 0.f;

    // i'm too lazy to make a formula out of this
    // so instead i'm just gonna simulate the amount of times initParticle will be called h
    // (this is basically a decomp of the first half of CCParticleSystem::update)
    size_t nextParticlesCount(float dt) {
        ZoneScoped;

        if(!m_bIsActive || m_fEmissionRate == 0.f)
            return 0;

        size_t finalCount = 0;

        auto particleCount = m_uParticleCount;
        auto totalParticles = m_uTotalParticles;

        float emissionPeriod = 1.f / m_fEmissionRate;
        unsigned int curParticleCount = particleCount;
        float startEmissionPeriod = emissionPeriod;

        if(curParticleCount >= totalParticles)
            return 0;

        auto emitCounter = m_fEmitCounter + dt;
        do {
            if(emitCounter <= emissionPeriod)
                break;

            if(curParticleCount != totalParticles) {
                finalCount++;
                particleCount++;
                emissionPeriod = startEmissionPeriod;
            }

            curParticleCount = particleCount;
            emitCounter -= emissionPeriod;
        } while(curParticleCount < totalParticles);

        return finalCount;
    }

    void update(float dt) {
        ZoneScoped;

        m_fields->m_newCount = 0;

        CCPoint curr = getPosition();

        // don't interpolate if running for the first time
        if(m_fields->m_firstTime) {
            CCParticleSystem::update(dt);
            m_fields->m_prevPosition = curr;
            m_fields->m_firstTime = false;
            return;
        }

        CCPoint prev = m_fields->m_prevPosition;
        m_fields->m_prevPosition = curr;

        m_fields->m_xDiff = curr.x - prev.x;
        m_fields->m_yDiff = curr.y - prev.y;

        // don't interpolate if didn't move or moved too fast
        if(m_fields->m_xDiff == 0.f && m_fields->m_yDiff == 0.f ||
            m_fields->m_xDiff >= tooFast || m_fields->m_yDiff >= tooFast) {
            CCParticleSystem::update(dt);
            return;
        }

        m_fields->m_newCount = nextParticlesCount(dt);
        m_fields->m_newIndex = 0;

        CCParticleSystem::update(dt);
    }

    void initParticle(tCCParticle* particle) {
        ZoneScoped;

        CCParticleSystem::initParticle(particle);

        if(m_fields->m_newCount == 0)
            return;

        float t = (float)m_fields->m_newIndex / m_fields->m_newCount;

        // don't interpolate on x if didn't move on x
        if(m_fields->m_xDiff != 0.f) {
            float tx = t * m_fields->m_xDiff;
            particle->startPos.x = tx + (particle->startPos.x - m_fields->m_xDiff);
        }

        // don't interpolate on y if didn't move on y
        if(m_fields->m_yDiff != 0.f) {
            float ty = t * m_fields->m_yDiff;
            particle->startPos.y = ty + (particle->startPos.y - m_fields->m_yDiff);
        }

        m_fields->m_newIndex++;
    }

    void resetSystem() {
        ZoneScoped;

        m_fields->m_firstTime = true;
        CCParticleSystem::resetSystem();
    }
    void resumeSystem() {
        ZoneScoped;

        m_fields->m_firstTime = true;
        CCParticleSystem::resumeSystem();
    }
};
#endif
