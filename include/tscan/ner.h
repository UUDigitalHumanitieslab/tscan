#ifndef NER_H
#define NER_H

#include <string>
#include <iostream>
#include "libfolia/folia.h"
#include "tscan/sem.h"

namespace NER {

    enum Type {
        NONER,
        LOC_B, LOC_I,
        EVE_B, EVE_I,
        ORG_B, ORG_I,
        MISC_B, MISC_I,
        PER_B, PER_I,
        PRO_B, PRO_I
    };
    Type lookupNer(const folia::Word*, const folia::Sentence*);
    std::string toString(Type);
    std::ostream& operator<<(std::ostream&, Type);

    SEM::Type toSem(Type);
}

#endif /* NER_H */
