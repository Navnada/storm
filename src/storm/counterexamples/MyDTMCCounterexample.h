#pragma once

#include "storm/models/sparse/Dtmc.h"

#include "storm/logic/Formula.h"

namespace storm {
    namespace counterexamples {
        
        template <class ValueType>
        class MyDTMCCounterexample {
        public:
            MyDTMCCounterexample(storm::models::sparse::Dtmc<ValueType> const& dtmc);
            
            void generateCounterexample(std::shared_ptr<storm::logic::Formula const> const& formula);
            
            void PrintPotato();

            struct Path {
                uint64_t PrevNode;
                uint64_t Kth;
                double Probability;
            };

            struct Candidate {
                uint64_t PrevNode;
                uint64_t Kth;
                double Probability;
            };

            struct Node {
                double Shortest = -1.0; // just for Dijkstra
                bool Visited = false;
                std::vector<Path> Paths = std::vector<Path>(1);
                std::vector<Candidate> Candidates = std::vector<Candidate>(0);
                std::vector< std::pair<uint64_t, double> > Predecessors = std::vector< std::pair<uint64_t, double> >(0);
            };

            struct UltimateStackItem {
                uint64_t Node;
                uint64_t K;
            };

        private:
            storm::models::sparse::Dtmc<ValueType> const& dtmc;
        };
    } // namespace counterexamples
} // namespace storm
