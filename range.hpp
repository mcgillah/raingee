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

#include <functional>
#include <limits>

#include "emptyassert.hpp"
#include "predicates.hpp"
#include "rangeimpl.hpp"

namespace NRaingee
{
    template <class TType, class TAssert = TEmptyAssert>
    class TRange
    {
        IRangeImpl<TType>* Impl_;

        template <unsigned N>
        struct TIntToType
        {
        };

        template <class TInputIterator>
        static inline IRangeImpl<TType>* MakeImpl(TInputIterator first,
            TInputIterator last, TIntToType<0>)
        {
            if (first == last)
            {
                return 0;
            }
            else
            {
                return new TSequenceRangeImpl<TType>(first, last);
            }
        }

    public:
        typedef typename TSequenceRangeImpl<TType>::TSizeType_ TSizeType_;

    private:
        static inline IRangeImpl<TType>* MakeImpl(TSizeType_ size,
            const TType& value, TIntToType<1>)
        {
            return size ? new TSequenceRangeImpl<TType>(size, value) : 0;
        }

    public:
        inline explicit TRange(IRangeImpl<TType>* impl = 0)
            : Impl_(impl)
        {
        }

        inline TRange(const TRange& range)
            : Impl_(range.IsEmpty() ? 0 : range.Impl_->Clone())
        {
        }

        inline TRange(TSizeType_ size, const TType& value)
            : Impl_(MakeImpl(size, value, TIntToType<1>()))
        {
        }

        template <class TInputIterator>
        inline TRange(TInputIterator first, TInputIterator last)
            : Impl_(MakeImpl(first, last,
                TIntToType<std::numeric_limits<TInputIterator>::is_integer>()))
        {
        }

        inline explicit TRange(const TType& value)
            : Impl_(new TSingleValueRangeImpl<TType>(value))
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
            IRangeImpl<TType>* tmp = Impl_;
            Impl_ = range.Impl_;
            range.Impl_ = tmp;
        }

        inline void Shrink()
        {
            if (Impl_)
            {
                if (Impl_->IsEmpty())
                {
                    delete Impl_;
                    Impl_ = 0;
                }
                else
                {
                    Impl_ = new TSequenceRangeImpl<TType>(Impl_);
                }
            }
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
        inline TRange& Complement(TRange range, TCompare compare)
        {
            if (!IsEmpty() && !range.IsEmpty())
            {
                Impl_ = new TComplementedRangesImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator -=(TRange range)
        {
            return Complement(TRange(range.Release()), std::less<TType>());
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

        inline TRange& operator |=(TRange range)
        {
            return Unite(TRange(range.Release()), std::less<TType>());
        }

        template <class TCompare>
        inline TRange& Intersect(TRange range, TCompare compare)
        {
            if (IsEmpty() || range.IsEmpty())
            {
                *this = TRange();
            }
            else
            {
                Impl_ = new TIntersectedRangesImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator &=(TRange range)
        {
            return Intersect(TRange(range.Release()), std::less<TType>());
        }

        template <class TCompare>
        inline TRange& SymmetricDifference(TRange range, TCompare compare)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TSymmetricDifferenceImpl<TType, TCompare>(Impl_,
                    range.Release(), compare);
            }
            return *this;
        }

        inline TRange& operator ^=(TRange range)
        {
            return SymmetricDifference(TRange(range.Release()),
                std::less<TType>());
        }

        template <class TCompare>
        inline TRange& Unique(TCompare compare)
        {
            if (IsEmpty())
            {
                *this = TRange();
            }
            else
            {
                Impl_ = new TUniqueRangeImpl<TType, TCompare>(Impl_, compare);
            }
            return *this;
        }

        inline TRange& Unique()
        {
            return Unique(std::equal_to<TType>());
        }
    };

    template <class TType, class TAssert, class TCounter>
    static inline TRange<TType, TAssert> operator *(TRange<TType, TAssert> lhs,
        TCounter counter)
    {
        lhs *= counter;
        return TRange<TType, TAssert>(lhs.Release());
    }

    template <class TType, class TAssert>
    static inline TRange<TType, TAssert> operator +(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        lhs += TRange<TType, TAssert>(rhs.Release());
        return TRange<TType, TAssert>(lhs.Release());
    }

    template <class TType, class TAssert>
    static inline TRange<TType, TAssert> operator -(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        lhs -= TRange<TType, TAssert>(rhs.Release());
        return TRange<TType, TAssert>(lhs.Release());
    }

    template <class TType, class TAssert>
    static inline TRange<TType, TAssert> operator |(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        lhs |= TRange<TType, TAssert>(rhs.Release());
        return TRange<TType, TAssert>(lhs.Release());
    }

    template <class TType, class TAssert>
    static inline TRange<TType, TAssert> operator &(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        lhs &= TRange<TType, TAssert>(rhs.Release());
        return TRange<TType, TAssert>(lhs.Release());
    }

    template <class TType, class TAssert>
    static inline TRange<TType, TAssert> operator ^(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        lhs ^= TRange<TType, TAssert>(rhs.Release());
        return TRange<TType, TAssert>(lhs.Release());
    }

    template <class TType, class TAssert, class TCompare>
    static inline bool Equal(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs, TCompare compare)
    {
        while(!(lhs.IsEmpty() || rhs.IsEmpty()))
        {
            if (!compare(lhs.Front(), rhs.Front()))
            {
                return false;
            }
            lhs.Pop();
            rhs.Pop();
        }
        return lhs.IsEmpty() && rhs.IsEmpty();
    }

    template <class TType, class TAssert>
    static inline bool operator ==(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        return Equal(TRange<TType, TAssert>(lhs.Release()),
            TRange<TType, TAssert>(rhs.Release()),
            std::equal_to<TType>());
    }

    template <class TType, class TAssert>
    static inline bool operator !=(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)
    {
        return !(TRange<TType, TAssert>(lhs.Release())
            == TRange<TType, TAssert>(rhs.Release()));
    }

    template <class TType, class TAssert, class TCompare>
    static inline bool Includes(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs, TCompare compare)
    {
        while (!(lhs.IsEmpty() || rhs.IsEmpty()))
        {
            if (compare(lhs.Front(), rhs.Front()))
            {
                lhs.Pop();
            }
            else if (compare(rhs.Front(), lhs.Front()))
            {
                return false;
            }
            else
            {
                lhs.Pop();
                rhs.Pop();
            }
        }
        return rhs.IsEmpty();
    }

    template <class TType, class TAssert>
    static inline bool Includes(TRange<TType, TAssert> lhs,
        TRange<TType, TAssert> rhs)

    {
        typedef TRange<TType, TAssert> TRangeType;
        return Includes(TRangeType(lhs.Release()), TRangeType(rhs.Release()),
            std::less<TType>());
    }

    template <class TType, class TAssert, class TCompare>
    static inline TRange<TType, TAssert> Unique(TRange<TType, TAssert> range,
        TCompare compare)
    {
        range.Unique(compare);
        return TRange<TType, TAssert>(range.Release());
    }

    template <class TType, class TAssert>
    static inline TRange<TType, TAssert> Unique(TRange<TType, TAssert> range)
    {
        return Unique(TRange<TType, TAssert>(range.Release()),
            std::equal_to<TType>());
    }

    template <class TType, class TAssert>
    static inline typename TRange<TType, TAssert>::TSizeType_ Size(
        TRange<TType, TAssert> range)
    {
        typedef typename TRange<TType, TAssert>::TSizeType_ TSizeType;
        TSizeType result = TSizeType();
        while (!range.IsEmpty())
        {
            ++result;
            range.Pop();
        }
        return result;
    }

    template <class TType, class TAssert>
    static inline void swap(TRange<TType, TAssert>& lhs,
        TRange<TType, TAssert>& rhs)
    {
        lhs.Swap(rhs);
    }
}

#endif

