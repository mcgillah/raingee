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
#include <iterator>
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

        inline void Clear()
        {
            delete Impl_;
            Impl_ = 0;
        }

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

        inline TType Front() const
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
                    Clear();
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
                if (!counter)
                {
                    Clear();
                }
                else
                {
                    Impl_ = new TRepeatedRangeImpl<TType, TCounter>(Impl_,
                        counter);
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
                Impl_ = new TConcatenatedRangesImpl<TType>(Impl_, range);
            }
            return *this;
        }

        template <class TCompare>
        inline void Complement(TRange range, TCompare compare)
        {
            if (!IsEmpty() && !range.IsEmpty())
            {
                Impl_ = new TComplementedRangesImpl<TType, TCompare>(Impl_,
                    range, compare);
            }
        }

        inline TRange& operator -=(TRange range)
        {
            Complement(TRange(range.Release()), std::less<TType>());
            return *this;
        }

        template <class TCompare>
        inline void Unite(TRange range, TCompare compare)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TUnitedRangesImpl<TType, TCompare>(Impl_, range,
                    compare);
            }
        }

        inline TRange& operator |=(TRange range)
        {
            Unite(TRange(range.Release()), std::less<TType>());
            return *this;
        }

        template <class TCompare>
        inline void Intersect(TRange range, TCompare compare)
        {
            if (!IsEmpty())
            {
                if(range.IsEmpty())
                {
                    Clear();
                }
                else
                {
                    Impl_ = new TIntersectedRangesImpl<TType, TCompare>(Impl_,
                        range, compare);
                }
            }
        }

        inline TRange& operator &=(TRange range)
        {
            Intersect(TRange(range.Release()), std::less<TType>());
            return *this;
        }

        template <class TCompare>
        inline void SymmetricDifference(TRange range, TCompare compare)
        {
            if (IsEmpty())
            {
                Swap(range);
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TSymmetricDifferenceImpl<TType, TCompare>(Impl_,
                    range, compare);
            }
        }

        inline TRange& operator ^=(TRange range)
        {
            SymmetricDifference(TRange(range.Release()), std::less<TType>());
            return *this;
        }

        template <class TCompare>
        inline void Unique(TCompare compare)
        {
            if (!IsEmpty())
            {
                Impl_ = new TUniqueRangeImpl<TType, TCompare>(Impl_, compare);
            }
        }

        inline void Unique()
        {
            Unique(std::equal_to<TType>());
        }

        template <class TPredicate>
        inline void Remove(TPredicate predicate)
        {
            if (!IsEmpty())
            {
                Impl_ = new TRemoveImpl<TType, TPredicate>(Impl_, predicate);
            }
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

    template <class TType, class TAssert, class TPredicate>
    static inline TRange<TType, TAssert> Remove(TRange<TType, TAssert> range,
        TPredicate predicate)
    {
        range.Remove(predicate);
        return TRange<TType, TAssert>(range.Release());
    }

    template <class TType, class TOldType, class TAssert, class TUnaryOp>
    static inline TRange<TType, TAssert> Transform(
        TRange<TOldType, TAssert> range, TUnaryOp op)
    {
        TRange<TType, TAssert> result;
        if (!range.IsEmpty())
        {
            TRange<TType, TAssert>(
                new TTransformedRangeImpl<TType, TOldType, TUnaryOp>(range,
                    op)).Swap(result);
        }
        return result;
    }

    template <class TType, class TFirstType, class TSecondType, class TAssert,
        class TBinaryOp>
    static inline TRange<TType, TAssert> Transform(
        TRange<TFirstType, TAssert> first,
        TRange<TSecondType, TAssert> second,
        TBinaryOp op)
    {
        TRange<TType, TAssert> result;
        if (!(first.IsEmpty() || second.IsEmpty()))
        {
            TRange<TType, TAssert>(new TTransformedRangesImpl<TType,
                TFirstType, TSecondType, TBinaryOp>(first, second, op)).Swap(
                    result);
        }
        return result;
    }

    template <class TType, class TInserter, class TDelimiter,
        class TEscapeChar, class TOldType, class TAssert>
    static inline TRange<TType, TAssert> Split(TRange<TOldType, TAssert> range,
        TDelimiter delimiter, TEscapeChar escapeChar)
    {
        TRange<TType, TAssert> result;
        if (!range.IsEmpty())
        {
            TRange<TType, TAssert>(new TSplittedRangeImpl<TType, TInserter,
                TDelimiter, TEscapeChar, TOldType>(range, delimiter,
                    escapeChar)).Swap(result);
        }
        return result;
    }

    template <class TType, class TInserter, class TDelimiter, class TOldType,
        class TAssert>
    static inline TRange<TType, TAssert> Split(TRange<TOldType, TAssert> range,
        TDelimiter delimiter)
    {
        return Split<TType, TInserter, TDelimiter, TFakeEscapeChar, TOldType,
            TAssert>(range, delimiter, TFakeEscapeChar());
    }

    template <class TType, class TDelimiter, class TEscapeChar, class TOldType,
        class TAssert>
    static inline TRange<TType, TAssert> Split(TRange<TOldType, TAssert> range,
        TDelimiter delimiter, TEscapeChar escapeChar)
    {
        return Split<TType, std::back_insert_iterator<TType>, TDelimiter,
            TEscapeChar, TOldType, TAssert>(range, delimiter, escapeChar);
    }

    template <class TType, class TDelimiter, class TOldType, class TAssert>
    static inline TRange<TType, TAssert> Split(TRange<TOldType, TAssert> range,
        TDelimiter delimiter)
    {
        return Split<TType, TDelimiter, TFakeEscapeChar, TOldType, TAssert>(
            range, delimiter, TFakeEscapeChar());
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

    // Complex ranges c'tors
    template <class TType, class TCounter>
    template <class TAssert>
    TRepeatedRangeImpl<TType, TCounter>::TRepeatedRangeImpl(
        TRange<TType, TAssert>& range,
        TRange<TType, TAssert>& current,
        TCounter counter)
        : Range_(range.Release())
        , CurrentRange_(current.Release())
        , Counter_(counter)
    {
    }

    template <class TType, class TCounter>
    IRangeImpl<TType>* TRepeatedRangeImpl<TType, TCounter>::Clone() const
    {
        TRange<TType, TEmptyAssert> range(Range_->Clone());
        TRange<TType, TEmptyAssert> current(CurrentRange_->Clone());
        return new TRepeatedRangeImpl(range, current, Counter_);
    }

    template <class TType>
    template <class TAssert>
    TConcatenatedRangesImpl<TType>::TConcatenatedRangesImpl(
        IRangeImpl<TType>* first, TRange<TType, TAssert>& second)
        : First_(first)
        , Second_(second.Release())
        , ActiveRange_(First_)
    {
    }

    template <class TType>
    template <class TAssert>
    TConcatenatedRangesImpl<TType>::TConcatenatedRangesImpl(
        TRange<TType, TAssert>& first, TRange<TType, TAssert>& second)
        : First_(first.Release())
        , Second_(second.Release())
        , ActiveRange_(First_)
    {
    }

    template <class TType>
    IRangeImpl<TType>* TConcatenatedRangesImpl<TType>::Clone() const
    {
        if (ActiveRange_ == First_)
        {
            TRange<TType, TEmptyAssert> first(First_->Clone());
            TRange<TType, TEmptyAssert> second(Second_->Clone());
            return new TConcatenatedRangesImpl(first, second);
        }
        else
        {
            return Second_->Clone();
        }
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TComplementedRangesImpl<TType, TCompare>::TComplementedRangesImpl(
        IRangeImpl<TType>* first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first)
        , Second_(second.Release())
        , Compare_(compare)
    {
        Next();
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TComplementedRangesImpl<TType, TCompare>::TComplementedRangesImpl(
        TRange<TType, TAssert>& first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first.Release())
        , Second_(second.Release())
        , Compare_(compare)
    {
        Next();
    }

    template <class TType, class TCompare>
    IRangeImpl<TType>* TComplementedRangesImpl<TType, TCompare>::Clone() const
    {
        if (Second_->IsEmpty())
        {
            return First_->Clone();
        }
        else
        {
            TRange<TType, TEmptyAssert> first(First_->Clone());
            TRange<TType, TEmptyAssert> second(Second_->Clone());
            return new TComplementedRangesImpl(first, second, Compare_);
        }
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TUnitedRangesImpl<TType, TCompare>::TUnitedRangesImpl(
        IRangeImpl<TType>* first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first)
        , Second_(second.Release())
        , ActiveRange_(Compare_(First_->Front(), Second_->Front()) ?
            First_ : Second_)
        , Compare_(compare)
        , PopBoth_(ActiveRange_ == Second_
            && !Compare_(Second_->Front(), First_->Front()))
    {
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TUnitedRangesImpl<TType, TCompare>::TUnitedRangesImpl(
        TRange<TType, TAssert>& first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first.Release())
        , Second_(second.Release())
        , ActiveRange_(Compare_(First_->Front(), Second_->Front()) ?
            First_ : Second_)
        , Compare_(compare)
        , PopBoth_(ActiveRange_ == Second_
            && !Compare_(Second_->Front(), First_->Front()))
    {
    }

    template <class TType, class TCompare>
    IRangeImpl<TType>* TUnitedRangesImpl<TType, TCompare>::Clone() const
    {
        if (First_->IsEmpty())
        {
            return Second_->Clone();
        }
        else if (Second_->IsEmpty())
        {
            return First_->Clone();
        }
        else
        {
            TRange<TType, TEmptyAssert> first(First_->Clone());
            TRange<TType, TEmptyAssert> second(Second_->Clone());
            return new TUnitedRangesImpl(first, second, Compare_);
        }
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TIntersectedRangesImpl<TType, TCompare>::TIntersectedRangesImpl(
        IRangeImpl<TType>* first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first)
        , Second_(second.Release())
        , Compare_(compare)
    {
        Next();
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TIntersectedRangesImpl<TType, TCompare>::TIntersectedRangesImpl(
        TRange<TType, TAssert>& first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first.Release())
        , Second_(second.Release())
        , Compare_(compare)
    {
        Next();
    }

    template <class TType, class TCompare>
    IRangeImpl<TType>* TIntersectedRangesImpl<TType, TCompare>::Clone() const
    {
        TRange<TType, TEmptyAssert> first(First_->Clone());
        TRange<TType, TEmptyAssert> second(Second_->Clone());
        return new TIntersectedRangesImpl(first, second, Compare_);
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TSymmetricDifferenceImpl<TType, TCompare>::TSymmetricDifferenceImpl(
        IRangeImpl<TType>* first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first)
        , Second_(second.Release())
        , ActiveRange_(Next())
        , Compare_(compare)
    {
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TSymmetricDifferenceImpl<TType, TCompare>::TSymmetricDifferenceImpl(
        TRange<TType, TAssert>& first,
        TRange<TType, TAssert>& second,
        TCompare compare)
        : First_(first.Release())
        , Second_(second.Release())
        , ActiveRange_(Next())
        , Compare_(compare)
    {
    }

    template <class TType, class TCompare>
    IRangeImpl<TType>* TSymmetricDifferenceImpl<TType, TCompare>::Clone() const
    {
        if (First_->IsEmpty())
        {
            return Second_->Clone();
        }
        else if (Second_->IsEmpty())
        {
            return First_->Clone();
        }
        else
        {
            TRange<TType, TEmptyAssert> first(First_->Clone());
            TRange<TType, TEmptyAssert> second(Second_->Clone());
            return new TSymmetricDifferenceImpl(first, second, Compare_);
        }
    }

    template <class TType, class TCompare>
    template <class TAssert>
    TUniqueRangeImpl<TType, TCompare>::TUniqueRangeImpl(
        TRange<TType, TAssert>& range, TCompare compare)
        : Range_(range.Release())
        , Compare_(compare)
    {
    }

    template <class TType, class TCompare>
    IRangeImpl<TType>* TUniqueRangeImpl<TType, TCompare>::Clone() const
    {
        TRange<TType, TEmptyAssert> range(Range_->Clone());
        return new TUniqueRangeImpl(range, Compare_);
    }

    template <class TType, class TPredicate>
    template <class TAssert>
    TRemoveImpl<TType, TPredicate>::TRemoveImpl(
        TRange<TType, TAssert>& range, TPredicate predicate)
        : Range_(range.Release())
        , Predicate_(predicate)
    {
        Next();
    }

    template <class TType, class TPredicate>
    IRangeImpl<TType>* TRemoveImpl<TType, TPredicate>::Clone() const
    {
        TRange<TType, TEmptyAssert> range(Range_->Clone());
        return new TRemoveImpl(range, Predicate_);
    }

    template <class TType, class TOldType, class TUnaryOp>
    template <class TAssert>
    TTransformedRangeImpl<TType, TOldType, TUnaryOp>::TTransformedRangeImpl(
        TRange<TOldType, TAssert>& range, TUnaryOp op)
        : Range_(range.Release())
        , Op_(op)
    {
    }

    template <class TType, class TOldType, class TUnaryOp>
    IRangeImpl<TType>*
    TTransformedRangeImpl<TType, TOldType, TUnaryOp>::Clone() const
    {
        TRange<TOldType, TEmptyAssert> range(Range_->Clone());
        return new TTransformedRangeImpl(range, Op_);
    }

    template <class TType, class TFirstType, class TSecondType,
        class TBinaryOp>
    template <class TAssert>
    TTransformedRangesImpl<TType, TFirstType, TSecondType, TBinaryOp>::
        TTransformedRangesImpl(TRange<TFirstType, TAssert>& first,
            TRange<TSecondType, TAssert>& second, TBinaryOp op)
        : First_(first.Release())
        , Second_(second.Release())
        , Op_(op)
    {
    }

    template <class TType, class TFirstType, class TSecondType,
        class TBinaryOp>
    IRangeImpl<TType>*
    TTransformedRangesImpl<TType, TFirstType, TSecondType, TBinaryOp>::
        Clone() const
    {
        TRange<TFirstType, TEmptyAssert> first(First_->Clone());
        TRange<TSecondType, TEmptyAssert> second(Second_->Clone());
        return new TTransformedRangesImpl(first, second, Op_);
    }

    template <class TType, class TInserter, class TDelimiter,
        class TEscapeChar, class TOldType>
    template <class TAssert>
    TSplittedRangeImpl<TType, TInserter, TDelimiter, TEscapeChar, TOldType>::
        TSplittedRangeImpl(TRange<TOldType, TAssert>& range,
            TDelimiter delimiter, TEscapeChar escapeChar)
            : Range_(range.Release())
            , Inserter_(Value_)
            , Delimiter_(delimiter)
            , EscapeChar_(escapeChar)
            , Empty_(true)
    {
        Next();
    }

    template <class TType, class TInserter, class TDelimiter,
        class TEscapeChar, class TOldType>
    template <class TAssert>
    TSplittedRangeImpl<TType, TInserter, TDelimiter, TEscapeChar, TOldType>::
        TSplittedRangeImpl(TRange<TOldType, TAssert>& range, TType value,
            TDelimiter delimiter, TEscapeChar escapeChar)
            : Range_(range.Release())
            , Value_(value)
            , Inserter_(Value_)
            , Delimiter_(delimiter)
            , EscapeChar_(escapeChar)
            , Empty_(false)
    {
    }

    template <class TType, class TInserter, class TDelimiter,
        class TEscapeChar, class TOldType>
    IRangeImpl<TType>*
    TSplittedRangeImpl<TType, TInserter, TDelimiter, TEscapeChar, TOldType>::
        Clone() const
    {
        TRange<TOldType, TEmptyAssert> range(Range_->Clone());
        return new TSplittedRangeImpl(range, Value_, Delimiter_, EscapeChar_);
    }
}

#endif

