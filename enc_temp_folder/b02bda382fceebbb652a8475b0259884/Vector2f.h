/**
 * @file oaVector2f.h
 * @author Rivaz (idv19c.rrivera@uartesdigitales.edu.mx)
 * @date 9/07/2021
 */

#pragma once

#define FORCEINLINE inline
#include <cmath>

namespace Harmony {
/**
 * @brief bidimencional vector of floats
*/
class Vector2f
{
 public:
  /**
   * @brief default constructor for the vector2f
  */
  Vector2f() = default;

  /**
   * @brief constructor with parameters
   * @param _x
   * @param _y
  */
  Vector2f(float _x, float _y) :x(_x), y(_y) {}

  ~Vector2f() = default;

  /**
   * @brief compares if two vectors are equal
   * @param v 
   * @return 
  */
  FORCEINLINE bool const
  operator==(const Vector2f& v) const {
    return x == v.x && y == v.y;
  }

  /**
  * @brief adds the vector v and this one
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  operator+(const Vector2f& v) const {
    return { x + v.x , y + v.y };
  }

  /**
  * @brief adds the float v and this one
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  operator+(float v) const {
    return { x + v , y + v };
  }

  /**
   * @brief adds the vector v to this one
   * @param v
  */
  FORCEINLINE Vector2f&
  operator+=(const Vector2f& v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  /**
   * @brief this vector minus vector v
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  operator-(const Vector2f& v) const {
    return { x - v.x , y - v.y };
  }

  /**
  * @brief minus the float v and this one
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  operator-(float v) const {
    return { x - v , y - v };
  }

  /**
   * @brief substract vector v from this
   * @param v
  */
  FORCEINLINE Vector2f&
  operator-=(const Vector2f& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  /**
   * @brief calculates a vector with the same direction as this and a lenght multiplied by v
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  operator*(float v) const {
    return { x * v , y * v };
  }

  /**
   * @brief multiply the length vector the vector by v
   * @param v
  */

  FORCEINLINE Vector2f&
  operator*=(float v) {
    x *= v;
    y *= v;
    return *this;
  }

  /**
   * @brief calculates a vector with the same direction as this and a lenght divided by v
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  operator/(float v) const {
    v = 1.f / v;
    return { x * v , y * v };
  }

  /**
   * @brief divide the length vector the vector by v
   * @param v
  */
  FORCEINLINE Vector2f&
  operator/=(float v) {
    v = 1.f / v;
    x *= v;
    y *= v;
    return *this;
  }

  /**
   * @brief multiplies each parameter for its congruent in the other vector
   * @param v 
   * @return 
  */
  FORCEINLINE Vector2f
  operator*(const Vector2f& v) {
    return { x * v.x,y * v.y };
  }

  /**
   * @brief the lenght of the vector
   * @return
  */
  FORCEINLINE float const
  magnitud() const {
    return std::sqrt(x * x + y * y);
  }

  /**
   * @brief the unitary form of the vector
   * @return
  */
  FORCEINLINE Vector2f const
  normalized() const {
    return *this / sqrt(x * x + y * y);
  }

  /**
  * @brief makes this vector have a magnitud of 1
  * @return
  */
  FORCEINLINE void
  normalize() {
    *this  /= sqrt(x * x + y * y);
  }

  /**
   * @brief proyects this vector into v
   * @param v
   * @return
  */
  FORCEINLINE Vector2f const
  project(const Vector2f& v) const {
    return v * (dot(*this,v) / dot(v,v));
  }

  /**
   * @brief the direction in radians of the vector
   * @return
  */
  FORCEINLINE float const
  getDirection() const {
    return atan2(x, y);
  }

  /**
   * @brief changes the direction of the vector to r in radians and the lenght stays the same
   * @return
  */
  FORCEINLINE void 
  setDirection(float r) {
    float l = magnitud();
    x = l * cos(r);
    y = l * sin(r);
  }

  /**
   * @brief dot product of two vectors
   * @param v1
   * @param v2
   * @return
  */
  FORCEINLINE static float
  dot(const Vector2f& v1, const Vector2f& v2) {
    return v1.x * v2.x + v1.y * v2.y;
  }

 public:

  static const Vector2f ZERO;

  /**
   * @brief the x component of the vector
  */
  float x;

  /**
   * @brief the y component of the vector
  */
  float y;

};


}
