#include "storm/counterexamples/MyDTMCCounterexample.h"

namespace storm {
    namespace counterexamples {
        template class MyDTMCCounterexample<double>;
        /*static bool computeCounterexample() {
                std::cout << "Counterexamples" << std::endl;
                return true;
                
        }*/
    }
}