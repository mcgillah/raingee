#ifndef __RANGE_HPP_2012_01_31__
#define __RANGE_HPP_2012_01_31__

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
        inline explicit TRange(IRangeImpl<TType>* impl = 0)
            : Impl_((impl && !impl->IsEmpty()) ? impl : 0)
        {
        }

        inline TRange(const TRange& range)
            : Impl_(range.IsEmpty() ? 0 : range.Impl_->Clone())
        {
        }

        inline explicit TRange(
            typename TSequenceRangeImpl<TType>::TSizeType_ size,
            const TType& value = TType())
            : Impl_(size ? new TSequenceRangeImpl<TType>(size, value) : 0)
        {
        }

        template <class TInputIterator>
        inline TRange(TInputIterator first, TInputIterator last)
            : Impl_(first == last ?
                0 : new TSequenceRangeImpl<TType>(first, last))
        {
        }

        inline TRange& operator =(const TRange& range)
        {
            TRange tmp(range);
            tmp.Swap(*this);
            return *this;
        }

        inline ~TRange()
        {
            delete Impl_;
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

        template <class TCounter>
        inline TRange operator *(TCounter counter)
        {
            TRange tmp(*this);
            return tmp *= counter;
        }

        inline TRange& operator +=(const TRange& range)
        {
            if (IsEmpty())
            {
                *this = range;
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TConcatenatedRangesImpl<TType>(Impl_,
                    range.Impl_->Clone());
            }
            return *this;
        }

        inline TRange operator +(const TRange& range)
        {
            TRange tmp(*this);
            return tmp += range;
        }

        template <class TCompare>
        inline TRange& Unite(const TRange& range, TCompare compare)
        {
            if (IsEmpty())
            {
                *this = range;
            }
            else if (!range.IsEmpty())
            {
                Impl_ = new TUnitedRangesImpl<TType, TCompare>(Impl_,
                    range.Impl_->Clone(), compare);
            }
            return *this;
        }

        inline TRange& operator |=(const TRange& range)
        {
            return Unite(range, std::less<TType>());
        }

        inline TRange operator |(const TRange& range)
        {
            TRange tmp(*this);
            return tmp |= range;
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

        inline TRange operator &(const TRange& range)
        {
            TRange tmp(*this);
            return tmp &= range;
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

        inline TRange operator -(const TRange& range)
        {
            TRange tmp(*this);
            return tmp -= range;
        }
    };
}

#endif

