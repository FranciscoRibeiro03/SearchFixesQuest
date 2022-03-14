#include "main.hpp"

#include "GlobalNamespace/BeatmapLevelFilterModel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
using namespace GlobalNamespace;

#include "System/StringComparison.hpp"

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

void toLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

bool findSubstring(std::string str, std::string substr) {
    toLower(str);
    toLower(substr);
    return str.find(substr) != std::string::npos;
}

MAKE_HOOK_MATCH(BeatmapLevelFilterModel_LevelContainsText, &BeatmapLevelFilterModel::LevelContainsText, bool, IPreviewBeatmapLevel* beatmapLevel, ArrayW<StringW> searchTexts) {
    int words = 0;
    int matches = 0;

    auto songName = beatmapLevel->get_songName();
    auto songSubName = beatmapLevel->get_songSubName();
    auto songAuthorName = beatmapLevel->get_songAuthorName();
    auto levelAuthorName = beatmapLevel->get_levelAuthorName();

    auto arrayLen = searchTexts.Length();

    for (int i = 0; i < arrayLen; i++)
    {

        StringW searchTerm = searchTexts[i];

        // getLogger().debug("Searching for '%s' in '%s'", static_cast<std::string>(searchTerm).c_str(), static_cast<std::string>(songName).c_str()); // Debugging Stuff

        if (searchTerm->get_Length() == 0)
            continue;

        words++;

        bool found = findSubstring(songName, searchTerm) ||
            findSubstring(songSubName, searchTerm) ||
            findSubstring(songAuthorName, searchTerm) ||
            findSubstring(levelAuthorName, searchTerm);

        /* Debugging stuff
        getLogger().debug("songName: '%s'; songSubName: '%s'; songAuthorName: '%s'; levelAuthorName: '%s'; searchTermString: %s", static_cast<std::string>(songName).c_str(), static_cast<std::string>(songSubName).c_str(), static_cast<std::string>(songAuthorName).c_str(), static_cast<std::string>(levelAuthorName).c_str(), searchTermString.c_str());
        getLogger().debug("Found: %s", found ? "true" : "false");
        getLogger().debug("");
        */


        if (found) matches++;

    }

    return matches == words;
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getLogger().info("Installing hooks...");
    INSTALL_HOOK(getLogger(), BeatmapLevelFilterModel_LevelContainsText);
    getLogger().info("Installed all hooks!");
}