#pragma once

namespace BPT::MainWindowDetails {
inline constexpr double kWidthScene = 1200;
inline constexpr double kHeightScene = 1000;
inline constexpr double kFramesForScene = 4;
inline constexpr double kTotalWidthScene = kWidthScene + kFramesForScene;
inline constexpr double kTotalHeightScene = kHeightScene + kFramesForScene;

inline constexpr double kDistBetweenItems = 10;
inline constexpr double kDistBetweenButtons = 15;
inline constexpr double kHeightButton = 40;
inline constexpr double kWidthButton = 180;
inline constexpr double kStartHeightOfDegreeAndSpeed =
    kDistBetweenItems + 4 * kHeightButton + 3 * kDistBetweenButtons + 350;
inline constexpr double kStartXOfButton =
    2 * kDistBetweenItems + kTotalWidthScene;
inline constexpr double kTotalWidth =
    kStartXOfButton + kWidthButton + kDistBetweenItems;
inline constexpr double kTotalHeight =
    2 * kDistBetweenItems + kTotalHeightScene;

inline constexpr int MinDegreeOfTree = 3;
inline constexpr int MaxDegreeOfTree = 10;

inline constexpr int MinAnimationSpeed = 100;
inline constexpr int MaxAnimationSpeed = 2000;
inline constexpr int StartAnimationSpeed = 1600;
}  // namespace BPT::MainWindowDetails
