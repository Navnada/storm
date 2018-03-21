#include "storm/counterexamples/MyDTMCCounterexample.h"

#include "storm/logic/Formulas.h"

#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/settings/SettingsManager.h"
#include "storm/api/verification.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

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

            std::cout << "states satisfying '" << boundedUntilFormula.getLeftSubformula() << "': " << leftStates << std::endl;
            std::cout << "states satisfying '" << boundedUntilFormula.getRightSubformula()<< "': " << rightStates << std::endl;
            
            // Loop over the transition matrix over the DTMC, because it's fun.
            auto const& transitionMatrix = this->dtmc.getTransitionMatrix();
            for (uint64_t row = 0; row < transitionMatrix.getRowCount(); ++row) {
                for (auto const& element : transitionMatrix.getRow(row)) {
                    std::cout << "The element at row " << row << " and column " << element.getColumn() << " is " << element.getValue() << std::endl;
                }
            }
        }
        
        template class MyDTMCCounterexample<double>;
    }
}
