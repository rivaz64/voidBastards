#pragma once
#include "Prerequisites.h"
#include "Vector2f.h"

namespace Harmony{

inline
void 
print(const string& v){
  std::cout<<v<<std::endl;
}

inline
void 
print(const uint v){
  std::cout<<v<<std::endl;
}

inline
void 
print(const int v){
  std::cout<<v<<std::endl;
}

inline
void 
print(const float v){
  std::cout<<v<<std::endl;
}

inline
void 
print(const Vector2f& v){
  std::cout<<v.x<<" "<<v.y<<std::endl;
}

}