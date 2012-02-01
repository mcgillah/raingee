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
        const std::vector<TType> Data_;
        typename std::vector<TType>::const_iterator Begin_;
        const typename std::vector<TType>::const_iterator End_;

    public:
        typedef typename std::vector<TType>::size_type TSizeType_;

        inline TSequenceRangeImpl(TSizeType_ size, const TType& value)
            : Data_(size, value)
            , Begin_(Data_.begin())
            , End_(Data_.end())
        {
        }

        template <class TInputIterator>
        inline TSequenceRangeImpl(TInputIterator first, TInputIterator last)
            : Data_(first, last)
            , Begin_(Data_.begin())
            , End_(Data_.end())
        {
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
            return new TSequenceRangeImpl(Begin_, End_);
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

    public:
        inline TUnitedRangesImpl(IRangeImpl<TType>* first,
            IRangeImpl<TType>* second, TCompare compare)
            : First_(first)
            , Second_(second)
            , ActiveRange_(Compare_(Second_->Front(), First_->Front()) ?
                Second_ : First_)
            , Compare_(compare)
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
            if (ActiveRange_)
            {
                ActiveRange_->Pop();
            }
            else
            {
                First_->Pop();
                Second_->Pop();
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
            else if (Compare_(Second_->Front(), First_->Front()))
            {
                ActiveRange_ = Second_;
            }
            else
            {
                ActiveRange_ = 0;
            }
        }

        inline const TType& Front() const
        {
            if (ActiveRange_)
            {
                return ActiveRange_->Front();
            }
            else
            {
                return First_->Front();
            }
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

