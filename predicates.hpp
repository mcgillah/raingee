/*
 * predicates.hpp           -- helper functors used as predicates
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

#ifndef __PREDICATES_HPP_2012_02_01__
#define __PREDICATES_HPP_2012_02_01__

namespace NRaingee
{
    template <class TPred>
    class TNot
    {
        const TPred Pred_;

    public:
        inline TNot(const TPred& pred)
            : Pred_(pred)
        {
        }

        inline bool operator ()() const
        {
            return !Pred_();
        }
    };

    template <class TPred>
    static inline TNot<TPred> Not(const TPred& pred)
    {
        return TNot<TPred>(pred);
    }

    template <class TClass, class TPred>
    class TBindMember
    {
        const TClass* const This_;
        const TPred Pred_;

    public:
        inline TBindMember(const TClass* pthis, const TPred& pred)
            : This_(pthis)
            , Pred_(pred)
        {
        }

        inline bool operator ()() const
        {
            return (This_->*Pred_)();
        }
    };

    template <class TClass, class TPred>
    static inline TBindMember<TClass, TPred> BindMember(const TClass* pthis,
        const TPred& pred)
    {
        return TBindMember<TClass, TPred>(pthis, pred);
    }
}

#endif

