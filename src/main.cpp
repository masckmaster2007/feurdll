/**
 * Include the Geode headers.
 */
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/utils/casts.hpp>

/**
 * Brings cocos2d and all Geode namespaces to the current scope.
 */
using namespace geode::prelude;

/**
 * `$modify` lets you extend and modify GD's classes.
 * To hook a function in Geode, simply $modify the class
 * and write a new function definition with the signature of
 * the function you want to hook.
 *
 * Here we use the overloaded `$modify` macro to set our own class name,
 * so that we can use it for button callbacks.
 *
 * Notice the header being included, you *must* include the header for
 * the class you are modifying, or you will get a compile error.
 *
 * Another way you could do this is like this:
 *
 * struct MyMenuLayer : Modify<MyMenuLayer, MenuLayer> {};
 */
#include <Geode/modify/MenuLayer.hpp>
class $modify(MyMenuLayer, MenuLayer) {
	/**
	 * Typically classes in GD are initialized using the `init` function, (though not always!),
	 * so here we use it to add our own button to the bottom menu.
	 *
	 * Note that for all hooks, your signature has to *match exactly*,
	 * `void init()` would not place a hook!
	*/
	bool init() {
		/**
		 * We call the original init function so that the
		 * original class is properly initialized.
		 */
		if (!MenuLayer::init()) {
			return false;
		}

		/**
		 * You can use methods from the `geode::log` namespace to log messages to the console,
		 * being useful for debugging and such. See this page for more info about logging:
		 * https://docs.geode-sdk.org/tutorials/logging
		*/
		log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());

		/**
		 * See this page for more info about buttons
		 * https://docs.geode-sdk.org/tutorials/buttons
		*/
		auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_gpgBtn_001.png"),
			this,
			/**
			 * Here we use the name we set earlier for our modify class.
			*/
			menu_selector(MyMenuLayer::onMyButton)
		);

		/**
		 * Here we access the `bottom-menu` node by its ID, and add our button to it.
		 * Node IDs are a Geode feature, see this page for more info about it:
		 * https://docs.geode-sdk.org/tutorials/nodetree
		*/
		auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		/**
		 * The `_spr` string literal operator just prefixes the string with
		 * your mod id followed by a slash. This is good practice for setting your own node ids.
		*/
		myButton->setID("my-button"_spr);

		/**
		 * We update the layout of the menu to ensure that our button is properly placed.
		 * This is yet another Geode feature, see this page for more info about it:
		 * https://docs.geode-sdk.org/tutorials/layouts
		*/
		menu->updateLayout();

		/**
		 * We return `true` to indicate that the class was properly initialized.
		 */
		return true;
	}

	/**
	 * This is the callback function for the button we created earlier.
	 * The signature for button callbacks must always be the same,
	 * return type `void` and taking a `CCObject*`.
	*/
	void onMyButton(CCObject*) {
		web::openLinkInBrowser("https://gdps.dimisaio.be/");
	}

	void onMoreGames(CCObject*) {
		web::openLinkInBrowser("https://gdps.dimisaio.be/moregames.html");
	} 
};

// Taken from the SecretLayer6 mod, I'm sorry
// and geode docs kms
#include <Geode/modify/SecretLayer5.hpp>
#include <Geode/loader/Event.hpp>
class $modify(MySecretLayer5, SecretLayer5) {

	struct Fields {
        	EventListener<web::WebTask> m_listener;
    	};

	void onSubmit(CCObject * sender) {
		std::string text = this->m_textInput->getString();
		std::transform(
			text.begin(),
			text.end(),
			text.begin(),
			[](unsigned char c) {
				return std::tolower(c);
			}
		);

		m_fields->m_listener.bind([] (web::WebTask::Event* e) {
		    if (web::WebResponse* res = e->getValue()) {
			std::string tesla = res->string().unwrapOr("0");
			if(tesla != "0") {
				web::openLinkInBrowser(tesla);
				return;
			}
		    } else if (web::WebProgress* p = e->getProgress()) {
			log::info("progress: {}", p->downloadProgress().value_or(0.f));
		    } else if (e->isCancelled()) {
			log::info("The request was cancelled... So sad :(");
		    }
		});

	        auto req = web::WebRequest();
	        // Let's fetch... uhh...
			std::string url = "https://gdps.dimisaio.be/database/getTesla.php?key=";
			url += text;
	        m_fields->m_listener.setFilter(req.get(url));

		SecretLayer5::onSubmit(sender);
	}
};

#include <Geode/modify/CreatorLayer.hpp>
class $modify(MyCreatorLayer, CreatorLayer) {
    struct Fields {
        bool m_demonlistButton = true;
    };

    bool init() override {
        if (!CreatorLayer::init()) return false;

        auto menu = static_cast<CCMenu*>(this->getChildByID("creator-buttons-menu"));
        if (!menu) return true;

        auto mapBtn = static_cast<CCMenuItemSpriteExtra*>(menu->getChildByID("versus-button"));
        if (mapBtn) mapBtn->setVisible(false);

        auto versus = CCSprite::create("dl.png"_spr);
        versus->setScale(0.8f);

        auto versusBtn = CCMenuItemSpriteExtra::create(
            versus,
            nullptr,
            this,
            menu_selector(MyCreatorLayer::onVersus)
        );
        versusBtn->setID("demonlist-button");

		worldsBtn->setPosition(mapBtn->getPositionX() + 2.f, mapBtn->getPositionY() - 2.f);

        menu->addChild(versusBtn);
        return true;
    }

    void onVersus(CCObject*) {
        if (!m_fields->m_demonlistButton) return;

        // Open custom search type
        auto searchObj = GJSearchObject::create(static_cast<SearchType>(3142), "");
        auto browser = LevelBrowserLayer::create(searchObj);

        auto scene = CCScene::create();
        scene->addChild(browser);

        CCDirector::sharedDirector()->pushScene(
            CCTransitionFade::create(0.5f, scene)
        );
    }
};
