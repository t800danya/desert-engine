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

#ifndef ENGINE_VEC2F_H_
#define ENGINE_VEC2F_H_

#include <cmath>
#include "arctic_types.h"
#include "vec2si32.h"

namespace arctic {

struct Vec2F {
  union {
    struct {
      float x;
      float y;
    };
    float element[2];
  };

  Vec2F() {}

  explicit Vec2F(float a) {
    x = a;
    y = a;
  }

  explicit Vec2F(float a, float b) {
    x = a;
    y = b;
  }

  explicit Vec2F(float const *const v) {
    x = v[0];
    y = v[1];
  }

  explicit Vec2F(const Vec2Si32 &v) {
    x = static_cast<float>(v.x);
    y = static_cast<float>(v.y);
  }

  float &operator[](Si32 i) {
    return element[i];
  }
  const float &operator[](Si32 i) const {
    return element[i];
  }

  Vec2F &operator= (Vec2F const &v) {
    x = v.x;
    y = v.y;
    return *this;
  }
  Vec2F &operator+=(float const &s) {
    x += s;
    y += s;
    return *this;
  }
  Vec2F &operator+=(Vec2F const &v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  Vec2F &operator-=(float const &s) {
    x -= s;
    y -= s;
    return *this;
  }
  Vec2F &operator-=(Vec2F const &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  Vec2F &operator*=(float const &s) {
    x *= s;
    y *= s;
    return *this;
  }
  Vec2F &operator*=(Vec2F const &v) {
    x *= v.x;
    y *= v.y;
    return *this;
  }
  Vec2F &operator/=(float const &s) {
    x /= s;
    y /= s;
    return *this;
  }
  Vec2F &operator/=(Vec2F const &v) {
    x /= v.x;
    y /= v.y;
    return *this;
  }

  Vec2F xx() const {
    return Vec2F(x, x);
  }
  Vec2F xy() const {
    return Vec2F(x, y);
  }
  Vec2F yx() const {
    return Vec2F(y, x);
  }
  Vec2F yy() const {
    return Vec2F(y, y);
  }
};

inline Vec2F operator+(Vec2F const &v, float const &s) {
  return Vec2F(v.x + s, v.y + s);
}
inline Vec2F operator+(float const &s, Vec2F const &v) {
  return Vec2F(s + v.x, s + v.y);
}
inline Vec2F operator+(Vec2F const &a, Vec2F const &b) {
  return Vec2F(a.x + b.x, a.y + b.y);
}
inline Vec2F operator-(Vec2F const &v, float const &s) {
  return Vec2F(v.x - s, v.y - s);
}
inline Vec2F operator-(float const &s, Vec2F const &v) {
  return Vec2F(s - v.x, s - v.y);
}
inline Vec2F operator-(Vec2F const &a, Vec2F const &b) {
  return Vec2F(a.x - b.x, a.y - b.y);
}
inline Vec2F operator*(Vec2F const &v, float const &s) {
  return Vec2F(v.x * s, v.y * s);
}
inline Vec2F operator*(float const &s, Vec2F const &v) {
  return Vec2F(s * v.x, s * v.y);
}
inline Vec2F operator*(Vec2F const &a, Vec2F const &b) {
  return Vec2F(a.x * b.x, a.y * b.y);
}
inline Vec2F operator/(Vec2F const &v, float const &s) {
  return Vec2F(v.x / s, v.y / s);
}
inline Vec2F operator/(float const &s, Vec2F const &v) {
  return Vec2F(s / v.x, s / v.y);
}
inline Vec2F operator/(Vec2F const &a, Vec2F const &b) {
  return Vec2F(a.x / b.x, a.y / b.y);
}

inline Vec2F floor(Vec2F const &v) {
  return Vec2F(floorf(v.x), floorf(v.y));
}

inline Vec2F Normalize(Vec2F const &v) {
  float const m2 = v.x * v.x + v.y * v.y;
  float const im = 1.0f / sqrtf(m2);
  return Vec2F(v.x * im, v.y * im);
}

inline Vec2F Mix(Vec2F const &a, Vec2F const &b, float const f) {
  return Vec2F(a.x * (1.0f - f) + f * b.x,
    a.y * (1.0f - f) + f * b.y);
}

inline float Length(Vec2F const &v) {
  return sqrtf(v.x * v.x + v.y * v.y);
}

inline float Dot(Vec2F const &a, Vec2F const &b) {
  return a.x * b.x + a.y * b.y;
}

inline float Distance(Vec2F const &a, Vec2F const &b) {
  return Length(a - b);
}

inline Vec2F Perpendicular(Vec2F const &v) {
  return Vec2F(v.y, -v.x);
}

inline Vec2F FromPolar(const float a) {
  return Vec2F(cosf(a), sinf(a));
}

inline float InverseLength(Vec2F const &v) {
  return 1.0f / sqrtf(v.x * v.x + v.y * v.y);
}

inline Vec2F sin(const Vec2F &v) {
  return Vec2F(sinf(v.x), sinf(v.y));
}

inline Vec2F cos(const Vec2F &v) {
  return Vec2F(cosf(v.x), cosf(v.y));
}

inline Vec2F sqrt(const Vec2F &v) {
  return Vec2F(sqrtf(v.x), sqrtf(v.y));
}

}  // namespace arctic

#endif  // ENGINE_VEC2F_H_
