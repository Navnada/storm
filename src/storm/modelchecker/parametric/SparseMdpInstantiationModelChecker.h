#pragma once

#include <memory>

#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/parametric/SparseInstantiationModelChecker.h"
#include "storm/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "storm/utility/ModelInstantiator.h"
#include "storm/storage/TotalScheduler.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            /*!
             * Class to efficiently check a formula on a parametric model with different parameter instantiations
             */
            template <typename SparseModelType, typename ConstantType>
            class SparseMdpInstantiationModelChecker : public SparseInstantiationModelChecker<SparseModelType, ConstantType> {
            public:
                SparseMdpInstantiationModelChecker(SparseModelType const& parametricModel);
                
                virtual std::unique_ptr<CheckResult> check(storm::utility::parametric::Valuation<typename SparseModelType::ValueType> const& valuation) override;

            protected:
                // Considers the result of the last check as a hint for the current check
                std::unique_ptr<CheckResult> checkWithResultHint(storm::modelchecker::SparseMdpPrctlModelChecker<storm::models::sparse::Mdp<ConstantType>>& modelChecker);
                
                storm::utility::ModelInstantiator<SparseModelType, storm::models::sparse::Mdp<ConstantType>> modelInstantiator;
            };
        }
    }
}
