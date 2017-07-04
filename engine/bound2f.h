// The MIT License(MIT)
//
// Copyright 2015 - 2016 Inigo Quilez
// Copyright 2016 Huldra
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef ENGINE_BOUND2F_H_
#define ENGINE_BOUND2F_H_

#include <cmath>
#include "arctic_types.h"
#include "vec2f.h"

namespace arctic {
struct Bound2F {
  union {
    struct {
      float min_x;
      float max_x;
      float min_y;
      float max_y;
    };
    float element[4];
  };

  Bound2F() {}

  explicit Bound2F(float mix, float max, float miy, float may) {
    min_x = mix;
    max_x = max;
    min_y = miy;
    max_y = may;
  }

  explicit Bound2F(float infi) {
    min_x = infi;
    max_x = -infi;
    min_y = infi;
    max_y = -infi;
  }

  explicit Bound2F(float const *const v) {
    min_x = v[0];
    max_x = v[1];
    min_y = v[2];
    max_y = v[3];
  }

  explicit Bound2F(float const *const vmin, float const *const vmax) {
    min_x = vmin[0];
    max_x = vmax[0];
    min_y = vmin[1];
    max_y = vmax[1];
  }

  explicit Bound2F(const Vec2F &mi, const Vec2F &ma) {
    min_x = mi.x;
    max_x = ma.x;
    min_y = mi.y;
    max_y = ma.y;
  }

  float &operator[](Si32 i) {
    return element[i];
  }
  const float &operator[](Si32 i) const {
    return element[i];
  }
};

inline bool Contains(Bound2F const &b, Vec2F const &p) {
  if (p.x < b.min_x) {
    return false;
  }
  if (p.y < b.min_y) {
    return false;
  }
  if (p.x > b.max_x) {
    return false;
  }
  if (p.y > b.max_y) {
    return false;
  }
  return true;
}

}  // namespace arctic

#endif  // ENGINE_BOUND2F_H_
