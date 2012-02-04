#include "range.hpp"

using namespace NRaingee;

#ifdef SPEED_TEST

const int Cycles = 1000;

template <class TType>
void Check(TRange<TType> r, const char*)
{
    while (!r.IsEmpty())
    {
        r.Front();
        r.Pop();
    }
}

#else

#include <cstdlib>

#include <iostream>
#include <sstream>

const int Cycles = 1;

template <class TType>
std::string PrintRange(TRange<TType> range)
{
    std::ostringstream str;
    while(!range.IsEmpty())
    {
        str << range.Front() << ' ';
        range.Pop();
    }
    return str.str();
}

template <class TType>
void Check(TRange<TType> r, const char* b)
{
    static int i = 0;
    ++i;
    std::string a = PrintRange(r);
    if (a != b)
    {
        std::cerr << '"' << a << "\" != \"" << b << '"' << std::endl;
        exit(i);
    }
}

#endif

int main()
{
    int a1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    int a2[] = {0};
    int a3[] = {0, 10};
    int a4[] = {0};
    int a5[] = {0, 10, 22, 27};
    int a6[] = {0, 10, 35};
    int a7[] = {0};
    int a8[] = {0, 10, 14, 19, 22, 25, 27, 32, 35, 39};
    int a9[] = {0, 10, 22, 27};
    int a10[] = {0, 10, 14, 19, 22, 25, 27, 32, 35, 39};
    int a11[] = {2, 4, 5, 7, 8, 9, 11, 12, 13};
    int a12[] = {10};
    int a13[] = {10};
    int a14[] = {10, 19, 22, 25, 27, 32, 35, 39};
    int a15[] = {7, 9, 15, 16, 17, 18};
    int a16[] = {14, 19, 27};
    int a17[] = {14};
    int a18[] = {14, 19};
    int a19[] = {14, 19, 25, 27, 35, 39};
    int a20[] = {7, 9, 13, 15, 17, 18, 20, 21};
    int a21[] = {19};
    int a22[] = {19, 27};
    int a23[] = {4, 7, 8, 9, 13, 23, 24};
    int a24[] = {22};
    int a25[] = {22};
    int a26[] = {7, 9, 13, 18, 26};
    int a27[] = {25, 39};
    int a28[] = {4, 7, 8, 9, 13, 15, 18, 21, 28, 29, 30, 31};
    int a29[] = {27};
    int a30[] = {27};
    int a31[] = {27};
    int a32[] = {27};
    int a33[] = {7, 9, 13, 33, 34};
    int a34[] = {32, 35};
    int a35[] = {32, 35};
    int a36[] = {5, 7, 9, 13, 18, 33, 34, 36, 37, 38};
    int a37[] = {35};
    int a38[] = {35};
    int a39[] = {35};
    int a40[] = {7, 9, 13, 18, 26, 40};
    int a41[] = {39};
    TRange<int> r[] = {
        TRange<int>(a1, a1 + sizeof(a1) / sizeof(a1[0])),
        TRange<int>(a2, a2 + sizeof(a2) / sizeof(a2[0])),
        TRange<int>(a3, a3 + sizeof(a3) / sizeof(a3[0])),
        TRange<int>(a4, a4 + sizeof(a4) / sizeof(a4[0])),
        TRange<int>(a5, a5 + sizeof(a5) / sizeof(a5[0])),
        TRange<int>(a6, a6 + sizeof(a6) / sizeof(a6[0])),
        TRange<int>(a7, a7 + sizeof(a7) / sizeof(a7[0])),
        TRange<int>(a8, a8 + sizeof(a8) / sizeof(a8[0])),
        TRange<int>(a9, a9 + sizeof(a9) / sizeof(a9[0])),
        TRange<int>(a10, a10 + sizeof(a10) / sizeof(a10[0])),
        TRange<int>(a11, a11 + sizeof(a11) / sizeof(a11[0])),
        TRange<int>(a12, a12 + sizeof(a12) / sizeof(a12[0])),
        TRange<int>(a13, a13 + sizeof(a13) / sizeof(a13[0])),
        TRange<int>(a14, a14 + sizeof(a14) / sizeof(a14[0])),
        TRange<int>(a15, a15 + sizeof(a15) / sizeof(a15[0])),
        TRange<int>(a16, a16 + sizeof(a16) / sizeof(a16[0])),
        TRange<int>(a17, a17 + sizeof(a17) / sizeof(a17[0])),
        TRange<int>(a18, a18 + sizeof(a18) / sizeof(a18[0])),
        TRange<int>(a19, a19 + sizeof(a19) / sizeof(a19[0])),
        TRange<int>(a20, a20 + sizeof(a20) / sizeof(a20[0])),
        TRange<int>(a21, a21 + sizeof(a21) / sizeof(a21[0])),
        TRange<int>(a22, a22 + sizeof(a22) / sizeof(a22[0])),
        TRange<int>(a23, a23 + sizeof(a23) / sizeof(a23[0])),
        TRange<int>(a24, a24 + sizeof(a24) / sizeof(a24[0])),
        TRange<int>(a25, a25 + sizeof(a25) / sizeof(a25[0])),
        TRange<int>(a26, a26 + sizeof(a26) / sizeof(a26[0])),
        TRange<int>(a27, a27 + sizeof(a27) / sizeof(a27[0])),
        TRange<int>(a28, a28 + sizeof(a28) / sizeof(a28[0])),
        TRange<int>(a29, a29 + sizeof(a29) / sizeof(a29[0])),
        TRange<int>(a30, a30 + sizeof(a30) / sizeof(a30[0])),
        TRange<int>(a31, a31 + sizeof(a31) / sizeof(a31[0])),
        TRange<int>(a32, a32 + sizeof(a32) / sizeof(a32[0])),
        TRange<int>(a33, a33 + sizeof(a33) / sizeof(a33[0])),
        TRange<int>(a34, a34 + sizeof(a34) / sizeof(a34[0])),
        TRange<int>(a35, a35 + sizeof(a35) / sizeof(a35[0])),
        TRange<int>(a36, a36 + sizeof(a36) / sizeof(a36[0])),
        TRange<int>(a37, a37 + sizeof(a37) / sizeof(a37[0])),
        TRange<int>(a38, a38 + sizeof(a38) / sizeof(a38[0])),
        TRange<int>(a39, a39 + sizeof(a39) / sizeof(a39[0])),
        TRange<int>(a40, a40 + sizeof(a40) / sizeof(a40[0])),
        TRange<int>(a41, a41 + sizeof(a41) / sizeof(a41[0]))
    };
    int tagsI[] = {4, 9, 13}; // web, it, pdf
    TRange<int> tags(tagsI, tagsI + sizeof(tagsI) / sizeof(tagsI[0]));
    for (int i = 0; i < Cycles; ++i)
    {
        TRange<int> copy(tags);
        TRange<int> commonFiles(r[copy.Front()]);
        copy.Pop();
        for (; !copy.IsEmpty(); copy.Pop())
        {
            commonFiles &= r[copy.Front()];
        }
        commonFiles.Shrink();
        Check(commonFiles, "10 22 27 ");
        TRange<int> filesTags;
        for (TRange<int> files(commonFiles); !files.IsEmpty(); files.Pop())
        {
            filesTags |= r[files.Front()];
        }
        filesTags -= tags;
        filesTags.Shrink();
        Check(filesTags, "2 5 7 8 11 12 15 18 21 23 24 28 29 30 31 ");
    }
}

