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

        private:
            storm::models::sparse::Dtmc<ValueType> const& dtmc;
        };
    } // namespace counterexamples
} // namespace storm
