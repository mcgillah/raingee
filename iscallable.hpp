/*
 * iscallable.hpp           -- nullary callable objects detection
 *
 * Copyright (C) 2012 Dmitry Potapov <potapov.d@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __ISCALLABLE_HPP_2012_02_22__
#define __ISCALLABLE_HPP_2012_02_22__

namespace NRaingee
{
    template <class TFunctor>
    class TIsCallable
    {
        template <unsigned N>
        class TIntToType
        {
            TIntToType();
        };

        template <class T>
        static char Test(TIntToType<sizeof((*((T*)0))())>*);

        typedef char TTwoChars[2];
        template <class>
        static TTwoChars& Test(...);

    public:
        static const bool Value_ = sizeof(Test<TFunctor>(0)) == 1;
    };
}

#endif

