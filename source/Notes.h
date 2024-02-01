#pragma once

#include <array>
#include <string>

namespace Notes
{
    const float C1 = 32.70, Db1 = 34.65, D1 = 36.71, Eb1 = 38.89,
                E1 = 41.20, F1 = 43.65, Gb1 = 46.25, G1 = 49.00,
                Ab1 = 51.91, A1 = 55.00, Bb1 = 58.27, B1 = 61.74,

                C2 = 65.41, Db2 = 69.30, D2 = 73.42, Eb2 = 77.78,
                E2 = 82.41, F2 = 87.31, Gb2 = 92.50, G2 = 98.00,
                Ab2 = 103.8, A2 = 110.0, Bb2 = 116.5, B2 = 123.5,

                C3 = 130.8, Db3 = 138.6, D3 = 146.8, Eb3 = 155.6,
                E3 = 164.8, F3 = 174.6, Gb3 = 185.0, G3 = 196.0,
                Ab3 = 207.7, A3 = 220.0, Bb3 = 233.1, B3 = 246.9,

                C4 = 261.6, Db4 = 277.2, D4 = 293.7, Eb4 = 311.1,
                E4 = 329.6, F4 = 349.2, Gb4 = 370.0, G4 = 392.0,
                Ab4 = 415.3, A4 = 440.0, Bb4 = 466.2, B4 = 493.9,

                C5 = 523.3, Db5 = 554.4, D5 = 587.3, Eb5 = 622.3,
                E5 = 659.3, F5 = 698.5, Gb5 = 740.0, G5 = 784.0,
                Ab5 = 830.6, A5 = 880.0, Bb5 = 932.3, B5 = 987.8,

                C6 = 1047, Db6 = 1109, D6 = 1175, Eb6 = 1245,
                E6 = 1319, F6 = 1397, Gb6 = 1480, G6 = 1568,
                Ab6 = 1661, A6 = 1760, Bb6 = 1865, B6 = 1976,

                C7 = 2093, Db7 = 2217, D7 = 2349, Eb7 = 2489,
                E7 = 2637, F7 = 2794, Gb7 = 2960, G7 = 3136,
                Ab7 = 3322, A7 = 3520, Bb7 = 3729, B7 = 3951,

                C8 = 4186, Db8 = 4435, D8 = 4699, Eb8 = 4978,
                E8 = 5274, F8 = 5588, Gb8 = 5920, G8 = 6272,
                Ab8 = 6645, A8 = 7040, Bb8 = 7459, B8 = 7902;

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
