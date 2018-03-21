#ifndef STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_
#define STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_

#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            /*!
             * This class represents the settings for counterexample generation.
             */
            class CounterexampleGeneratorSettings : public ModuleSettings {
            public:
                enum class Format {
                    Paths, HighLevel
                };
                
                enum class HighLevelTechnique {
                    MaxSat, Milp
                };
                
                /*!
                 * Creates a new set of counterexample settings.
                 */
                CounterexampleGeneratorSettings();
                
                /*!
                 * Retrieves the selected format for counterexamples.
                 */
                Format getFormat() const;
                
                /*!
                 * Retrieves the technique selected for high-level counterexamples
                 */
                HighLevelTechnique getHighLevelTechnique() const;
                
                /*!
                 * Retrieves whether reachability of a target state is to be encoded if the MAXSAT-based technique is
                 * used to generate a minimal command set counterexample.
                 *
                 * @return True iff reachability of a target state is to be encoded.
                 */
                bool isEncodeReachabilitySet() const;
                
                /*!
                 * Retrieves whether scheduler cuts are to be used if the MAXSAT-based technique is used to generate a
                 * minimal command set counterexample.
                 *
                 * @return True iff scheduler cuts are to be used.
                 */
                bool isUseSchedulerCutsSet() const;
                
                /*!
                 * Retrieves whether to use the dynamic constraints in the MAXSAT-based technique.
                 *
                 * @return True iff dynamic constraints are to be used.
                 */
                bool isUseDynamicConstraintsSet() const;
                
                bool check() const override;
                
                // The name of the module.
                static const std::string moduleName;
                
            private:
                // Define the string names of the options as constants.
                static const std::string formatOptionName;
                static const std::string highLevelOptionName;
                static const std::string encodeReachabilityOptionName;
                static const std::string schedulerCutsOptionName;
                static const std::string noDynamicConstraintsOptionName;
            };
            
        } // namespace modules
    } // namespace settings
} // namespace storm

#endif /* STORM_SETTINGS_MODULES_COUNTEREXAMPLEGENERATORSETTINGS_H_ */
