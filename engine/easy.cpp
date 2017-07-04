// The MIT License(MIT)
//
// Copyright 2017 Huldra
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

#include "easy.h"

#include <algorithm>
#include <chrono>  // NOLINT
#include <limits>
#include <thread>  // NOLINT
#include <utility>

#include "arctic_platform.h"

namespace arctic {
namespace easy {

static Ui32 g_key_state[kKeyCount] = {0};
static Engine *g_engine = nullptr;
static Vec2Si32 g_mouse_pos_prev = Vec2Si32(0, 0);
static Vec2Si32 g_mouse_pos = Vec2Si32(0, 0);
static Vec2Si32 g_mouse_move = Vec2Si32(0, 0);
static Si32 g_mouse_wheel_delta = 0;

void DrawLine(Vec2Si32 a, Vec2Si32 b, Rgba color) {
    DrawLine(a, b, color, color);
}

void DrawLine(Vec2Si32 a, Vec2Si32 b, Rgba color_a, Rgba color_b) {
    Vec2Si32 ab = b - a;
    Vec2Si32 abs_ab(std::abs(ab.x), std::abs(ab.y));
    if (abs_ab.x >= abs_ab.y) {
        if (a.x > b.x) {
            DrawLine(b, a, color_b, color_a);
        } else {
            Sprite back = GetEngine()->GetBackbuffer();
            Vec2Si32 back_size = back.Size();
            if (ab.x == 0) {
                if (a.x >= 0 && a.x < back_size.x &&
                        a.y >= 0 && a.y < back_size.y) {
                    back.RgbaData()[a.x + a.y * back.StridePixels()] = color_a;
                }
                return;
            }
            Si32 x1 = std::max(0, a.x);
            Si32 x2 = std::min(back_size.x - 1, b.x);
            Si32 y1 = a.y + ab.y * (x1 - a.x) / ab.x;
            Si32 y2 = a.y + ab.y * (x2 - a.x) / ab.x;
            if (y1 < 0) {
                if (y2 < 0) {
                    return;
                }
                // lower left -> upper right
                y1 = 0;
                x1 = a.x + ab.x * (y1 - a.y) / ab.y;
                x1 = std::max(0, x1);
            } else if (y1 >= back_size.y) {
                if (y2 >= back_size.y) {
                    return;
                }
                // upper left -> lower right
                y1 = back_size.y - 1;
                x1 = a.x + ab.x * (y1 - a.y) / ab.y;
                x1 = std::max(0, x1);
            }
            if (y2 < 0) {
                // upper left -> lower right
                y2 = 0;
                x2 = a.x + ab.x * (y2 - a.y) / ab.y;
                x2 = std::min(back_size.x - 1, x2);
            } else if (y2 >= back_size.y) {
                // lower left -> upper right
                y2 = back_size.y - 1;
                x2 = a.x + ab.x * (y2 - a.y) / ab.y;
                x2 = std::min(back_size.x - 1, x2);
            }

            Vec4Si32 rgba_a(
                static_cast<Si32>(color_a.r),
                static_cast<Si32>(color_a.g),
                static_cast<Si32>(color_a.b),
                static_cast<Si32>(color_a.a));
            Vec4Si32 rgba_b(
                static_cast<Si32>(color_b.r),
                static_cast<Si32>(color_b.g),
                static_cast<Si32>(color_b.b),
                static_cast<Si32>(color_b.a));
            Vec4Si32 rgba_ab = rgba_b - rgba_a;
            Vec4Si32 rgba_1 = rgba_a + rgba_ab * (x1 - a.x) / ab.x;
            Vec4Si32 rgba_2 = rgba_a + rgba_ab * (x2 - a.x) / ab.x;
            Vec4Si32 rgba_12 = rgba_2 - rgba_1;

            if (x2 <= x1) {
                if (x2 == x1) {
                    Rgba color(rgba_1.x, rgba_1.y, rgba_1.z, rgba_1.w);
                    back.RgbaData()[x1 + y1 * back.StridePixels()] = color;
                }
                return;
            }
            Vec4Si32 rgba_16 = rgba_1 * 65536;
            Vec4Si32 rgba_12_16 = rgba_12 * 65536;
            Vec4Si32 rgba_12_16_step = rgba_12_16 / (x2 - x1);
            Si32 y_16 = y1 * 65536;
            Si32 y12_16_step = ((y2 - y1) * 65536) / (x2 - x1);
            Si32 stride = back.StridePixels();
            for (Si32 x = x1; x <= x2; ++x) {
                Rgba color(
                    rgba_16.x >> 16,
                    rgba_16.y >> 16,
                    rgba_16.z >> 16,
                    rgba_16.w >> 16);
                back.RgbaData()[x + (y_16 >> 16) * stride] = color;
                rgba_16 += rgba_12_16_step;
                y_16 += y12_16_step;
            }
        }
    } else {
        if (a.y > b.y) {
            DrawLine(b, a, color_b, color_a);
        } else {
            Sprite back = GetEngine()->GetBackbuffer();
            Vec2Si32 back_size = back.Size();
            if (ab.y == 0) {
                if (a.y >= 0 && a.y < back_size.y &&
                    a.x >= 0 && a.x < back_size.x) {
                    back.RgbaData()[a.x + a.y * back.StridePixels()] = color_a;
                }
                return;
            }
            Si32 y1 = std::max(0, a.y);
            Si32 y2 = std::min(back_size.y - 1, b.y);
            Si32 x1 = a.x + ab.x * (y1 - a.y) / ab.y;
            Si32 x2 = a.x + ab.x * (y2 - a.y) / ab.y;
            if (x1 < 0) {
                if (x2 < 0) {
                    return;
                }
                // lower left -> upper right
                x1 = 0;
                y1 = a.y + ab.y * (x1 - a.x) / ab.x;
                y1 = std::max(0, y1);
            } else if (x1 >= back_size.x) {
                if (x2 >= back_size.x) {
                    return;
                }
                // lower right -> upper left
                x1 = back_size.x - 1;
                y1 = a.y + ab.y * (x1 - a.x) / ab.x;
                y1 = std::max(0, y1);
            }
            if (x2 < 0) {
                // lower right -> upper left
                x2 = 0;
                y2 = a.y + ab.y * (x2 - a.x) / ab.x;
                y2 = std::min(back_size.y - 1, y2);
            } else if (x2 >= back_size.x) {
                // lower left -> upper right
                x2 = back_size.x - 1;
                y2 = a.y + ab.y * (x2 - a.x) / ab.x;
                y2 = std::min(back_size.y - 1, y2);
            }

            Vec4Si32 rgba_a(
                static_cast<Si32>(color_a.r),
                static_cast<Si32>(color_a.g),
                static_cast<Si32>(color_a.b),
                static_cast<Si32>(color_a.a));
            Vec4Si32 rgba_b(
                static_cast<Si32>(color_b.r),
                static_cast<Si32>(color_b.g),
                static_cast<Si32>(color_b.b),
                static_cast<Si32>(color_b.a));
            Vec4Si32 rgba_ab = rgba_b - rgba_a;
            Vec4Si32 rgba_1 = rgba_a + rgba_ab * (y1 - a.y) / ab.y;
            Vec4Si32 rgba_2 = rgba_a + rgba_ab * (y2 - a.y) / ab.y;
            Vec4Si32 rgba_12 = rgba_2 - rgba_1;

            if (y2 <= y1) {
                if (y2 == y1) {
                    Rgba color(rgba_1.y, rgba_1.x, rgba_1.z, rgba_1.w);
                    back.RgbaData()[x1 + y1 * back.StridePixels()] = color;
                }
                return;
            }
            Vec4Si32 rgba_16 = rgba_1 * 65536;
            Vec4Si32 rgba_12_16 = rgba_12 * 65536;
            Vec4Si32 rgba_12_16_step = rgba_12_16 / (y2 - y1);
            Si32 x_16 = x1 * 65536;
            Si32 x12_16_step = ((x2 - x1) * 65536) / (y2 - y1);
            Si32 stride = back.StridePixels();
            for (Si32 y = y1; y <= y2; ++y) {
                Rgba color(
                    rgba_16.x >> 16,
                    rgba_16.y >> 16,
                    rgba_16.z >> 16,
                    rgba_16.w >> 16);
                back.RgbaData()[(x_16 >> 16) + y * stride] = color;
                rgba_16 += rgba_12_16_step;
                x_16 += x12_16_step;
            }
        }
    }
}

void DrawTriangle(Vec2Si32 a, Vec2Si32 b, Vec2Si32 c, Rgba color) {
    DrawTriangle(a, b, c, color, color, color);
}

// Just like MasterBoy wrote in HUGi 17, but without subpixel
// see http://www.hugi.scene.org/online/coding/hugi%2017%20-%20cotriang.htm
// or http://www.hugi.scene.org/online/hugi17/
inline void DrawTrianglePart(Rgba *dst, Si32 stride,
        float *x1, float *x2, Vec4F *rgba_a, Vec4F *rgba_b,
        float dxdy1, float dxdy2,
        Vec4F dcdy1, Vec4F dcdy2,
        Si32 width, Si32 height,
        Si32 y1, Si32 y2) {
    Si32 y = y1;
    if (y1 < 0) {
        Si32 yc = std::min(0, y2);
        float d = static_cast<float>(yc - y);
        y = yc;
        *x1 += dxdy1 * d;
        *x2 += dxdy2 * d;
        *rgba_a += dcdy1 * d;
        *rgba_b += dcdy2 * d;
    }
    Si32 ye = std::min(height, y2);
    dst += y * stride;
    for (; y < ye; y++) {
        Si32 x1i = static_cast<Si32>(*x1);
        Si32 x2i = static_cast<Si32>(*x2);

        Si32 x12i = x2i - x1i;
        Si32 x1c = std::max(0, x1i);
        Si32 x2c = std::min(width, x2i);

        if (x2c <= x1c) {
            if (x2c == x1c && x2c < width) {
                Rgba color(Ui8(rgba_a->x),
                    Ui8(rgba_a->y),
                    Ui8(rgba_a->z),
                    Ui8(rgba_a->w));
                Rgba *p = dst + x1c;
                p->rgba = color.rgba;
            }
        } else {
            Vec4F rgba_ab = *rgba_b - *rgba_a;
            Vec4F rgba_1c = *rgba_a + rgba_ab * static_cast<float>(x1c - x1i) /
                static_cast<float>(x12i);
            Vec4F rgba_2c = *rgba_a + rgba_ab * static_cast<float>(x2c - x1i) /
                static_cast<float>(x12i);
            Vec4F rgba_1c2c = rgba_2c - rgba_1c;

            Vec4Si32 rgba_16(rgba_1c * 65536.f);
            Vec4Si32 rgba_12_16(rgba_1c2c * 65536.f);
            Vec4Si32 rgba_12_16_step = rgba_12_16 / (x2c - x1c);

            Rgba *p = dst + x1c;
            for (Si32 x = x1c; x < x2c; ++x) {
                Rgba color(
                    rgba_16.x >> 16,
                    rgba_16.y >> 16,
                    rgba_16.z >> 16,
                    rgba_16.w >> 16);
                p->rgba = color.rgba;
                p++;
                rgba_16 += rgba_12_16_step;
            }
        }
        *x1 += dxdy1;
        *x2 += dxdy2;
        *rgba_a += dcdy1;
        *rgba_b += dcdy2;
        dst += stride;
    }
}

void DrawTriangle(Vec2Si32 a, Vec2Si32 b, Vec2Si32 c,
    Rgba color_a, Rgba color_b, Rgba color_c) {
    if (a.y > b.y) {
        std::swap(a, b);
        std::swap(color_a, color_b);
    }
    if (a.y > c.y) {
        std::swap(a, c);
        std::swap(color_a, color_c);
    }
    if (b.y > c.y) {
        std::swap(b, c);
        std::swap(color_b, color_c);
    }
    if (a.y == c.y) {
        return;
    }
    Sprite back = GetEngine()->GetBackbuffer();
    Si32 stride = back.StridePixels();
    Rgba *dst = back.RgbaData();
    Si32 width = back.Width();
    Si32 height = back.Height();

    float dxdy_ac = static_cast<float>(c.x - a.x) /
        static_cast<float>(c.y - a.y);
    float dxdy_bc = static_cast<float>(c.x - b.x) /
        static_cast<float>(c.y - b.y);
    float dxdy_ab = static_cast<float>(b.x - a.x) /
        static_cast<float>(b.y - a.y);

    Vec4F rgba_a(color_a.r, color_a.g, color_a.b, color_a.a);
    Vec4F rgba_b(color_b.r, color_b.g, color_b.b, color_b.a);
    Vec4F rgba_c(color_c.r, color_c.g, color_c.b, color_c.a);

    Vec4F dcdy_ac = (rgba_c - rgba_a) / static_cast<float>(c.y - a.y);
    Vec4F dcdy_bc = (rgba_c - rgba_b) / static_cast<float>(c.y - b.y);
    Vec4F dcdy_ab = (rgba_b - rgba_a) / static_cast<float>(b.y - a.y);

    float x1;
    float x2;
    float dxdy1;
    float dxdy2;
    Vec4F rgba1;
    Vec4F rgba2;
    Vec4F dcdy1;
    Vec4F dcdy2;

    bool is_b_at_the_right_side = dxdy_ac < dxdy_ab;
    if (is_b_at_the_right_side) {
        dxdy1 = dxdy_ac;
        dcdy1 = dcdy_ac;
        if (a.y == b.y) {
            dxdy2 = dxdy_bc;
            x1 = static_cast<float>(a.x);
            x2 = static_cast<float>(b.x);
            dcdy2 = dcdy_bc;
            rgba1 = rgba_a;
            rgba2 = rgba_b;
            DrawTrianglePart(dst, stride, &x1, &x2, &rgba1, &rgba2,
                dxdy1, dxdy2,
                dcdy1, dcdy2, width, height, a.y, c.y);
            return;
        }
        if (a.y < b.y) {
            dxdy2 = dxdy_ab;
            x1 = static_cast<float>(a.x);
            x2 = static_cast<float>(a.x);
            dcdy2 = dcdy_ab;
            rgba1 = rgba_a;
            rgba2 = rgba_a;
            DrawTrianglePart(dst, stride, &x1, &x2, &rgba1, &rgba2,
                dxdy1, dxdy2,
                dcdy1, dcdy2, width, height, a.y, b.y);
        }
        if (b.y < c.y) {
            dxdy2 = dxdy_bc;
            x2 = static_cast<float>(b.x);
            dcdy2 = dcdy_bc;
            rgba2 = rgba_b;
            DrawTrianglePart(dst, stride, &x1, &x2, &rgba1, &rgba2,
                dxdy1, dxdy2,
                dcdy1, dcdy2, width, height, b.y, c.y);
        }
    } else {
        // b is at the left side
        dxdy2 = dxdy_ac;
        dcdy2 = dcdy_ac;
        if (a.y == b.y) {
            dxdy1 = dxdy_bc;
            x1 = static_cast<float>(b.x);
            x2 = static_cast<float>(a.x);
            dcdy1 = dcdy_bc;
            rgba1 = rgba_b;
            rgba2 = rgba_a;
            DrawTrianglePart(dst, stride, &x1, &x2, &rgba1, &rgba2,
                dxdy1, dxdy2,
                dcdy1, dcdy2, width, height, a.y, c.y);
            return;
        }
        if (a.y < b.y) {
            dxdy1 = dxdy_ab;
            x1 = static_cast<float>(a.x);
            x2 = static_cast<float>(a.x);
            dcdy1 = dcdy_ab;
            rgba1 = rgba_a;
            rgba2 = rgba_a;
            DrawTrianglePart(dst, stride, &x1, &x2, &rgba1, &rgba2,
                dxdy1, dxdy2,
                dcdy1, dcdy2, width, height, a.y, b.y);
        }
        if (b.y < c.y) {
            dxdy1 = dxdy_bc;
            x1 = static_cast<float>(b.x);
            dcdy1 = dcdy_bc;
            rgba1 = rgba_b;
            DrawTrianglePart(dst, stride, &x1, &x2, &rgba1, &rgba2,
                dxdy1, dxdy2,
                dcdy1, dcdy2, width, height, b.y, c.y);
        }
    }
}

void ShowFrame() {
    GetEngine()->Draw2d();

    InputMessage message;
    g_mouse_pos_prev = g_mouse_pos;
    g_mouse_wheel_delta = 0;
    while (PopInputMessage(&message)) {
        if (message.kind == InputMessage::kKeyboard) {
            g_key_state[message.keyboard.key] = message.keyboard.key_state;
        } else if (message.kind == InputMessage::kMouse) {
            Vec2Si32 pos = GetEngine()->MouseToBackbuffer(message.mouse.pos);
            g_mouse_pos = pos;
            g_mouse_wheel_delta += message.mouse.wheel_delta;
            if (message.keyboard.key != kKeyCount) {
                g_key_state[message.keyboard.key] = message.keyboard.key_state;
            }
        }
    }
    g_mouse_move = g_mouse_pos - g_mouse_pos_prev;
}

bool IsKeyImpl(Ui32 key_code) {
    if (key_code >= kKeyCount) {
        return false;
    }
    return ((g_key_state[key_code] & 1) != 0);
}

bool IsKey(const KeyCode key_code) {
    return IsKeyImpl(static_cast<Ui32>(key_code));
}

bool IsKey(const char *keys) {
    for (const char *key = keys; *key != 0; ++key) {
        if (IsKey(*key)) {
            return true;
        }
    }
    return false;
}

bool IsKey(const char key) {
    if (key >= 'a' && key <= 'z') {
        return IsKeyImpl(static_cast<Ui32>(key)
            + static_cast<Ui32>('A')
            - static_cast<Ui32>('a'));
    }
    return IsKeyImpl(static_cast<Ui32>(key));
}

bool IsKey(const std::string &keys) {
    return IsKey(keys.c_str());
}

void SetKeyImpl(Ui32 key_code, bool is_pressed) {
	if (key_code < kKeyCount) {
		auto& k = g_key_state[key_code];
		if (is_pressed) {
			k = k | 0x1;
		}
		else {
			k = k & 0xfffffffe;
		}
	}
}

void SetKey(const KeyCode key_code, bool is_pressed) {
	SetKeyImpl(static_cast<Ui32>(key_code), is_pressed);
}

void SetKey(const char key, bool is_pressed) {
	if (key >= 'a' && key <= 'z') {
		SetKeyImpl(static_cast<Ui32>(key)
			+ static_cast<Ui32>('A')
			- static_cast<Ui32>('a'), is_pressed);
	}
	return SetKeyImpl(static_cast<Ui32>(key), is_pressed);
}

Vec2Si32 MousePos() {
    return g_mouse_pos;
}

Vec2Si32 MouseMove() {
    return g_mouse_move;
}

Si32 MouseWheelDelta() {
    return g_mouse_wheel_delta;
}

Vec2Si32 ScreenSize() {
    return GetEngine()->GetBackbuffer().Size();
}

void ResizeScreen(const Si32 width, const Si32 height) {
    GetEngine()->ResizeBackbuffer(width, height);
    return;
}

void Clear() {
    GetEngine()->GetBackbuffer().Clear();
}

void Clear(Rgba color) {
    GetEngine()->GetBackbuffer().Clear(color);
}

double Time() {
    return GetEngine()->GetTime();
}

void Sleep(double duration_seconds) {
    SleepSeconds(duration_seconds);
/*    if (duration_seconds <= 0.0) {
        return;
    }
    double usec = duration_seconds * 1000000.0;
    double limit = nexttoward(std::numeric_limits<Si64>::max(), 0ll);
    Check(usec < limit, "Sleep duration is too long");
    Si64 usec_int = static_cast<Si64>(usec);
    std::chrono::duration<Si64, std::micro> dur(usec_int);
    std::this_thread::sleep_for(dur);*/
}

std::vector<Ui8> ReadFile(const char *file_name) {
    return ReadWholeFile(file_name);
}

void WriteFile(const char *file_name, const Ui8 *data, const Ui64 data_size) {
    WriteWholeFile(file_name, data, data_size);
}

Engine *GetEngine() {
    if (!g_engine) {
        g_engine = new Engine();
    }
    return g_engine;
}

}  // namespace easy
}  // namespace arctic
