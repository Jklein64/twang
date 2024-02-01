#pragma once

#include <array>
#include <string>

namespace Notes
{
    const float C1 = 32.70f, Db1 = 34.65f, D1 = 36.71f, Eb1 = 38.89f,
                E1 = 41.20f, F1 = 43.65f, Gb1 = 46.25f, G1 = 49.00f,
                Ab1 = 51.91f, A1 = 55.00f, Bb1 = 58.27f, B1 = 61.74f,

                C2 = 65.41f, Db2 = 69.30f, D2 = 73.42f, Eb2 = 77.78f,
                E2 = 82.41f, F2 = 87.31f, Gb2 = 92.50f, G2 = 98.00f,
                Ab2 = 103.8f, A2 = 110.0f, Bb2 = 116.5f, B2 = 123.5f,

                C3 = 130.8f, Db3 = 138.6f, D3 = 146.8f, Eb3 = 155.6f,
                E3 = 164.8f, F3 = 174.6f, Gb3 = 185.0f, G3 = 196.0f,
                Ab3 = 207.7f, A3 = 220.0f, Bb3 = 233.1f, B3 = 246.9f,

                C4 = 261.6f, Db4 = 277.2f, D4 = 293.7f, Eb4 = 311.1f,
                E4 = 329.6f, F4 = 349.2f, Gb4 = 370.0f, G4 = 392.0f,
                Ab4 = 415.3f, A4 = 440.0f, Bb4 = 466.2f, B4 = 493.9f,

                C5 = 523.3f, Db5 = 554.4f, D5 = 587.3f, Eb5 = 622.3f,
                E5 = 659.3f, F5 = 698.5f, Gb5 = 740.0f, G5 = 784.0f,
                Ab5 = 830.6f, A5 = 880.0f, Bb5 = 932.3f, B5 = 987.8f,

                C6 = 1047.0f, Db6 = 1109.0f, D6 = 1175.0f, Eb6 = 1245.0f,
                E6 = 1319.0f, F6 = 1397.0f, Gb6 = 1480.0f, G6 = 1568.0f,
                Ab6 = 1661.0f, A6 = 1760.0f, Bb6 = 1865.0f, B6 = 1976.0f,

                C7 = 2093.0f, Db7 = 2217.0f, D7 = 2349.0f, Eb7 = 2489.0f,
                E7 = 2637.0f, F7 = 2794.0f, Gb7 = 2960.0f, G7 = 3136.0f,
                Ab7 = 3322.0f, A7 = 3520.0f, Bb7 = 3729.0f, B7 = 3951.0f,

                C8 = 4186.0f, Db8 = 4435.0f, D8 = 4699.0f, Eb8 = 4978.0f,
                E8 = 5274.0f, F8 = 5588.0f, Gb8 = 5920.0f, G8 = 6272.0f,
                Ab8 = 6645.0f, A8 = 7040.0f, Bb8 = 7459.0f, B8 = 7902.0f;

    // clang-format off
    const std::array<float, 12 * 8> ALL_NOTES = {
        C1, Db1, D1, Eb1, E1, F1, Gb1, G1, Ab1, A1, Bb1, B1,
        C2, Db2, D2, Eb2, E2, F2, Gb2, G2, Ab2, A2, Bb2, B2,
        C3, Db3, D3, Eb3, E3, F3, Gb3, G3, Ab3, A3, Bb3, B3,
        C4, Db4, D4, Eb4, E4, F4, Gb4, G4, Ab4, A4, Bb4, B4,
        C5, Db5, D5, Eb5, E5, F5, Gb5, G5, Ab5, A5, Bb5, B5,
        C6, Db6, D6, Eb6, E6, F6, Gb6, G6, Ab6, A6, Bb6, B6,
        C7, Db7, D7, Eb7, E7, F7, Gb7, G7, Ab7, A7, Bb7, B7,
        C8, Db8, D8, Eb8, E8, F8, Gb8, G8, Ab8, A8, Bb8, B8
    };
    // clang-format on

    // clang-format off
    const std::array<std::string, 12 * 8> ALL_NOTES_STRINGS = {
        "C1", "Db1", "D1", "Eb1", "E1", "F1", "Gb1", "G1", "Ab1", "A1", "Bb1", "B1",
        "C2", "Db2", "D2", "Eb2", "E2", "F2", "Gb2", "G2", "Ab2", "A2", "Bb2", "B2",
        "C3", "Db3", "D3", "Eb3", "E3", "F3", "Gb3", "G3", "Ab3", "A3", "Bb3", "B3",
        "C4", "Db4", "D4", "Eb4", "E4", "F4", "Gb4", "G4", "Ab4", "A4", "Bb4", "B4",
        "C5", "Db5", "D5", "Eb5", "E5", "F5", "Gb5", "G5", "Ab5", "A5", "Bb5", "B5",
        "C6", "Db6", "D6", "Eb6", "E6", "F6", "Gb6", "G6", "Ab6", "A6", "Bb6", "B6",
        "C7", "Db7", "D7", "Eb7", "E7", "F7", "Gb7", "G7", "Ab7", "A7", "Bb7", "B7",
        "C8", "Db8", "D8", "Eb8", "E8", "F8", "Gb8", "G8", "Ab8", "A8", "Bb8", "B8"
    };
    // clang-format on

    std::string hz_to_note (float frequency)
    {
        auto beg = ALL_NOTES.begin(),
             end = ALL_NOTES.end();
        size_t index = std::distance (beg, std::min_element (beg, end, [frequency] (float a, float b) {
            return std::abs (a - frequency) < std::abs (b - frequency);
        }));
        return ALL_NOTES_STRINGS[index];
    }
}
