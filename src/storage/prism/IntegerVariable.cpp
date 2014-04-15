#include "src/storage/prism/IntegerVariable.h"

namespace storm {
    namespace prism {
        IntegerVariable::IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, std::string const& filename, uint_fast64_t lineNumber) : Variable(variableName, lowerBoundExpression, true, filename, lineNumber), lowerBoundExpression(lowerBoundExpression), upperBoundExpression(upperBoundExpression) {
            // Intentionally left empty.
        }

        IntegerVariable::IntegerVariable(std::string const& variableName, storm::expressions::Expression const& lowerBoundExpression, storm::expressions::Expression const& upperBoundExpression, storm::expressions::Expression const& initialValueExpression, std::string const& filename, uint_fast64_t lineNumber) : Variable(variableName, initialValueExpression, false, filename, lineNumber), lowerBoundExpression(lowerBoundExpression), upperBoundExpression(upperBoundExpression) {
            // Intentionally left empty.
        }
        
        storm::expressions::Expression const& IntegerVariable::getLowerBoundExpression() const {
            return this->lowerBoundExpression;
        }
        
        storm::expressions::Expression const& IntegerVariable::getUpperBoundExpression() const {
            return this->upperBoundExpression;
        }
        
        IntegerVariable IntegerVariable::substitute(std::map<std::string, storm::expressions::Expression> const& substitution) const {
            return IntegerVariable(this->getName(), this->getLowerBoundExpression().substitute<std::map>(substitution), this->getUpperBoundExpression().substitute<std::map>(substitution), this->getInitialValueExpression().substitute<std::map>(substitution), this->getFilename(), this->getLineNumber());
        }
        
        std::ostream& operator<<(std::ostream& stream, IntegerVariable const& variable) {
            stream << variable.getName() << ": [" << variable.getLowerBoundExpression() << ".." << variable.getUpperBoundExpression() << "]" << " init " << variable.getInitialValueExpression() << ";";
            return stream;
        }
    } // namespace prism
} // namespace storm