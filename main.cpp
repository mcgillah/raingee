#include <iostream>

#include "range.hpp"

using namespace NRaingee;

template <class TType>
void PrintRange(TRange<TType> range)
{
    while(!range.IsEmpty())
    {
        std::cout << range.Front() << ' ';
        range.Pop();
    }
    std::cout << std::endl;
}

int main()
{
    int a[] = {1, 3, 5, 7, 9};
    TRange<int> r(a, a + sizeof(a) / sizeof(a[0]));
    PrintRange(r);
    PrintRange(r + r);
    PrintRange(r - r);
    PrintRange(r * 3);
    PrintRange(r * 0);
    int b[] = {4, 5, 6, 7};
    TRange<int> r2(b, b + sizeof(b) / sizeof(b[0]));
    PrintRange(r | r2);
    PrintRange(r & r2);
    PrintRange(r - r2);
}

/* Expected output:
 * 1 3 5 7 9
 * 1 3 5 7 9 1 3 5 7 9
 *
 * 1 3 5 7 9 1 3 5 7 9 1 3 5 7 9
 *
 * 1 3 4 5 6 7 9
 * 5 7
 * 1 3 9
 */

