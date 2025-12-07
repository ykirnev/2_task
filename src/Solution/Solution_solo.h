#include "Abstract/AbstractSolution.h"
#include <vector>
#include <random>
#include <limits>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <numeric>  // for std::iota

class SchedulingSolution : public Solution {
public:
    SchedulingSolution(int numJobs, int numProcessors, const std::vector<uint8_t> &jobDurations)
            : numJobs(numJobs), numProcessors(numProcessors), jobDurations(jobDurations),
              distribution(0, numProcessors - 1) {
        std::random_device rd;
        rng.seed(rd());
        initializeSchedule();
    }

    double getCost() const override {
        std::vector<int> completion(numJobs, 0);
        for (int p = 0; p < numProcessors; ++p) {
            int currentTime = 0;
            for (int job : processorJobs[p]) {
                currentTime += jobDurations[job];
                completion[job] = currentTime;
            }
        }
        if (completion.empty()) return 0.0;
        int Tmax = *std::max_element(completion.begin(), completion.end());
        int Tmin = *std::min_element(completion.begin(), completion.end());
        return static_cast<double>(Tmax - Tmin);
    }

    void print() const override {
        for (int p = 0; p < numProcessors; ++p) {
            std::cout << "Processor " << p << ": ";
            int load = 0;
            for (int job : processorJobs[p]) {
                std::cout << "Job " << job << " (" << static_cast<int>(jobDurations[job]) << ") ";
                load += jobDurations[job];
            }
            std::cout << " | Total Load = " << load << std::endl;
        }
    }

    std::shared_ptr<Solution> cloneWithNewSeed(unsigned int seed) const override {
        auto cloned = std::make_shared<SchedulingSolution>(*this);
        cloned->rng.seed(seed);
        return cloned;
    }

    std::shared_ptr<Solution> clone() const override {
        return std::make_shared<SchedulingSolution>(*this);
    }

    int getNumJobs() const { return numJobs; }
    int getNumProcessors() const { return numProcessors; }

    int getJobProcessor(int job) const {
        for (int p = 0; p < numProcessors; ++p) {
            auto& jobs = processorJobs[p];
            if (std::find(jobs.begin(), jobs.end(), job) != jobs.end()) {
                return p;
            }
        }
        return -1; 
    }

    void removeJobFromProcessor(int job, int proc) {
        auto& jobs = processorJobs[proc];
        auto it = std::find(jobs.begin(), jobs.end(), job);
        if (it != jobs.end()) {
            jobs.erase(it);
        }
    }

    void addJobToProcessor(int job, int proc, int position) {
        auto& jobs = processorJobs[proc];
        if (position > static_cast<int>(jobs.size())) position = jobs.size();
        jobs.insert(jobs.begin() + position, job);
    }

    std::mt19937 &getRng() override { return rng; }
    mutable std::vector<std::vector<int>> processorJobs;

private:
    void initializeSchedule() {
        processorJobs.assign(numProcessors, std::vector<int>());
        std::vector<int> jobList(numJobs);
        std::iota(jobList.begin(), jobList.end(), 0);
        std::shuffle(jobList.begin(), jobList.end(), rng);
        for (int job : jobList) {
            int proc = distribution(rng);
            processorJobs[proc].push_back(job);
        }
    }

    int numJobs;
    int numProcessors;
    std::vector<uint8_t> jobDurations;
    mutable std::mt19937 rng;
    std::uniform_int_distribution<int> distribution;
};