#pragma once
// standard lib
#include <filesystem>
#include <inttypes.h>
// third party
// project

namespace clay {

class Audio {
public:
    /**
     * @brief Load an audio file and save to an AL audio id
     *
     * @param filename Audio file path
     * @return int AL buffer
     */
    static int loadAudio(const std::filesystem::path& filename);

    /**
     * @brief Construct a new Audio object
     *
     * @param filePath file path to load the audio from
     */
    Audio(const std::filesystem::path& filePath);

    /** @brief Destructor */
    ~Audio();

    /** Get the AL buffer id */
    int getId();

private:
    /** AL audio id */
    unsigned int mId_;
};

} // namespace clay