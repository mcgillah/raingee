/*
 * range.hpp                -- range containter
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

#ifndef __RANGE_HPP_2012_01_31__
#define __RANGE_HPP_2012_01_31__

#include <algorithm>
#include <functional>

#include "emptyassert.hpp"
#include "predicates.hpp"
#include "rangeimpl.hpp"

namespace NRaingee
{
    template <class TType, class TAssert = TEmptyAssert>
    class TRange
    {
        IRangeImpl<TType>* Impl_;

    public:
        typedef typename TSequenceRangeImpl<TType>::TSizeType_ TSizeType_;

        inline explicit TRange(IRangeImpl<TType>* impl = 0)
            : Impl_((impl && !impl->IsEmpty()) ? impl : 0)
        {
        }

        inline TRange(const TRange& range)
            : Impl_(range.IsEmpty() ? 0 : range.Impl_->Clone())
        {
        }

        inline explicit TRange(TSizeType_ size, const TType& value = TType())
            : Impl_(size ? new TSequenceRangeImpl<TType>(size, value) : 0)
        {
        }

        template <class TInputIterator>
        inline TRange(TInputIterator first, TInputIterator last)
            : Impl_(first == last ?
                0 : new TSequenceRangeImpl<TType>(first, last))
        {
        }

        inline TRange& operator =(TRange range)
        {
            Swap(range);
            return *this;
        }

        inline ~TRange()
        {
            delete Impl_;
        }

        IRangeImpl<TType>* Release()
        {
            IRangeImpl<TType>* result = Impl_;
            Impl_ = 0;
            return result;
        }

        inline bool IsEmpty() const
        {
            return !Impl_ || Impl_->IsEmpty();
        }

        inline void Pop()
        {
            TAssert::Assert(Not(BindMember(this, &TRange::IsEmpty)),
                "TRange::Pop called on empty range");
            Impl_->Pop();
        }

        inline const TType& Front() const
        {
            TAssert::Assert(Not(BindMember(this, &TRange::IsEmpty)),
                "TRange::Front called on empty range");
            return Impl_->Front();
        }

        inline void Swap(TRange& range)
        {
            std::swap(Impl_, range.Impl_);
        }

        template <class TCounter>
        inline TRange& operator *=(TCounter counter)
        {
            if (!IsEmpty())
            {
                if (counter)
                {
                    Impl_ = new TRepeatedRangeImpl<TType, TCounter>(Impl_,
                        counter);
                }
                else
                {
                    *this = TRange();
                }
            }
            return *this;
        }

        inline TRange& operator +=(TRange range)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TConcatenatedRangesImpl<TType>(Impl_,
                    range.Release());
            }
            return *this;
        }

        template <class TCompare>
        inline TRange& Complement(const TRange& range, TCompare compare)
        {
            if (!IsEmpty() && !range.IsEmpty())
            {
                Impl_ = new TComplementedRangesImpl<TType, TCompare>(Impl_,
                    range.Impl_->Clone(), compare);
            }
            return *this;
        }

        inline TRange& operator -=(const TRange& range)
        {
            return Complement(range, std::less<TType>());
        }

        template <class TCompare>
        inline TRange& Unite(TRange range, TCompare compare)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TUnitedRangesImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator |=(const TRange& range)
        {
            return Unite(range, std::less<TType>());
        }

        template <class TCompare>
        inline TRange& Intersect(const TRange& range, TCompare compare)
        {
            if (IsEmpty() || range.IsEmpty())
            {
                *this = TRange();
            }
            else
            {
                Impl_ = new TIntersectedRangesImpl<TType, TCompare>(Impl_,
                    range.Impl_->Clone(), compare);
            }
            return *this;
        }

        inline TRange& operator &=(const TRange& range)
        {
            return Intersect(range, std::less<TType>());
        }
    };

    template <class TType, class TAssert, class TCounter>
    inline TRange<TType, TAssert> operator *(TRange<TType, TAssert> lhs,
        TCounter counter)
    {
        lhs *= counter;
        TRange<TType, TAssert> result;
        result.Swap(lhs);
        return result;
    }

    template <class TType, class TAssert>
    inline TRange<TType, TAssert> operator +(TRange<TType, TAssert> lhs,
        const TRange<TType, TAssert>& rhs)
    {
        lhs += rhs;
        TRange<TType, TAssert> result;
        result.Swap(lhs);
        return result;
    }

    template <class TType, class TAssert>
    inline TRange<TType, TAssert> operator -(TRange<TType, TAssert> lhs,
        const TRange<TType, TAssert>& rhs)
    {
        lhs -= rhs;
        TRange<TType, TAssert> result;
        result.Swap(lhs);
        return result;
    }

    template <class TType, class TAssert>
    inline TRange<TType, TAssert> operator |(TRange<TType, TAssert> lhs,
        const TRange<TType, TAssert>& rhs)
    {
        lhs |= rhs;
        TRange<TType, TAssert> result;
        result.Swap(lhs);
        return result;
    }

    template <class TType, class TAssert>
    inline TRange<TType, TAssert> operator &(TRange<TType, TAssert> lhs,
        const TRange<TType, TAssert>& rhs)
    {
        lhs &= rhs;
        TRange<TType, TAssert> result;
        result.Swap(lhs);
        return result;
    }
}

namespace std
{
    template <class TType, class TAssert>
    void swap(NRaingee::TRange<TType, TAssert>& lhs,
        NRaingee::TRange<TType, TAssert>& rhs)
    {
        lhs.Swap(rhs);
    }
}

#endif

