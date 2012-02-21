/*
 * rangeimpl.hpp            -- range containters implementation
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

#ifndef __RANGEIMPL_HPP_2012_01_31__
#define __RANGEIMPL_HPP_2012_01_31__

#include <vector>

namespace NRaingee
{
    template <class TType, class TAssert>
    class TRange;

    template <class TType>
    class IRangeImpl
    {
        IRangeImpl(const IRangeImpl&);
        IRangeImpl& operator =(const IRangeImpl&);

    public:
        inline IRangeImpl()
        {
        }

        virtual inline ~IRangeImpl()
        {
        }

        virtual bool IsEmpty() const = 0;
        virtual void Pop() = 0 ;
        virtual TType Front() const = 0;
        virtual IRangeImpl* Clone() const = 0;
    };

    template <class TType>
    class TSequenceRangeImpl: public IRangeImpl<TType>
    {
        typedef std::vector<TType> TData_;

        class TSharedStorage_
        {
            const TData_ Data_;
            unsigned Counter_;

        public:
            inline TSharedStorage_(TData_ data)
                : Data_(data)
                , Counter_(1)
            {
            }

            inline void IncreaseCounter()
            {
                ++Counter_;
            }

            inline unsigned DecreaseCounter()
            {
                return --Counter_;
            }

            inline const TData_& GetData() const
            {
                return Data_;
            }
        };

        TSharedStorage_* const Storage_;
        typename TData_::const_iterator Begin_;
        const typename TData_::const_iterator End_;

        static TData_ ConvertToSequence(IRangeImpl<TType>* range)
        {
            TData_ result;
            while (!range->IsEmpty())
            {
                result.push_back(range->Front());
                range->Pop();
            }
            return result;
        }

        inline TSequenceRangeImpl(const TSequenceRangeImpl* range)
            : Storage_(range->Storage_)
            , Begin_(range->Begin_)
            , End_(range->End_)
        {
            Storage_->IncreaseCounter();
        }

    public:
        typedef typename TData_::size_type TSizeType_;

        inline TSequenceRangeImpl(IRangeImpl<TType>* range)
            : Storage_(new TSharedStorage_(TData_(ConvertToSequence(range))))
            , Begin_(Storage_->GetData().begin())
            , End_(Storage_->GetData().end())
        {
            delete range;
        }

        inline TSequenceRangeImpl(TSizeType_ size, const TType& value)
            : Storage_(new TSharedStorage_(TData_(size, value)))
            , Begin_(Storage_->GetData().begin())
            , End_(Storage_->GetData().end())
        {
        }

        template <class TInputIterator>
        inline TSequenceRangeImpl(TInputIterator first, TInputIterator last)
            : Storage_(new TSharedStorage_(TData_(first, last)))
            , Begin_(Storage_->GetData().begin())
            , End_(Storage_->GetData().end())
        {
        }

        inline ~TSequenceRangeImpl()
        {
            if (!Storage_->DecreaseCounter())
            {
                delete Storage_;
            }
        }

        inline bool IsEmpty() const
        {
            return Begin_ == End_;
        }

        inline void Pop()
        {
            ++Begin_;
        }

        inline TType Front() const
        {
            return *Begin_;
        }

        inline IRangeImpl<TType>* Clone() const
        {
            return new TSequenceRangeImpl(this);
        }
    };

    template <class TType>
    class TSingleValueRangeImpl: public IRangeImpl<TType>
    {
        const TType Value_;
        bool Empty_;

    public:
        inline explicit TSingleValueRangeImpl(const TType& value)
            : Value_(value)
            , Empty_(false)
        {
        }

        inline bool IsEmpty() const
        {
            return Empty_;
        }

        inline void Pop()
        {
            Empty_ = true;
        }

        inline TType Front() const
        {
            return Value_;
        }

        inline IRangeImpl<TType>* Clone() const
        {
            return new TSingleValueRangeImpl(Value_);
        }
    };

    template <class TType, class TCounter>
    class TRepeatedRangeImpl: public IRangeImpl<TType>
    {
        const IRangeImpl<TType>* const Range_;
        IRangeImpl<TType>* CurrentRange_;
        TCounter Counter_;

        template <class TAssert>
        TRepeatedRangeImpl(TRange<TType, TAssert>& range,
            TRange<TType, TAssert>& current, TCounter counter);

    public:
        inline TRepeatedRangeImpl(const IRangeImpl<TType>* range,
            TCounter counter)
            : Range_(range)
            , CurrentRange_(Range_->Clone())
            , Counter_(counter)
        {
            --Counter_;
        }

        inline ~TRepeatedRangeImpl()
        {
            delete CurrentRange_;
            delete Range_;
        }

        inline bool IsEmpty() const
        {
            return CurrentRange_->IsEmpty() && !Counter_;
        }

        inline void Pop()
        {
            CurrentRange_->Pop();
            if (CurrentRange_->IsEmpty() && !!Counter_)
            {
                delete CurrentRange_;
                CurrentRange_ = Range_->Clone();
                --Counter_;
            }
        }

        inline TType Front() const
        {
            return CurrentRange_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    struct TInfiniteCounter
    {
        inline bool operator !() const
        {
            return false;
        }

        inline TInfiniteCounter& operator --()
        {
            return *this;
        }
    };

    template <class TType>
    class TConcatenatedRangesImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const First_;
        IRangeImpl<TType>* const Second_;
        IRangeImpl<TType>* ActiveRange_;

        template <class TAssert>
        TConcatenatedRangesImpl(TRange<TType, TAssert>& first,
            TRange<TType, TAssert>& second);

    public:
        template <class TAssert>
        TConcatenatedRangesImpl(IRangeImpl<TType>* first,
            TRange<TType, TAssert>& second);

        inline ~TConcatenatedRangesImpl()
        {
            delete First_;
            delete Second_;
        }

        inline bool IsEmpty() const
        {
            return ActiveRange_->IsEmpty();
        }

        inline void Pop()
        {
            ActiveRange_->Pop();
            if (ActiveRange_->IsEmpty())
            {
                ActiveRange_ = Second_;
            }
        }

        inline TType Front() const
        {
            return ActiveRange_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TCompare>
    class TUnitedRangesImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const First_;
        IRangeImpl<TType>* const Second_;
        IRangeImpl<TType>* ActiveRange_;
        TCompare Compare_;
        bool PopBoth_;

        template <class TAssert>
        TUnitedRangesImpl(TRange<TType, TAssert>& first,
            TRange<TType, TAssert>& second, TCompare compare);

    public:
        template <class TAssert>
        TUnitedRangesImpl(IRangeImpl<TType>* first,
            TRange<TType, TAssert>& second, TCompare compare);

        inline ~TUnitedRangesImpl()
        {
            delete First_;
            delete Second_;
        }

        inline bool IsEmpty() const
        {
            return First_->IsEmpty() && Second_->IsEmpty();
        }

        inline void Pop()
        {
            if (PopBoth_)
            {
                First_->Pop();
                Second_->Pop();
                PopBoth_ = false;
            }
            else
            {
                ActiveRange_->Pop();
            }

            if (First_->IsEmpty())
            {
                ActiveRange_ = Second_;
            }
            else if (Second_->IsEmpty())
            {
                ActiveRange_ = First_;
            }
            else if (Compare_(First_->Front(), Second_->Front()))
            {
                ActiveRange_ = First_;
            }
            else
            {
                ActiveRange_ = Second_;
                if (!Compare_(Second_->Front(), First_->Front()))
                {
                    PopBoth_ = true;
                }
            }
        }

        inline TType Front() const
        {
            return ActiveRange_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TCompare>
    class TIntersectedRangesImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const First_;
        IRangeImpl<TType>* const Second_;
        TCompare Compare_;

        void Next()
        {
            while (!IsEmpty())
            {
                if (Compare_(First_->Front(), Second_->Front()))
                {
                    First_->Pop();
                }
                else if (Compare_(Second_->Front(), First_->Front()))
                {
                    Second_->Pop();
                }
                else
                {
                    break;
                }
            }
        }

        template <class TAssert>
        TIntersectedRangesImpl(TRange<TType, TAssert>& first,
            TRange<TType, TAssert>& second, TCompare compare);

    public:
        template <class TAssert>
        TIntersectedRangesImpl(IRangeImpl<TType>* first,
            TRange<TType, TAssert>& second, TCompare compare);

        inline ~TIntersectedRangesImpl()
        {
            delete First_;
            delete Second_;
        }

        inline bool IsEmpty() const
        {
            return First_->IsEmpty() || Second_->IsEmpty();
        }

        inline void Pop()
        {
            First_->Pop();
            Second_->Pop();
            Next();
        }

        inline TType Front() const
        {
            return First_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TCompare>
    class TComplementedRangesImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const First_;
        IRangeImpl<TType>* const Second_;
        TCompare Compare_;

        void Next()
        {
            while (!IsEmpty() && !Second_->IsEmpty())
            {
                if (Compare_(First_->Front(), Second_->Front()))
                {
                    break;
                }
                else
                {
                    if (!Compare_(Second_->Front(), First_->Front()))
                    {
                        First_->Pop();
                    }
                    Second_->Pop();
                }
            }
        }

        template <class TAssert>
        TComplementedRangesImpl(TRange<TType, TAssert>& first,
            TRange<TType, TAssert>& second, TCompare compare);

    public:
        template <class TAssert>
        TComplementedRangesImpl(IRangeImpl<TType>* first,
            TRange<TType, TAssert>& second, TCompare compare);

        inline ~TComplementedRangesImpl()
        {
            delete First_;
            delete Second_;
        }

        inline bool IsEmpty() const
        {
            return First_->IsEmpty();
        }

        inline void Pop()
        {
            First_->Pop();
            Next();
        }

        inline TType Front() const
        {
            return First_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TCompare>
    class TSymmetricDifferenceImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const First_;
        IRangeImpl<TType>* const Second_;
        IRangeImpl<TType>* ActiveRange_;
        TCompare Compare_;

        IRangeImpl<TType>* Next()
        {
            while (!First_->IsEmpty())
            {
                if (Second_->IsEmpty())
                {
                    return First_;
                }
                else if (Compare_(First_->Front(), Second_->Front()))
                {
                    return First_;
                }
                else if (Compare_(Second_->Front(), First_->Front()))
                {
                    return Second_;
                }
                First_->Pop();
                Second_->Pop();
            }
            return Second_;
        }

        template <class TAssert>
        TSymmetricDifferenceImpl(TRange<TType, TAssert>& first,
            TRange<TType, TAssert>& second, TCompare compare);

    public:
        template <class TAssert>
        TSymmetricDifferenceImpl(IRangeImpl<TType>* first,
            TRange<TType, TAssert>& second, TCompare compare);

        inline ~TSymmetricDifferenceImpl()
        {
            delete First_;
            delete Second_;
        }

        inline bool IsEmpty() const
        {
            return First_->IsEmpty() && Second_->IsEmpty();
        }

        inline void Pop()
        {
            ActiveRange_->Pop();
            ActiveRange_ = Next();
        }

        inline TType Front() const
        {
            return ActiveRange_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TCompare>
    class TUniqueRangeImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const Range_;
        TCompare Compare_;

        template <class TAssert>
        TUniqueRangeImpl(TRange<TType, TAssert>& range, TCompare compare);

    public:
        inline TUniqueRangeImpl(IRangeImpl<TType>* range, TCompare compare)
            : Range_(range)
            , Compare_(compare)
        {
        }

        inline ~TUniqueRangeImpl()
        {
            delete Range_;
        }

        inline bool IsEmpty() const
        {
            return Range_->IsEmpty();
        }

        inline void Pop()
        {
            TType val = Range_->Front();
            do {
                Range_->Pop();
            } while (!Range_->IsEmpty() && Compare_(val, Range_->Front()));
        }

        inline TType Front() const
        {
            return Range_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TPredicate>
    class TRemoveImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const Range_;
        TPredicate Predicate_;

        void Next()
        {
            while (!Range_->IsEmpty() && Predicate_(Range_->Front()))
            {
                Range_->Pop();
            }
        }

        template <class TAssert>
        TRemoveImpl(TRange<TType, TAssert>& range, TPredicate predicate);

    public:
        inline TRemoveImpl(IRangeImpl<TType>* range, TPredicate predicate)
            : Range_(range)
            , Predicate_(predicate)
        {
            Next();
        }

        inline ~TRemoveImpl()
        {
            delete Range_;
        }

        inline bool IsEmpty() const
        {
            return Range_->IsEmpty();
        }

        inline void Pop()
        {
            Range_->Pop();
            Next();
        }

        inline TType Front() const
        {
            return Range_->Front();
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TOldType, class TUnaryOp>
    class TTransformedRangeImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TOldType>* const Range_;
        TUnaryOp Op_;

    public:
        template <class TAssert>
        TTransformedRangeImpl(TRange<TOldType, TAssert>& range, TUnaryOp op);

        inline ~TTransformedRangeImpl()
        {
            delete Range_;
        }

        inline bool IsEmpty() const
        {
            return Range_->IsEmpty();
        }

        inline void Pop()
        {
            Range_->Pop();
        }

        inline TType Front() const
        {
            return Op_(Range_->Front());
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TFirstType, class TSecondType,
        class TBinaryOp>
    class TTransformedRangesImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TFirstType>* const First_;
        IRangeImpl<TSecondType>* const Second_;
        TBinaryOp Op_;

    public:
        template <class TAssert>
        TTransformedRangesImpl(TRange<TFirstType, TAssert>& first,
            TRange<TSecondType, TAssert>& second, TBinaryOp op);

        inline ~TTransformedRangesImpl()
        {
            delete First_;
            delete Second_;
        }

        inline bool IsEmpty() const
        {
            return First_->IsEmpty() || Second_->IsEmpty();
        }

        inline void Pop()
        {
            First_->Pop();
            Second_->Pop();
        }

        inline TType Front() const
        {
            return Op_(First_->Front(), Second_->Front());
        }

        IRangeImpl<TType>* Clone() const;
    };

    template <class TType, class TInserter, class TDelimiter,
        class TEscapeChar, class TOldType>
    class TSplittedRangeImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TOldType>* const Range_;
        TType Value_;
        TInserter Inserter_;
        const TDelimiter Delimiter_;
        const TEscapeChar EscapeChar_;
        bool Empty_;

        void Next()
        {
            while (!Range_->IsEmpty())
            {
                const TOldType& front = Range_->Front();
                if (Delimiter_ == front)
                {
                    if (Empty_)
                    {
                        Range_->Pop();
                    }
                    else
                    {
                        break;
                    }
                }
                else if (EscapeChar_ == front)
                {
                    Empty_ = false;
                    Range_->Pop();
                    if (Range_->IsEmpty())
                    {
                        Inserter_ = front;
                    }
                    else
                    {
                        Inserter_ = Range_->Front();
                        Range_->Pop();
                    }
                }
                else
                {
                    Empty_ = false;
                    Inserter_ = front;
                    Range_->Pop();
                }
            }
        }

        template <class TAssert>
        TSplittedRangeImpl(TRange<TOldType, TAssert>& range, TType value,
            TDelimiter delimiter, TEscapeChar escapeChar);

    public:
        template <class TAssert>
        TSplittedRangeImpl(TRange<TOldType, TAssert>& range,
            TDelimiter delimiter, TEscapeChar escapeChar);

        inline ~TSplittedRangeImpl()
        {
            delete Range_;
        }

        inline bool IsEmpty() const
        {
            return Empty_;
        }

        inline void Pop()
        {
            Empty_ = true;
            if (!Range_->IsEmpty())
            {
                Value_ = TType();
                Next();
            }
        }

        inline TType Front() const
        {
            return Value_;
        }

        IRangeImpl<TType>* Clone() const;
    };

    struct TFakeEscapeChar
    {
        template <class TType>
        inline bool operator == (TType) const
        {
            return false;
        }
    };

    template <class TType, class TGenerator, class TCounter>
    class TGeneratedRangeImpl: public IRangeImpl<TType>
    {
        TGenerator Generator_;
        TCounter Counter_;
        TType Value_;
        bool Empty_;

        inline TGeneratedRangeImpl(TType value, TGenerator generator,
            TCounter counter)
            : Generator_(generator)
            , Counter_(counter)
            , Value_(value)
            , Empty_(false)
        {
        }

    public:
        inline TGeneratedRangeImpl(TGenerator generator, TCounter counter)
            : Generator_(generator)
            , Counter_(counter)
            , Value_(Generator_())
            , Empty_(false)
        {
            --Counter_;
        }

        inline bool IsEmpty() const
        {
            return Empty_;
        }

        inline void Pop()
        {
            Value_ = Generator_();
            if (!Counter_)
            {
                Empty_ = true;
            }
            else
            {
                --Counter_;
            }
        }

        inline TType Front() const
        {
            return Value_;
        }

        inline IRangeImpl<TType>* Clone() const
        {
            return new TGeneratedRangeImpl(Value_, Generator_, Counter_);
        }
    };
}

#endif

