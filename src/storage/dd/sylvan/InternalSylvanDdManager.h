#ifndef STORM_STORAGE_DD_INTERNALSYLVANDDMANAGER_H_
#define STORM_STORAGE_DD_INTERNALSYLVANDDMANAGER_H_

#include "src/storage/dd/DdType.h"
#include "src/storage/dd/InternalDdManager.h"

#include "src/storage/dd/sylvan/InternalSylvanBdd.h"
#include "src/storage/dd/sylvan/InternalSylvanAdd.h"

namespace storm {
    namespace dd {
        template<DdType LibraryType, typename ValueType>
        class InternalAdd;
        
        template<DdType LibraryType>
        class InternalBdd;
        
        template<>
        class InternalDdManager<DdType::Sylvan> {
        public:
            friend class InternalBdd<DdType::Sylvan>;
            
            template<DdType LibraryType, typename ValueType>
            friend class InternalAdd;
            
            /*!
             * Creates a new internal manager for CUDD DDs.
             */
            InternalDdManager();
            
            /*!
             * Retrieves a BDD representing the constant one function.
             *
             * @return A BDD representing the constant one function.
             */
            InternalBdd<DdType::Sylvan> getBddOne() const;
            
            /*!
             * Retrieves an ADD representing the constant one function.
             *
             * @return An ADD representing the constant one function.
             */
            template<typename ValueType>
            InternalAdd<DdType::Sylvan, ValueType> getAddOne() const;
            
            /*!
             * Retrieves a BDD representing the constant zero function.
             *
             * @return A BDD representing the constant zero function.
             */
            InternalBdd<DdType::Sylvan> getBddZero() const;
            
            /*!
             * Retrieves an ADD representing the constant zero function.
             *
             * @return An ADD representing the constant zero function.
             */
            template<typename ValueType>
            InternalAdd<DdType::Sylvan, ValueType> getAddZero() const;
            
            /*!
             * Retrieves an ADD representing the constant function with the given value.
             *
             * @return An ADD representing the constant function with the given value.
             */
            template<typename ValueType>
            InternalAdd<DdType::Sylvan, ValueType> getConstant(ValueType const& value) const;
            
            /*!
             * Creates a new pair of DD variables and returns the two cubes as a result.
             *
             * @return The two cubes belonging to the DD variables.
             */
            std::pair<InternalBdd<DdType::Sylvan>, InternalBdd<DdType::Sylvan>> createNewDdVariablePair();
            
            /*!
             * Sets whether or not dynamic reordering is allowed for the DDs managed by this manager.
             *
             * @param value If set to true, dynamic reordering is allowed and forbidden otherwise.
             */
            void allowDynamicReordering(bool value);
            
            /*!
             * Retrieves whether dynamic reordering is currently allowed.
             *
             * @return True iff dynamic reordering is currently allowed.
             */
            bool isDynamicReorderingAllowed() const;
            
            /*!
             * Triggers a reordering of the DDs managed by this manager.
             */
            void triggerReordering();
            
        private:

        };
    }
}

#endif /* STORM_STORAGE_DD_INTERNALSYLVANDDMANAGER_H_ */