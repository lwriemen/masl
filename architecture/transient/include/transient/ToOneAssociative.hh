/*
 * ----------------------------------------------------------------------------
 * (c) 2005-2023 - CROWN OWNED COPYRIGHT. All rights reserved.
 * The copyright of this Software is vested in the Crown
 * and the Software is the property of the Crown.
 * ----------------------------------------------------------------------------
 * SPDX-License-Identifier: Apache-2.0
 * ----------------------------------------------------------------------------
 */

#ifndef TRANSIENT_ToOneAssociative_HH
#define TRANSIENT_ToOneAssociative_HH

#include <swa/ProgramError.hh>

namespace transient {
    template <class Related, class Associative>
    class ToOneAssociative {
      private:
        typedef SWA::ObjectPtr<Related> RelatedPtr;
        typedef SWA::ObjectPtr<Associative> AssociativePtr;

      public:
        ToOneAssociative()
            : related(), assoc() {}

        void link(RelatedPtr rhs, AssociativePtr associative);
        void unlink(RelatedPtr rhs, AssociativePtr associative);

        RelatedPtr navigate() const {
            return related;
        }

        template <class Predicate>
        RelatedPtr navigate(Predicate predicate) const {
            return related && predicate(related.deref()) ? related : RelatedPtr();
        }

        AssociativePtr navigateAssociative() const {
            return assoc;
        }

        template <class Predicate>
        AssociativePtr navigateAssociative(Predicate predicate) const {
            return assoc && predicate(assoc.deref()) ? assoc : AssociativePtr();
        }

        AssociativePtr correlate(RelatedPtr rhs) const {
            return (rhs == related) ? assoc : AssociativePtr();
        }

        std::size_t count() const {
            return related ? 1 : 0;
        }

      private:
        RelatedPtr related;
        AssociativePtr assoc;
    };

    template <class Related, class Associative>
    void ToOneAssociative<Related, Associative>::link(RelatedPtr rhs, AssociativePtr associative) {
        if (related)
            throw SWA::ProgramError("Attempt to overwrite relationship");
        related = rhs;
        assoc = associative;
    }

    template <class Related, class Associative>
    void ToOneAssociative<Related, Associative>::unlink(RelatedPtr rhs, AssociativePtr associative) {
        if (related != rhs)
            throw SWA::ProgramError("Objects not linked");
        if (assoc != associative)
            throw SWA::ProgramError("Objects not linked unsing this associative");
        related = RelatedPtr();
        assoc = AssociativePtr();
    }

} // namespace transient

#endif
