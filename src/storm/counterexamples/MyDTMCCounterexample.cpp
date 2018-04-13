#include "storm/counterexamples/MyDTMCCounterexample.h"

#include "storm/logic/Formulas.h"

#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/settings/SettingsManager.h"
#include "storm/api/verification.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

#include <utility> 

namespace storm {
    namespace counterexamples {
        
        template <class ValueType>
        MyDTMCCounterexample<ValueType>::MyDTMCCounterexample(storm::models::sparse::Dtmc<ValueType> const& dtmc) : dtmc(dtmc) {
            // Intentionally left empty.
        }

        template <class ValueType>
        void MyDTMCCounterexample<ValueType>::generateCounterexample(std::shared_ptr<storm::logic::Formula const> const& formula) {
            // (1) Check formula for sanity.
            
            // Formula is a probability operator.
            STORM_LOG_THROW(formula->isProbabilityOperatorFormula(), storm::exceptions::NotSupportedException, "Only probability operators are supported as the top-level formula.");
            auto const& probabilityOperatorFormula = formula->asProbabilityOperatorFormula();
            
            // Probability operator has upper bound.
            STORM_LOG_THROW(probabilityOperatorFormula.hasBound() && !storm::logic::isLowerBound(probabilityOperatorFormula.getComparisonType()), storm::exceptions::NotSupportedException, "Only probability operators with bounds supported as the top-level formula.");
            double threshold = probabilityOperatorFormula.getThresholdAs<double>();

            // Nested formula is a bounded until.
            STORM_LOG_THROW(probabilityOperatorFormula.getSubformula().isBoundedUntilFormula(), storm::exceptions::NotSupportedException, "Currently only bounded until formulae are supported.");
            auto const& boundedUntilFormula = probabilityOperatorFormula.getSubformula().asBoundedUntilFormula();
            
            // (2) Create model checker to (a) verify the property does not hold (and there is a counterexample) and (b)
            // to get the set of states that are mentioned in the bounded until formula.
            storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<ValueType>> checker(this->dtmc);
            
            // Check the outer formula.
            auto task = storm::api::createTask<ValueType>(formula, true);
            auto result = checker.check(task);
            
            // Check formula is violated.
            STORM_LOG_ASSERT(result->isExplicitQualitativeCheckResult(), "Expected explicit qualitative result.");
            auto const& explicitQualitativeResult = result->asExplicitQualitativeCheckResult();
            STORM_LOG_THROW(!explicitQualitativeResult.forallTrue(), storm::exceptions::NotSupportedException, "Cannot generate counterexample for property that is satisfied by all initial states.");
            
            // Get subset of states for a and b (in formula a U<=k b).
            task = storm::api::createTask<ValueType>(boundedUntilFormula.getLeftSubformula().asSharedPointer(), false);
            result = checker.check(task);
            STORM_LOG_ASSERT(result->isExplicitQualitativeCheckResult(), "Expected explicit qualitative result.");
            storm::storage::BitVector leftStates = result->asExplicitQualitativeCheckResult().getTruthValuesVector();

            task = storm::api::createTask<ValueType>(boundedUntilFormula.getRightSubformula().asSharedPointer(), false);
            result = checker.check(task);
            STORM_LOG_ASSERT(result->isExplicitQualitativeCheckResult(), "Expected explicit qualitative result.");
            storm::storage::BitVector rightStates = result->asExplicitQualitativeCheckResult().getTruthValuesVector();

            // std::cout << "states satisfying '" << boundedUntilFormula.getLeftSubformula() << "': " << leftStates.size() << std::endl;
            // std::cout << "states satisfying '" << boundedUntilFormula.getRightSubformula()<< "': " << rightStates.size() << std::endl;
            
            // Loop over the transition matrix over the DTMC, because it's fun.
            auto const& transitionMatrix = this->dtmc.getTransitionMatrix();
           /* for (uint64_t row = 0; row < transitionMatrix.getRowCount(); ++row) {
                for (auto const& element : transitionMatrix.getRow(row)) {
                    std::cout << "The element at row " << row << " and column " << element.getColumn() << " is " << element.getValue() << std::endl;

                }
            }*/

            std::cout << std::endl; 

            /*------ Dijkstra to all -----*/
            
            std::vector<Node> allNodes(transitionMatrix.getRowCount());
            // std::vector<Node> allNodes(transitionMatrix.getRowCount()); // free once not needed
            
            allNodes[0].Shortest = 1.0;
            allNodes[0].Paths[0].Probability = 1.0;
            int x = 0;
            double prob = -1.0;
            while(true) {
                prob = -1.0;

                for (uint64_t n = 0; n < allNodes.size(); ++n) {
                    if(allNodes[n].Shortest > prob  && !allNodes[n].Visited) {
                        prob = allNodes[n].Shortest;
                        x = n;
                    }
                }   
                if (prob == -1.0)             
                    break;
                for (auto const& element : transitionMatrix.getRow(x)) {
                    if (allNodes[element.getColumn()].Shortest < allNodes[x].Shortest * element.getValue() && !allNodes[element.getColumn()].Visited) {
                        allNodes[element.getColumn()].Shortest = allNodes[x].Shortest * element.getValue();
                        allNodes[element.getColumn()].Paths[0].PrevNode = x;
                        allNodes[element.getColumn()].Paths[0].Kth = 0;
                        allNodes[element.getColumn()].Paths[0].Probability = allNodes[x].Shortest * element.getValue();
                        // std::cout << ".";
                        // std::cout << "setting unvisited probability of " << element.getColumn() << " to " << allNodes[element.getColumn()].Shortest << std::endl;
                    }
                }
                allNodes[x].Shortest = allNodes[x].Shortest;
                allNodes[x].Visited = true;
            }

            /*------ End of Dijkstra to all -----*/
            uint64_t t = 0;
            for (uint64_t n = 0; n < allNodes.size(); ++n) { // beware of multiple terminal states
                if(rightStates.get(n) == 1)
                    t = n;
            }
            std::cout << "terminal state: " << t << std::endl;
            std::cout << "probability threshold: " << threshold << std::endl;

            /*------ KSH -----*/

            for (uint64_t n = 0; n < allNodes.size(); ++n) {
                for (auto const& element : transitionMatrix.getRow(n)) {
                    if (!(element.getColumn() == n && element.getValue() == 1.0)) {
                        allNodes[element.getColumn()].Predecessors.emplace_back(n, element.getValue());
                    }
                }
            }
            
            /*------ Real fun with KSH -----*/

            std::vector<UltimateStackItem> ultimateStack(0); // cool "array" to help out with "recursion"

            uint64_t k = 1;     // looking for second shortest path
            uint64_t v = t;     // into terminal node
            
            uint64_t u = 0;     // just init
            uint64_t ktmp = 0;  // just init
            bool skip = false;  // just init - skip to step 6 from step 2
            bool in = true;

            double probabilitySum = allNodes[t].Paths[0].Probability;

            while (!(threshold <= probabilitySum && v == t)) {
                // std::cout << "checkpoint 1" << std::endl;
                if (in) {
                    // step 1 if k = 1 get candidates (note we are counting from zero)
                    if(k == 1) {
                        for (uint64_t n = 0; n < allNodes[v].Predecessors.size(); ++n) {
                            if (allNodes[v].Predecessors[n].first != allNodes[v].Paths[0].PrevNode) {
                                allNodes[v].Candidates.push_back({allNodes[v].Predecessors[n].first, 0, allNodes[allNodes[v].Predecessors[n].first].Paths[0].Probability * allNodes[v].Predecessors[n].second});
                            }
                        }
                        // step 2 if k = 1 and v = s get ready for skip 
                        if(v == 0) {
                            in = false;
                            skip = true;
                        }
                    }
                    if (!skip) {
                        // step 3 prepare some variables
                        u = allNodes[v].Paths[k-1].PrevNode;
                        ktmp = allNodes[v].Paths[k-1].Kth + 1;
                        // step 4
                        if (allNodes[u].Paths.size() > ktmp)
                        {
                            for (uint64_t n = 0; n < allNodes[v].Predecessors.size(); ++n) {
                                if (allNodes[v].Predecessors[n].first == u) {
                                    allNodes[v].Candidates.push_back({u, ktmp, allNodes[u].Paths[ktmp].Probability * allNodes[v].Predecessors[n].second});
                                }
                            }
                            in = false;
                        }
                        else
                        {
                            ultimateStack.push_back({v,k});
                            v = u;
                            k = ktmp;
                        }
                    }
                } else {
                    // step 6
                    int bestCandidate = -1;                 //
                    double bestCandidateProbability = -1.0; // you can fill both into Paths directly
                    for (uint64_t n = 0; n < allNodes[v].Candidates.size(); ++n) {
                        if (allNodes[v].Candidates[n].Probability > bestCandidateProbability) {
                            bestCandidate = n;
                            bestCandidateProbability = allNodes[v].Candidates[n].Probability;
                        }
                    }

                    if (bestCandidate != -1) {
                        allNodes[v].Paths.push_back({allNodes[v].Candidates[bestCandidate].PrevNode, allNodes[v].Candidates[bestCandidate].Kth, bestCandidateProbability});
                        allNodes[v].Candidates.erase(allNodes[v].Candidates.begin() + bestCandidate);
                    } else if (ultimateStack.size() == 0) {
                        std::cout << "No more paths available!" << std::endl; // right?
                        break;
                    }

                    if(ultimateStack.size() == 0) {
                        probabilitySum += allNodes[t].Paths[k].Probability;
                        k++;
                        v = t;
                        in = true;
                        skip = false;
                        // std::cout << ".";
                    } else {
                        // std::cout << "checkpoint 5" << std::endl; // step 5 is here
                        UltimateStackItem pop = ultimateStack.back();
                        for (uint64_t n = 0; n < allNodes[pop.Node].Predecessors.size(); ++n) {
                            if (allNodes[pop.Node].Predecessors[n].first == v && bestCandidate != -1) { //!!
                                allNodes[pop.Node].Candidates.push_back({v, k, allNodes[v].Paths[k].Probability * allNodes[pop.Node].Predecessors[n].second});
                                // std::cout << "Adding candidate " << v << " - " << k << " with prob " << allNodes[v].Paths[k].Probability * allNodes[pop.Node].NodePredecessorsProbability[n] << " to node " << pop.Node << std::endl;
                            }
                        }
                        k = pop.K;
                        v = pop.Node;

                        ultimateStack.pop_back();
                    }
                }
            }
            
            /*------ End of KSH -----*/

            for (uint64_t n = 0; n < allNodes[t].Paths.size(); ++n) {
                std::cout << "k: " << n << " (" << allNodes[t].Paths[n].Probability << ")" << std::endl;
                x = t;
                k = n;
                while (!(k == 0 && x == 0)) {
                    std::cout << x << " <- ";
                    ktmp = allNodes[x].Paths[k].Kth;
                    x = allNodes[x].Paths[k].PrevNode;
                    k = ktmp;
                }
                std::cout << "0" << std::endl;
            }

            //storm::counterexamples::MyDTMCCounterexample<ValueType>::PrintPotato();
            //exit (0);
        }

        template <class ValueType>
        void MyDTMCCounterexample<ValueType>::PrintPotato() {
            std::cout << "POTATO" << std::endl;
        }

        //void MyDTMCCounterexample<ValueType>::NextPath(std::vector<storm::counterexamples::MyDTMCCounterexample<ValueType>::Node> &AllNodes) {
        /*template <class ValueType>
        void NextPath(auto &OneNode) {
            std::cout << "xxx" << std::endl;        
        }*/


        template class MyDTMCCounterexample<double>;
    }
}
