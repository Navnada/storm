#include <iostream>

namespace storm {
    namespace counterexamples {
        template <class T>
        class MyDTMCCounterexample {
        public:
            //static bool computeCounterexample();
            static bool computeCounterexample(int x) {
                int y = 2*x;
                meandyou z;
                z.me = y;
                z.you = nullptr;
                std::cout << "Counterexamples " << z.me << std::endl;
                return true;
            }

            struct meandyou {
                int me;
                meandyou* you;
            };
        };
    } // namespace counterexamples
} // namespace storm