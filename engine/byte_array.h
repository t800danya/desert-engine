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

#ifndef ENGINE_BYTE_ARRAY_H_
#define ENGINE_BYTE_ARRAY_H_

#include "arctic_types.h"

namespace arctic {

class ByteArray {
 private:
    Ui64 allocated_size_;
    Ui64 size_;
    Ui8 *data_;

 public:
    ByteArray();
    explicit ByteArray(Ui64 size);
    ~ByteArray();
    void* GetVoidData() const;
    Ui8* data() const;
    Ui64 size() const;
    void Resize(Ui64 size);
    void Reserve(Ui64 size);
};

}  // namespace arctic

#endif  // ENGINE_BYTE_ARRAY_H_
