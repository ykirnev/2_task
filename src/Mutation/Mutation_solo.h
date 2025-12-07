#pragma once
#include "Abstract/AbstractMutationOperation.h"
#include "Solution/Solution_solo.h" 
#include <random>
#include <algorithm>

class SchedulingMutation : public MutationOperation {
public:
    void mutate(Solution &solution) override {
        SchedulingSolution &schedSolution = dynamic_cast<SchedulingSolution &>(solution);
        std::mt19937 &rng = schedSolution.getRng();
        std::uniform_int_distribution<int> jobDist(0, schedSolution.getNumJobs() - 1);
        int jobIndex = jobDist(rng);
        int oldProcessor = schedSolution.getJobProcessor(jobIndex);
        if (oldProcessor == -1) return;  // Error
        std::uniform_int_distribution<int> procDist(0, schedSolution.getNumProcessors() - 1);
        int newProcessor = procDist(rng);
        while (newProcessor == oldProcessor) {
            newProcessor = procDist(rng);
        }
        schedSolution.removeJobFromProcessor(jobIndex, oldProcessor);
        std::uniform_int_distribution<int> posDist(0, schedSolution.processorJobs[newProcessor].size());
        int insertPos = posDist(rng);
        schedSolution.addJobToProcessor(jobIndex, newProcessor, insertPos);
    }
};