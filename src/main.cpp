#include "main.hpp"

#include "GlobalNamespace/BeatmapLevelFilterModel.hpp"
#include "GlobalNamespace/IPreviewBeatmapLevel.hpp"
using namespace GlobalNamespace;

#include "System/StringComparison.hpp"

using namespace std;

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

MAKE_HOOK_MATCH(BeatmapLevelFilterModel_LevelContainsText, &BeatmapLevelFilterModel::LevelContainsText, bool, IPreviewBeatmapLevel* beatmapLevel, Array<Il2CppString*>* searchTexts) {
    int words = 0;
    int matches = 0;

    auto songName = beatmapLevel->get_songName();
    auto songSubName = beatmapLevel->get_songSubName();
    auto songAuthorName = beatmapLevel->get_songAuthorName();
    auto levelAuthorName = beatmapLevel->get_levelAuthorName();

    for (int i = 0; i < searchTexts->get_Length(); i++)
    {

        if (!csstrtostr(searchTexts->get(i)).empty())
        {

            words++;

            auto searchTerm = searchTexts->get(i);

            if (i == searchTexts->get_Length() - 1)
            {
                searchTerm = searchTerm->Substring(0, searchTerm->get_Length() - 1);
            }

            if (songName->IndexOf(searchTerm, 0, System::StringComparison::CurrentCultureIgnoreCase) != -1 || 
                songSubName->IndexOf(searchTerm, 0, System::StringComparison::CurrentCultureIgnoreCase) != -1 ||
                songAuthorName->IndexOf(searchTerm, 0, System::StringComparison::CurrentCultureIgnoreCase) != -1 ||
                levelAuthorName->IndexOf(searchTerm, 0, System::StringComparison::CurrentCultureIgnoreCase) != -1)
            {
                matches++;
            }

        }

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