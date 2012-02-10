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
        virtual const TType& Front() const = 0;
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

        inline const TType& Front() const
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

        inline const TType& Front() const
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

    public:
        inline TRepeatedRangeImpl(const IRangeImpl<TType>* range,
            TCounter counter)
            : Range_(range)
            , CurrentRange_(Range_->Clone())
            , Counter_(counter)
        {
            --Counter_;
        }

        inline TRepeatedRangeImpl(const IRangeImpl<TType>* range,
            IRangeImpl<TType>* current,
            TCounter counter)
            : Range_(range)
            , CurrentRange_(current)
            , Counter_(counter)
        {
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
            if (CurrentRange_->IsEmpty() && Counter_)
            {
                delete CurrentRange_;
                CurrentRange_ = Range_->Clone();
                --Counter_;
            }
        }

        inline const TType& Front() const
        {
            return CurrentRange_->Front();
        }

        inline IRangeImpl<TType>* Clone() const
        {
            return new TRepeatedRangeImpl(Range_->Clone(),
                CurrentRange_->Clone(), Counter_);
        }
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

    public:
        inline TConcatenatedRangesImpl(IRangeImpl<TType>* first,
            IRangeImpl<TType>* second)
            : First_(first)
            , Second_(second)
            , ActiveRange_(First_)
        {
        }

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

        inline const TType& Front() const
        {
            return ActiveRange_->Front();
        }

        inline IRangeImpl<TType>* Clone() const
        {
            if (ActiveRange_ == First_)
            {
                return new TConcatenatedRangesImpl(First_->Clone(),
                    Second_->Clone());
            }
            else
            {
                return Second_->Clone();
            }
        }
    };

    template <class TType, class TCompare>
    class TUnitedRangesImpl: public IRangeImpl<TType>
    {
        IRangeImpl<TType>* const First_;
        IRangeImpl<TType>* const Second_;
        IRangeImpl<TType>* ActiveRange_;
        TCompare Compare_;
        bool PopBoth_;

    public:
        inline TUnitedRangesImpl(IRangeImpl<TType>* first,
            IRangeImpl<TType>* second, TCompare compare)
            : First_(first)
            , Second_(second)
            , ActiveRange_(Compare_(First_->Front(), Second_->Front()) ?
                First_ : Second_)
            , Compare_(compare)
            , PopBoth_(ActiveRange_ == Second_
                && !Compare_(Second_->Front(), First_->Front()))
        {
        }

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

        inline const TType& Front() const
        {
            return ActiveRange_->Front();
        }

        inline IRangeImpl<TType>* Clone() const
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
                return new TUnitedRangesImpl(First_->Clone(),
                    Second_->Clone(), Compare_);
            }
        }
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

    public:
        inline TIntersectedRangesImpl(IRangeImpl<TType>* first,
            IRangeImpl<TType>* second, TCompare compare)
            : First_(first)
            , Second_(second)
            , Compare_(compare)
        {
            Next();
        }

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

        inline const TType& Front() const
        {
            return First_->Front();
        }

        inline IRangeImpl<TType>* Clone() const
        {
            return new TIntersectedRangesImpl(First_->Clone(),
                Second_->Clone(), Compare_);
        }
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

    public:
        inline TComplementedRangesImpl(IRangeImpl<TType>* first,
            IRangeImpl<TType>* second, TCompare compare)
            : First_(first)
            , Second_(second)
            , Compare_(compare)
        {
            Next();
        }

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

        inline const TType& Front() const
        {
            return First_->Front();
        }

        inline IRangeImpl<TType>* Clone() const
        {
            if (Second_->IsEmpty())
            {
                return First_->Clone();
            }
            else
            {
                return new TComplementedRangesImpl(First_->Clone(),
                    Second_->Clone(), Compare_);
            }
        }
    };
}

#endif

