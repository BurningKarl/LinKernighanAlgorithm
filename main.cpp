#include <iostream>
#include <fstream>
#include "Tour.h"
#include "TsplibProblem.h"
#include "TsplibTour.h"
#include "SimpleHeuristic.h"

// TODO: Don't let Tour be a subclass of VertexList, but instead use it internally and make all Tours constant ???
// TODO: Merge TsplibProblem and TsplibTour
// TODO: Replace std::map neighbors by std::vector neighbors to avoid errors with missing vertices
// TODO: Refactor all const in this code

int main(int argc, char *argv[]) {
    std::ifstream problemFile;
    if (argc < 2) {
        std::cerr << "No file supplied." << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "    LinKerninghanAlgorithm tsplib_problem.tsp [tsplib_problem.opt.tour]" << std::endl;
        return 1;
    } else {
        problemFile.open(argv[1]);
    }

    // Check whether the problem file could be opened
    if (!problemFile.is_open() or !problemFile.good()) {
        std::cerr << "Could not open the TSPLIB file '" << argv[1] << "'" << std::endl;
        return 1;
    }

    // Interpret the problem file and report any syntax errors, logical errors or unsupported keywords
    TsplibProblem problem;
    std::string errorMessage = problem.readFile(problemFile);
    problemFile.close();

    std::cout << "Opened the " << problem.getName() << " TSPLIB file" << std::endl;
    if (!errorMessage.empty()) {
        std::cerr << "The TSPLIB file has an invalid format: " << errorMessage
                  << std::endl;
        return 1;
    }

    // Use the heuristic from the introduction assignment to get a tour
    Tour tour = simpleHeuristic(problem);

    // DEBUG: Check whether tour really is a hamiltonian tour
    if (!tour.isHamiltonianTour()) {
        throw std::runtime_error("The tour generated by the algorithm is not a hamiltonian tour");
    }

    // Output the best tour found by the algorithm and compare it to the optimal tour if given
    distance_t length = problem.length(tour);
    std::cout << "This is the shortest tour found:" << std::endl;
    std::cout << tour;
    std::cout << "It is " << length << " units long." << std::endl << std::endl;

    // Check whether a second command line argument is given
    std::ifstream optimalTourFile;
    if (argc >= 3) {
        // Try to open the file
        optimalTourFile.open(argv[2]);
        if (!optimalTourFile.is_open() or !optimalTourFile.good()) {
            std::cerr << "Could not open the TSPLIB tour file '" << argv[2] << "'" << std::endl;
            return 1;
        }

        // Interpret the tour file and report any syntax errors, logical errors or unsupported keywords
        TsplibTour optimalTour;
        std::string tourErrorMessage = optimalTour.readFile(optimalTourFile);
        optimalTourFile.close();

        if (!tourErrorMessage.empty()) {
            std::cerr << "The TSPLIB tour file has an invalid format: " << tourErrorMessage << std::endl;
            return 1;
        }

        // Check whether the TsplibProblem problem and the TsplibTour optimalTour belong to the same problem
        if (optimalTour.getName() != problem.getName() + ".opt.tour") {
            std::cerr << "The TSPLIB tour file does not belong to the TSPLIB problem file";
            return 1;
        }

        // Print the optimal tour and its length and compare it to the tour returned by the heuristic
        distance_t optimalLength = problem.length(optimalTour);
        std::cout << "This is the optimal tour:" << std::endl;
        std::cout << optimalTour;
        std::cout << "It is " << optimalLength << " units long." << std::endl << std::endl;
        std::cout << "The best tour found by the heuristic is " << (length / (double) optimalLength - 1) * 100
                  << "% above the optimum." << std::endl;
    }

    return 0;
}
