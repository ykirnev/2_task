#pragma once

#include "AbstractSolution.h"

class MutationOperation {
public:
    virtual void mutate(Solution &solution) = 0;
    virtual ~MutationOperation() = default;
};