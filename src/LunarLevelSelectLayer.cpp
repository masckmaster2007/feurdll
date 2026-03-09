#include <Geode/Geode.hpp>
#include <Geode/modify/LevelSelectLayer.hpp>

using namespace geode::prelude;

int count = 27;

class $modify(HookedLevelSelectLayer, LevelSelectLayer) {
    $override bool init(int pageID) {
        if(!LevelSelectLayer::init(pageID))
            return false;
        
        auto GSM = GameStatsManager::sharedState();
        auto GM = GameManager::sharedState();

        if(GSM->getStat("8") >= 20 && !GM->getUGV("52")) {
            auto delay = CCDelayTime::create(0.4f);
            auto func = CCCallFunc::create(this, callfunc_selector(HookedLevelSelectLayer::createGoldCoinDialog));
            this->runAction(CCSequence::create(delay, func, 0));
        }

        m_scrollLayer->m_dynamicObjects->removeAllObjects();

        auto dotsArray = CCArrayExt<CCSprite*>(m_scrollLayer->m_dots);

        for(CCSprite* dot : dotsArray) {
            dot->removeFromParent();
        }

        m_scrollLayer->m_dots->removeAllObjects();

        for(int i = 1; i < count; i++) {
            m_scrollLayer->m_dynamicObjects->addObject(GameLevelManager::get()->getMainLevel(i, true));
        }
        auto towerLevel = GJGameLevel::create();
        towerLevel->m_levelID = -2;
        auto comingSoon = GJGameLevel::create();
        comingSoon->m_levelID = -1;

        m_scrollLayer->m_dynamicObjects->addObject(towerLevel);
        m_scrollLayer->m_dynamicObjects->addObject(comingSoon);

        auto batchNode = CCSpriteBatchNode::create("smallDot.png", 29);
        m_scrollLayer->addChild(batchNode, 5);

        for(int i = 0; i < m_scrollLayer->m_dynamicObjects->count(); i++) {
            auto sprite = CCSprite::create("smallDot.png");
            batchNode->addChild(sprite);
            m_scrollLayer->m_dots->addObject(sprite);
        }

        m_scrollLayer->updateDots(0.f);
        m_scrollLayer->updatePages();
        this->updatePageWithObject(m_scrollLayer->m_pages->objectAtIndex(pageID % 3), m_scrollLayer->m_dynamicObjects->objectAtIndex(pageID));
        this->m_scrollLayer->repositionPagesLooped();

        return true;
    }
};