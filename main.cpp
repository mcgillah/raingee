#include <cstdlib>

#include "range.hpp"

using namespace NRaingee;

void Check(bool a)
{
    if (!a)
    {
        exit(42);
    }
}

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
    int a[] = {1, 3, 5, 7, 9};
    TRange<int> r(a, a + sizeof(a) / sizeof(a[0]));
    int b[] = {4, 5, 6, 7};
    TRange<int> r2(b, b + sizeof(b) / sizeof(b[0]));
    int c[] = {1, 2, 3, 4, 9};
    TRange<int> r3(c, c + sizeof(c) / sizeof(c[0]));
    for (int i = 0; i < Cycles; ++i)
    {
        Check(TRange<int>(2, 2), "2 2 ");
        Check(r, "1 3 5 7 9 ");
        Check(r + r, "1 3 5 7 9 1 3 5 7 9 ");
        Check(r - r, "");
        Check(r - r - r, "");
        Check(r * 3, "1 3 5 7 9 1 3 5 7 9 1 3 5 7 9 ");
        Check((r + r + r) * 2 - (r * 2 - r) * 5, "1 3 5 7 9 ");
        Check(r * 0, "");
        Check(r | r2, "1 3 4 5 6 7 9 ");
        Check((r | r2) & (r2 | r), "1 3 4 5 6 7 9 ");
        Check(((r | r2) & (r2 | r)) & r, "1 3 5 7 9 ");
        Check(r & r2, "5 7 ");
        Check(r - r2, "1 3 9 ");
        Check(r2 - r, "4 6 ");
        Check((r - r2) | (r2 - r), "1 3 4 6 9 ");
        Check((TRange<int>(5) + TRange<int>(7)) == (r & r2));
        Check(TRange<int>(5, 7), "7 7 7 7 7 ");
        Check((r - r2) != (r | r2));
        Check(Includes(r | r2, r));
        Check(!Includes((r - r2) | (r2 - r), r));
        Check(NRaingee::Includes(r, r));
        Check(r & r3, "1 3 9 ");
        Check(r3 & r, "1 3 9 ");
        Check(r & r2 & r3, "");
        Check(r | r3, "1 2 3 4 5 7 9 ");
        Check(r3 | r, "1 2 3 4 5 7 9 ");
        Check(r2 | r3, "1 2 3 4 5 6 7 9 ");
        Check(r | r2 | r3, "1 2 3 4 5 6 7 9 ");
        Check(r - r3, "5 7 ");
        Check(r3 - r, "2 4 ");
        Check(r3 - r2 - r, "2 ");
        Check(Unique(TRange<int>(5, 7)), "7 ");
        Check(Unique(r - r - r), "");
        Check((r - r3) & (r3 - r) & TRange<int>(4), "");
        Check(TRange<int>(4) & (r * 0) & TRange<int>(4), "");
        Check(Unique(TRange<int>(1) * 2 + TRange<int>(2)* 5 + TRange<int>(3)),
            "1 2 3 ");
        Check((r | r3) ^ (r2 | r3), "6 ");
        Check(r ^ r2, "1 3 4 6 9 ");
        Check((r ^ r) * 3, "");
        Check((r2 - r2) * 2, "");
        Check((r + r2 + r3) * 0, "");
        Check((r | r2 | r3) ^ (r3 | r2 | r), "");
        Check((r ^ r) | (r2 ^ r2) | (r3 ^ r3), "");
        Check(Size(r ^ r) == 0);
        Check(Size(r) == 5);
        Check(Size(r * 3) == 15);
        Check(Size(r & r2) == 2);
    }
}

