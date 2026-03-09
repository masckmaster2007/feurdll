#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// duh
class $modify(GJBaseGameLayer) {
    bool shouldExitHackedLevel() {
        return false;
    }
};

// allows tiny-rated levels to be completed by tricking the game it's 10s long
class $modify(PlayLayer) {
    void levelComplete() {
        if (this->m_timePlayed < 10.0) this->m_timePlayed = 10.0;
        if (this->m_attemptTime < 10.0) this->m_attemptTime = 10.0;
        PlayLayer::levelComplete();
    }
}; 

// makes main levels always pass the level integrity check
class $modify(LevelTools) {
    static bool verifyLevelIntegrity(gd::string p0, int p1) {
        //LevelTools::verifyLevelIntegrity(p0, p1);
        return true;
    }
}; 