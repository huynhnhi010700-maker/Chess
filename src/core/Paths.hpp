#pragma once
#include <filesystem>
inline std::filesystem::path saveDirectory() {
#ifdef XIANGQI_SAVE_DIRECTORY
    return std::filesystem::u8path(XIANGQI_SAVE_DIRECTORY);
#else
    return std::filesystem::current_path() / "saves";
#endif
}
inline std::filesystem::path quickSavePath() { return saveDirectory() / "last_game.txt"; }
