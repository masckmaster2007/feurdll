#include <Geode/Geode.hpp>
#include <Geode/modify/LevelTools.hpp>
#include <Geode/utils/file.hpp> // geode::utils::file::readJson
#include <array>
#include <algorithm>

using namespace geode::prelude;

static matjson::Value levels;
static matjson::Value tracks;
static matjson::Value artists;

$on_mod(Loaded) {
    auto levelsFile = Mod::get()->getConfigDir(false) / "levels.json";
    auto tracksFile = Mod::get()->getConfigDir(false) / "tracks.json";
    auto artistsFile= Mod::get()->getConfigDir(false) / "artists.json";

    auto lvls = geode::utils::file::readJson(levelsFile);
    auto trs = geode::utils::file::readJson(tracksFile);
    auto arts = geode::utils::file::readJson(artistsFile);
    if (lvls && trs && arts) {
        levels = lvls.unwrap();
        tracks = trs.unwrap();
        artists = arts.unwrap();
    }
}

class $modify(OdysseyLevelTools, LevelTools)
{
	$override static gd::string getAudioTitle(int trackID)
	{
        std::string tkey = std::to_string(trackID);
        if (!tracks.contains(tkey)) return LevelTools::getAudioTitle(trackID);
        return tracks[tkey]["m_title"].asString().unwrap();
	}

	$override static gd::string getAudioFileName(int trackID)
	{
        std::string tkey = std::to_string(trackID);
        if (!tracks.contains(tkey)) return LevelTools::getAudioFileName(trackID);
        return tracks[tkey]["m_fileName"].asString().unwrap();
	}

	$override static int artistForAudio(int p0)
	{
        auto tkey = std::to_string(p0);

        if (!tracks.contains(tkey))
            return LevelTools::artistForAudio(p0);

        auto track = tracks[tkey];

        int artist = track["m_artistID"].asInt().unwrapOr(0);
        return artist == 0 ? LevelTools::artistForAudio(p0) : artist;
	}

	$override static gd::string nameForArtist(int p0)
	{
        auto akey = std::to_string(p0);

        if (!artists.contains(akey))
            return LevelTools::nameForArtist(p0);

        gd::string artist = artists[akey]["name"].asString().unwrapOr("");
        return artist.empty() ? LevelTools::nameForArtist(p0) : artist;
	}

	$override static gd::string ngURLForArtist(int p0)
	{
        auto akey = std::to_string(p0);

        if (!artists.contains(akey))
            return LevelTools::nameForArtist(p0);

        gd::string artist = artists[akey]["link"].asString().unwrapOr("");
        return artist.empty() ? LevelTools::nameForArtist(p0) : artist;
	}


    static GJDifficulty zmlDiff(int diff) {
        diff = std::clamp(diff, 1, 6);

        static const std::array<GJDifficulty, 6> diffs = {
            GJDifficulty::Easy,
            GJDifficulty::Normal,
            GJDifficulty::Hard,
            GJDifficulty::Harder,
            GJDifficulty::Insane,
            GJDifficulty::Demon,
        };

        return diffs[diff - 1];
    }

	// $override static GJGameLevel *getLevel(int levelID, bool loaded)
	// {
	// 	GJGameLevel *level = GJGameLevel::create();

    //     std::string lkey = std::to_string(levelID);
    //     if (!levels.contains(lkey)) return LevelTools::getLevel(levelID, loaded);
        
    //     level->m_levelName = levels[lkey]["m_levelName"].asString().unwrap();
    //     level->m_audioTrack = levels[lkey]["m_audioTrack"].asInt().unwrap();
    //     level->m_stars = levels[lkey]["m_stars"].asInt().unwrap();
    //     level->m_difficulty = zmlDiff(levels[lkey]["m_difficulty"].asInt().unwrap());
    //     level->m_requiredCoins = 0;
    //     level->m_timestamp = 24273;

	// 	if (!loaded)
	// 		level->m_levelString = LocalLevelManager::sharedState()->getMainLevelString(levelID);

	// 	level->m_levelType = GJLevelType::Main;
	// 	level->m_levelID = levelID;

	// 	return level;
	// };

    $override static GJGameLevel *getLevel(int levelID, bool loaded)
	{
		GJGameLevel *level = LevelTools::getLevel(levelID, loaded);

        std::string lkey = std::to_string(levelID);
        if (!levels.contains(lkey)) return level;
        
        level->m_levelName = levels[lkey]["m_levelName"].asString().unwrap();
        level->m_audioTrack = levels[lkey]["m_audioTrack"].asInt().unwrap();
        level->m_stars = levels[lkey]["m_stars"].asInt().unwrap();
        level->m_difficulty = zmlDiff(levels[lkey]["m_difficulty"].asInt().unwrap());
        level->m_requiredCoins = 0;
        level->m_timestamp = 24273;

		if (!loaded)
			level->m_levelString = LocalLevelManager::sharedState()->getMainLevelString(levelID);

		level->m_levelType = GJLevelType::Main;
		level->m_levelID = levelID;

		return level;
	};

	$override static gd::string urlForAudio(int songID)
	{
        auto tkey = std::to_string(songID);

        if (!tracks.contains(tkey))
            return LevelTools::urlForAudio(songID);

        return tracks[tkey]["m_url"].asString().unwrap();
	}
};

#include <Geode/modify/GameLevelManager.hpp>
class $modify(MLE_GameLevelManager, GameLevelManager) {

    $override GJGameLevel* getMainLevel(int levelID, bool dontGetLevelString) {
        return OdysseyLevelTools::getLevel(levelID, dontGetLevelString);
    };

};