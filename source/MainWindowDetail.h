namespace BPT::MainWindowDetails {
static constexpr double kWidthScene = 1200;
static constexpr double kHeightScene = 1000;
static constexpr double kFramesForScene = 4;
static constexpr double kTotalWidthScene = kWidthScene + kFramesForScene;
static constexpr double kTotalHeightScene = kHeightScene + kFramesForScene;

static constexpr double kDistBetweenItems = 10;
static constexpr double kDistBetweenButtons = 15;
static constexpr double kHeightButton = 40;
static constexpr double kWidthButton = 180;
static constexpr double kStartHeightOfDegreeAndSpeed =
    kDistBetweenItems + 4 * kHeightButton + 3 * kDistBetweenButtons + 350;
static constexpr double kStartXOfButton =
    2 * kDistBetweenItems + kTotalWidthScene;
static constexpr double kTotalWidth =
    kStartXOfButton + kWidthButton + kDistBetweenItems;
static constexpr double kTotalHeight =
    2 * kDistBetweenItems + kTotalHeightScene;

static constexpr int MinDegreeOfTree = 3;
static constexpr int MaxDegreeOfTree = 10;

static constexpr int MinAnimationSpeed = 100;
static constexpr int MaxAnimationSpeed = 2000;
static constexpr int StartAnimationSpeed = 1600;
}  // namespace BPT::MainWindowDetails
