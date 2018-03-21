#include "storm/settings/modules/CounterexampleGeneratorSettings.h"

#include "storm/settings/SettingsManager.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/settings/Option.h"
#include "storm/settings/OptionBuilder.h"
#include "storm/settings/ArgumentBuilder.h"
#include "storm/settings/Argument.h"
#include "storm/settings/modules/IOSettings.h"

namespace storm {
    namespace settings {
        namespace modules {
            
            const std::string CounterexampleGeneratorSettings::moduleName = "counterexample";
            const std::string CounterexampleGeneratorSettings::formatOptionName = "format";
            const std::string CounterexampleGeneratorSettings::highLevelOptionName = "highlevel";
            const std::string CounterexampleGeneratorSettings::encodeReachabilityOptionName = "encreach";
            const std::string CounterexampleGeneratorSettings::schedulerCutsOptionName = "schedcuts";
            const std::string CounterexampleGeneratorSettings::noDynamicConstraintsOptionName = "nodyn";

            CounterexampleGeneratorSettings::CounterexampleGeneratorSettings() : ModuleSettings(moduleName) {
                std::vector<std::string> formats = {"highlevel", "paths"};
                this->addOption(storm::settings::OptionBuilder(moduleName, formatOptionName, true, "Selects the format to use for counterexample generation.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("format", "Sets which format to use.").setDefaultValueString("highlevel").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(formats)).build()).build());

                std::vector<std::string> techniques = {"maxsat", "milp"};
                this->addOption(storm::settings::OptionBuilder(moduleName, highLevelOptionName, true, "Computes a counterexample for the given model in terms of a minimal command/edge set. Note that this requires the model to be given in a symbolic format.")
                                .addArgument(storm::settings::ArgumentBuilder::createStringArgument("technique", "Sets which technique is used to derive the counterexample.").setDefaultValueString("maxsat").addValidatorString(ArgumentValidatorFactory::createMultipleChoiceValidator(techniques)).build()).build());
                this->addOption(storm::settings::OptionBuilder(moduleName, encodeReachabilityOptionName, true, "Sets whether to encode reachability for MAXSAT-based counterexample generation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, schedulerCutsOptionName, true, "Sets whether to add the scheduler cuts for MILP-based counterexample generation.").build());
                this->addOption(storm::settings::OptionBuilder(moduleName, noDynamicConstraintsOptionName, true, "Disables the generation of dynamic constraints in the MAXSAT-based counterexample generation.").build());
            }
            
            CounterexampleGeneratorSettings::Format CounterexampleGeneratorSettings::getFormat() const {
                std::string formatAsString = this->getOption(formatOptionName).getArgumentByName("format").getValueAsString();
                if (formatAsString == "highlevel") {
                    return CounterexampleGeneratorSettings::Format::HighLevel;
                } else {
                    return CounterexampleGeneratorSettings::Format::Paths;
                }
            }
            
            CounterexampleGeneratorSettings::HighLevelTechnique CounterexampleGeneratorSettings::getHighLevelTechnique() const {
                std::string techniqueAsString = this->getOption(formatOptionName).getArgumentByName("technique").getValueAsString();
                if (techniqueAsString == "maxsat") {
                    return CounterexampleGeneratorSettings::HighLevelTechnique::MaxSat;
                } else {
                    return CounterexampleGeneratorSettings::HighLevelTechnique::Milp;
                }
            }
            
            bool CounterexampleGeneratorSettings::isEncodeReachabilitySet() const {
                return this->getOption(encodeReachabilityOptionName).getHasOptionBeenSet();
            }
            
            bool CounterexampleGeneratorSettings::isUseSchedulerCutsSet() const {
                return this->getOption(schedulerCutsOptionName).getHasOptionBeenSet();
            }

            bool CounterexampleGeneratorSettings::isUseDynamicConstraintsSet() const {
                return !this->getOption(noDynamicConstraintsOptionName).getHasOptionBeenSet();
            }

            bool CounterexampleGeneratorSettings::check() const {
                return true;
            }
            
        } // namespace modules
    } // namespace settings
} // namespace storm
