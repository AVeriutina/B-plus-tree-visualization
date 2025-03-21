#pragma once

#include "GeomTree.h"
namespace BPTree::GeomModel {

namespace Settings {
constexpr double WidthKey = 8.;
constexpr double DistBetweenKeys = 1.;
constexpr double DistKeyBoard = 1.;
constexpr double HeightNode = 2 * DistKeyBoard + WidthKey;
constexpr double HeightBetweenNodes = 3.;
constexpr double WidthBetweenNodes = 3.;
}  // namespace Settings

enum Pallet {
  // TODO
};

}  // namespace BPTree::GeomModel