#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>
#include <chrono>
#include <memory>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include "Mutation/Mutation_mult.h"
#include "Cooling.h"

std::shared_ptr<Solution> globalBestSolution;

class ParallelSimulatedAnnealing {
public:
    ParallelSimulatedAnnealing(Solution *solution, MutationOperation *mutationOperation, CoolingSchedule *coolingSchedule, double initialTemperature, int maxNoImprovementCount, int threadID, unsigned int seed)
        : initialSolution(solution->cloneWithNewSeed(seed)), mutationOperation(mutationOperation), coolingSchedule(coolingSchedule), temperature(initialTemperature), maxNoImprovementCount(maxNoImprovementCount), threadID(threadID), rng(seed) {}

    void start() {
        int iteration = 0;
        double bestCost = initialSolution->getCost();
        auto bestSolution = initialSolution->clone();
        int noImprovementCount = 0;
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        while (noImprovementCount < maxNoImprovementCount) {
            auto currentSolution = bestSolution->clone();
            mutationOperation->mutate(*currentSolution);
            double currentCost = currentSolution->getCost();
            if (currentCost < bestCost) {
                bestCost = currentCost;
                noImprovementCount = 0;
                bestSolution = currentSolution;
            } else {
                double acceptanceProbability = std::exp(-(currentCost - bestCost) / temperature);
                if (uni(rng) < acceptanceProbability) {
                    noImprovementCount = 0;
                    bestSolution = currentSolution;
                } else {
                    noImprovementCount++;
                }
            }
            temperature = coolingSchedule->getNextTemperature(temperature, iteration);
            iteration++;
        }
        localBestSolution = bestSolution;
    }

    std::shared_ptr<Solution> getLocalBestSolution() const {
        return localBestSolution;
    }

private:
    std::shared_ptr<Solution> initialSolution;
    std::shared_ptr<Solution> localBestSolution;
    MutationOperation *mutationOperation;
    CoolingSchedule *coolingSchedule;
    double temperature;
    int maxNoImprovementCount;
    int threadID;
    std::mt19937 rng;
};

std::vector<uint8_t> loadJobDurationsFromCSV(const std::string &filename) {
    std::vector<uint8_t> jobDurations;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file " + filename);
    }
    std::string line;
    bool isHeader = true;
    while (std::getline(file, line)) {
        if (isHeader) {
            isHeader = false;
            continue;
        }
        std::stringstream ss(line);
        std::string jobId;
        std::string durationStr;
        std::getline(ss, jobId, ',');
        std::getline(ss, durationStr, ',');
        uint8_t duration = std::stoi(durationStr);
        jobDurations.push_back(duration);
    }
    file.close();
    return jobDurations;
}

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <numThreads>" << std::endl;
            return 1;
        }
        int numThreads = std::stoi(argv[1]);
        std::vector<uint8_t> jobDurations = loadJobDurationsFromCSV("input/jobs_16000.csv");
        int numJobs = jobDurations.size();
        int numProcessors = 40;
        int maxNoImprovementCount = 100;
        int maxGlobalNoImprovementCount = 20;
        SchedulingMutation mutationOperation;
        BoltzmannCooling coolingSchedule(100.0);
        double initialTemperature = 100.0;
        int globalNoImprovementCount = 0;
        if (!globalBestSolution) {
            unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
            globalBestSolution = std::make_shared<SchedulingSolution>(numJobs, numProcessors, jobDurations, seed);
        }
        while (globalNoImprovementCount < maxGlobalNoImprovementCount) {
            std::vector<std::thread> threads;
            std::vector<std::shared_ptr<Solution>> localBestSolutions(numThreads);
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back([&, i]() {
                    unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count() + i;
                    std::shared_ptr<Solution> initialSolution = globalBestSolution->cloneWithNewSeed(seed);
                    ParallelSimulatedAnnealing sa(initialSolution.get(), &mutationOperation, &coolingSchedule, initialTemperature, maxNoImprovementCount, i, seed);
                    sa.start();
                    localBestSolutions[i] = sa.getLocalBestSolution();
                });
            }
            for (auto &t : threads) {
                t.join();
            }
            bool improved = false;
            for (const auto &localBest : localBestSolutions) {
                if (localBest->getCost() < globalBestSolution->getCost()) {
                    globalBestSolution = localBest;
                    improved = true;
                }
            }
            if (improved) {
                globalNoImprovementCount = 0;
            } else {
                globalNoImprovementCount++;
            }
            std::cout << "Current best solution cost: " << globalBestSolution->getCost() << std::endl;
        }
        std::cout << "Best solution cost: " << globalBestSolution->getCost() << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}