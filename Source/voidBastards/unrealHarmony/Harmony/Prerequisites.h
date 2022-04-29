#pragma once
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <memory>

#define UNREAL_HARMONY

#ifdef UNREAL_HARMONY
#include "CoreMinimal.h"
#endif


namespace Harmony{

class Vector2f;
class Pawn;
class Controller;
class State;
class Message;
class BlackBoard;
class Transition;
class Detector;

using std::vector;
using std::map;
using std::pair;
using std::string;
using uint = unsigned int;

#ifdef UNREAL_HARMONY
using Dimencion = FVector;
#else
using Dimencion = Vector2f;
#endif

inline float
size(const Dimencion& vector){

#ifdef UNREAL_HARMONY
return vector.Size();
#else
return vector.magnitud();
#endif

}

}