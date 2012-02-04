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
    for (int i = 0; i < Cycles; ++i)
    {
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
        Check((TRange<int>(1, 5) + TRange<int>(1, 7)) == (r & r2));
        Check((r - r2) != (r | r2));
        Check(Includes(r | r2, r));
        Check(!Includes((r - r2) | (r2 - r), r));
        Check(Includes(r, r));
    }
}

