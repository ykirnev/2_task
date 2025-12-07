#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <memory>
#include <fstream>
#include <sstream>
#include "Mutation/Mutation_solo.h"
#include "Cooling.h"

class SimulatedAnnealing {
public:
    SimulatedAnnealing(Solution *solution, MutationOperation *mutationOperation, CoolingSchedule *coolingSchedule,
                       double initialTemperature, int maxIterations, int maxNoImprovementCount)
            : solution(solution), mutationOperation(mutationOperation), coolingSchedule(coolingSchedule),
              temperature(initialTemperature), maxIterations(maxIterations),
              maxNoImprovementCount(maxNoImprovementCount) {}

    void start() {
        int iteration = 0;
        double bestCost = solution->getCost();
        auto bestSolution = solution->clone();
        int noImprovementCount = 0;
        std::uniform_real_distribution<double> uni(0.0, 1.0);
        while (iteration < maxIterations && noImprovementCount < maxNoImprovementCount) {
            auto currentSolution = bestSolution->clone();
            mutationOperation->mutate(*currentSolution);
            double currentCost = currentSolution->getCost();
            if (currentCost < bestCost) {
                bestCost = currentCost;
                noImprovementCount = 0;
                bestSolution = currentSolution;
            } else {
                double delta = currentCost - bestCost;
                double acceptanceProbability = std::exp(-delta / temperature);
                if (uni(bestSolution->getRng()) < acceptanceProbability) {
                    noImprovementCount = 0;
                    bestSolution = currentSolution;
                } else {
                    noImprovementCount++;
                }
            }
            temperature = coolingSchedule->getNextTemperature(temperature, iteration);
            iteration++;
        }
        bestSolution->print();
        std::cout << "Best solution found with cost: " << bestCost << std::endl;
    }

private:
    Solution *solution;
    MutationOperation *mutationOperation;
    CoolingSchedule *coolingSchedule;
    double temperature;
    int maxIterations;
    int maxNoImprovementCount;
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
        std::string jobId, durationStr;
        std::getline(ss, jobId, ',');
        std::getline(ss, durationStr, ',');
        jobDurations.push_back(std::stoi(durationStr));
    }
    file.close();
    return jobDurations;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <num_processors> <cooling_method>" << std::endl;
        std::cerr << "Cooling methods: boltzmann, cauchy, logarithmic" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    int numProcessors = std::stoi(argv[2]);
    std::string coolingMethod = argv[3];
    std::unique_ptr<CoolingSchedule> coolingSchedule;
    double initialTemperature = 100.0;
    if (coolingMethod == "boltzmann") {
        coolingSchedule = std::make_unique<BoltzmannCooling>(initialTemperature);
    } else if (coolingMethod == "cauchy") {
        coolingSchedule = std::make_unique<CauchyCooling>(initialTemperature);
    } else if (coolingMethod == "logarithmic") {
        coolingSchedule = std::make_unique<LogarithmicCooling>(initialTemperature);
    } else {
        std::cerr << "Invalid cooling method. Available methods: boltzmann, cauchy, logarithmic" << std::endl;
        return 1;
    }
    try {
        std::vector<uint8_t> jobDurations = loadJobDurationsFromCSV(filename);
        int numJobs = jobDurations.size();
        SchedulingSolution solution(numJobs, numProcessors, jobDurations);
        SchedulingMutation mutationOperation;
        int maxIterations = 100000;
        int maxNoImprovementCount = 100;
        SimulatedAnnealing sa(&solution, &mutationOperation, coolingSchedule.get(), initialTemperature, maxIterations,
                              maxNoImprovementCount);
        sa.start();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}