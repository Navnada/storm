#include "storm/solver/MinMaxLinearEquationSolver.h"

#include <cstdint>

#include "storm/solver/LinearEquationSolver.h"
#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/TopologicalMinMaxLinearEquationSolver.h"
#include "storm/solver/LpMinMaxLinearEquationSolver.h"

#include "storm/environment/Environment.h"
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/IllegalFunctionCallException.h"

namespace storm {
    namespace solver {
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::MinMaxLinearEquationSolver(OptimizationDirectionSetting direction) : direction(direction), trackScheduler(false), uniqueSolution(false), cachingEnabled(false), requirementsChecked(false) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolver<ValueType>::~MinMaxLinearEquationSolver() {
            // Intentionally left empty.
        }

        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::solveEquations(Environment const& env, OptimizationDirection d, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_WARN_COND_DEBUG(this->isRequirementsCheckedSet(), "The requirements of the solver have not been marked as checked. Please provide the appropriate check or mark the requirements as checked (if applicable).");
            return internalSolveEquations(Environment const& env, d, x, b);
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::solveEquations(Environment const& env, std::vector<ValueType>& x, std::vector<ValueType> const& b) const {
            STORM_LOG_THROW(isSet(this->direction), storm::exceptions::IllegalFunctionCallException, "Optimization direction not set.");
            solveEquations(Environment const& env, convert(this->direction), x, b);
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::repeatedMultiply(std::vector<ValueType>& x, std::vector<ValueType>* b, uint_fast64_t n) const {
            STORM_LOG_THROW(isSet(this->direction), storm::exceptions::IllegalFunctionCallException, "Optimization direction not set.");
            return repeatedMultiply(convert(this->direction), x, b, n);
        }

        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setOptimizationDirection(OptimizationDirection d) {
            direction = convert(d);
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::unsetOptimizationDirection() {
            direction = OptimizationDirectionSetting::Unset;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setHasUniqueSolution(bool value) {
            uniqueSolution = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasUniqueSolution() const {
            return uniqueSolution;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setTrackScheduler(bool trackScheduler) {
            this->trackScheduler = trackScheduler;
            if (!this->trackScheduler) {
                schedulerChoices = boost::none;
            }
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isTrackSchedulerSet() const {
            return this->trackScheduler;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasScheduler() const {
            return static_cast<bool>(schedulerChoices);
        }
        
        template<typename ValueType>
        storm::storage::Scheduler<ValueType> MinMaxLinearEquationSolver<ValueType>::computeScheduler() const {
            STORM_LOG_THROW(hasScheduler(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve scheduler, because none was generated.");
            storm::storage::Scheduler<ValueType> result(schedulerChoices->size());
            uint_fast64_t state = 0;
            for (auto const& schedulerChoice : schedulerChoices.get()) {
                result.setChoice(schedulerChoice, state);
                ++state;
            }
            return result;
        }
        
        template<typename ValueType>
        std::vector<uint_fast64_t> const& MinMaxLinearEquationSolver<ValueType>::getSchedulerChoices() const {
            STORM_LOG_THROW(hasScheduler(), storm::exceptions::IllegalFunctionCallException, "Cannot retrieve scheduler choices, because they were not generated.");
            return schedulerChoices.get();
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setCachingEnabled(bool value) {
            if(cachingEnabled && !value) {
                // caching will be turned off. Hence we clear the cache at this point
                clearCache();
            }
            cachingEnabled = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isCachingEnabled() const {
            return cachingEnabled;
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::clearCache() const {
            // Intentionally left empty.
        }
                
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setInitialScheduler(std::vector<uint_fast64_t>&& choices) {
            initialScheduler = std::move(choices);
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::hasInitialScheduler() const {
            return static_cast<bool>(initialScheduler);
        }
        
        template<typename ValueType>
        std::vector<uint_fast64_t> const& MinMaxLinearEquationSolver<ValueType>::getInitialScheduler() const {
            return initialScheduler.get();
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements MinMaxLinearEquationSolver<ValueType>::getRequirements(Environment const&, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            return MinMaxLinearEquationSolverRequirements();
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolver<ValueType>::setRequirementsChecked(bool value) {
            this->requirementsChecked = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolver<ValueType>::isRequirementsCheckedSet() const {
            return requirementsChecked;
        }
        
        template<typename ValueType>
        MinMaxLinearEquationSolverFactory<ValueType>::MinMaxLinearEquationSolverFactory() : requirementsChecked(false) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        void MinMaxLinearEquationSolverFactory<ValueType>::setRequirementsChecked(bool value) {
            this->requirementsChecked = value;
        }
        
        template<typename ValueType>
        bool MinMaxLinearEquationSolverFactory<ValueType>::isRequirementsCheckedSet() const {
            return this->requirementsChecked;
        }

        template<typename ValueType>
        MinMaxLinearEquationSolverRequirements MinMaxLinearEquationSolverFactory<ValueType>::getRequirements(Environment const& env, bool hasUniqueSolution, boost::optional<storm::solver::OptimizationDirection> const& direction) const {
            // Create dummy solver and ask it for requirements.
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> solver = this->create(env);
            solver->setHasUniqueSolution(hasUniqueSolution);
            return solver->getRequirements(env, direction);
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(Environment const& env, storm::storage::SparseMatrix<ValueType> const& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> solver = this->create(env);
            solver->setMatrix(matrix);
            return solver;
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> MinMaxLinearEquationSolverFactory<ValueType>::create(Environment const& env, storm::storage::SparseMatrix<ValueType>&& matrix) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> solver = this->create(env);
            solver->setMatrix(std::move(matrix));
            return solver;
        }
        
        template<typename ValueType>
        GeneralMinMaxLinearEquationSolverFactory<ValueType>::GeneralMinMaxLinearEquationSolverFactory() : MinMaxLinearEquationSolverFactory<ValueType>() {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> GeneralMinMaxLinearEquationSolverFactory<ValueType>::create(Environment const& env) const {
            std::unique_ptr<MinMaxLinearEquationSolver<ValueType>> result;
            auto method = env.solver().minMax().getMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::RationalSearch) {
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<ValueType>>(std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>());
            } else if (method == MinMaxMethod::Topological) {
                result = std::make_unique<TopologicalMinMaxLinearEquationSolver<ValueType>>();
            } else if (method == MinMaxMethod::LinearProgramming) {
                result = std::make_unique<LpMinMaxLinearEquationSolver<ValueType>>(std::make_unique<GeneralLinearEquationSolverFactory<ValueType>>(), std::make_unique<storm::utility::solver::LpSolverFactory<ValueType>>());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setRequirementsChecked(this->isRequirementsCheckedSet());
            return result;
        }

        template<>
        std::unique_ptr<MinMaxLinearEquationSolver<storm::RationalNumber>> GeneralMinMaxLinearEquationSolverFactory<storm::RationalNumber>::create(Environment const& env) const {
            std::unique_ptr<MinMaxLinearEquationSolver<storm::RationalNumber>> result;
            auto method = env.solver().minMax().getMethod();
            if (method == MinMaxMethod::ValueIteration || method == MinMaxMethod::PolicyIteration || method == MinMaxMethod::RationalSearch) {
                IterativeMinMaxLinearEquationSolverSettings<storm::RationalNumber> iterativeSolverSettings;
                result = std::make_unique<IterativeMinMaxLinearEquationSolver<storm::RationalNumber>>(std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>());
            } else if (method == MinMaxMethod::LinearProgramming) {
                result = std::make_unique<LpMinMaxLinearEquationSolver<storm::RationalNumber>>(std::make_unique<GeneralLinearEquationSolverFactory<storm::RationalNumber>>(), std::make_unique<storm::utility::solver::LpSolverFactory<storm::RationalNumber>>());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidSettingsException, "Unsupported technique.");
            }
            result->setRequirementsChecked(this->isRequirementsCheckedSet());
            return result;
        }

        template class MinMaxLinearEquationSolver<float>;
        template class MinMaxLinearEquationSolver<double>;
        
        template class MinMaxLinearEquationSolverFactory<double>;
        template class GeneralMinMaxLinearEquationSolverFactory<double>;

#ifdef STORM_HAVE_CARL
        template class MinMaxLinearEquationSolver<storm::RationalNumber>;
        template class MinMaxLinearEquationSolverFactory<storm::RationalNumber>;
        template class GeneralMinMaxLinearEquationSolverFactory<storm::RationalNumber>;
#endif
    }
}
