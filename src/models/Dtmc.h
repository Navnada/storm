/*
 * Dtmc.h
 *
 *  Created on: 14.11.2012
 *      Author: Christian Dehnert
 */

#ifndef STORM_MODELS_DTMC_H_
#define STORM_MODELS_DTMC_H_

#include <ostream>
#include <iostream>
#include <memory>
#include <cstdlib>

#include "AbstractDeterministicModel.h"
#include "AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/settings/Settings.h"

namespace storm {

namespace models {

/*!
 * This class represents a discrete-time Markov chain (DTMC) whose states are
 * labeled with atomic propositions.
 */
template <class T>
class Dtmc : public storm::models::AbstractDeterministicModel<T> {

public:
	/*!
	 * Constructs a DTMC object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are copied.
	 * @param probabilityMatrix The matrix representing the transitions in the model.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 * @param stateRewardVector The reward values associated with the states.
	 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
	 */
	Dtmc(storm::storage::SparseMatrix<T> const& probabilityMatrix, storm::models::AtomicPropositionsLabeling const& stateLabeling,
				boost::optional<std::vector<T>> const& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>> const& optionalTransitionRewardMatrix,
                boost::optional<std::vector<std::set<uint_fast64_t>>> const& optionalChoiceLabeling)
			: AbstractDeterministicModel<T>(probabilityMatrix, stateLabeling, optionalStateRewardVector, optionalTransitionRewardMatrix, optionalChoiceLabeling) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
		if (this->hasTransitionRewards()) {
			if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
				LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
				throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
			}
		}
	}

	/*!
	 * Constructs a DTMC object from the given transition probability matrix and
	 * the given labeling of the states.
	 * All values are moved.
	 * @param probabilityMatrix The matrix representing the transitions in the model.
	 * @param stateLabeling The labeling that assigns a set of atomic
	 * propositions to each state.
	 * @param stateRewardVector The reward values associated with the states.
	 * @param transitionRewardMatrix The reward values associated with the transitions of the model.
	 */
	Dtmc(storm::storage::SparseMatrix<T>&& probabilityMatrix, storm::models::AtomicPropositionsLabeling&& stateLabeling,
				boost::optional<std::vector<T>>&& optionalStateRewardVector, boost::optional<storm::storage::SparseMatrix<T>>&& optionalTransitionRewardMatrix,
                boost::optional<std::vector<std::set<uint_fast64_t>>>&& optionalChoiceLabeling)
				// The std::move call must be repeated here because otherwise this calls the copy constructor of the Base Class
			: AbstractDeterministicModel<T>(std::move(probabilityMatrix), std::move(stateLabeling), std::move(optionalStateRewardVector), std::move(optionalTransitionRewardMatrix),
                                            std::move(optionalChoiceLabeling)) {
		if (!this->checkValidityOfProbabilityMatrix()) {
			LOG4CPLUS_ERROR(logger, "Probability matrix is invalid.");
			throw storm::exceptions::InvalidArgumentException() << "Probability matrix is invalid.";
		}
		if (this->hasTransitionRewards()) {
			if (!this->getTransitionRewardMatrix().isSubmatrixOf(this->getTransitionMatrix())) {
				LOG4CPLUS_ERROR(logger, "Transition reward matrix is not a submatrix of the transition matrix, i.e. there are rewards for transitions that do not exist.");
				throw storm::exceptions::InvalidArgumentException() << "There are transition rewards for nonexistent transitions.";
			}
		}
	}

	/*!
	 * Copy Constructor. Performs a deep copy of the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be copied.
	 */
	Dtmc(Dtmc<T> const & dtmc) : AbstractDeterministicModel<T>(dtmc) {
		// Intentionally left empty.
	}

	/*!
	 * Move Constructor. Performs a move on the given DTMC.
	 * @param dtmc A reference to the DTMC that is to be moved.
	 */
	Dtmc(Dtmc<T>&& dtmc) : AbstractDeterministicModel<T>(std::move(dtmc)) {
		// Intentionally left empty.
	}

	//! Destructor
	/*!
	 * Destructor.
	 */
	~Dtmc() {
		// Intentionally left empty.
	}
	
	storm::models::ModelType getType() const {
		return DTMC;
	}

	/*!
	 * Calculates a hash over all values contained in this Model.
	 * @return size_t A Hash Value
	 */
	virtual std::size_t getHash() const override {
		return AbstractDeterministicModel<T>::getHash();
	}

	/*!
	 * Generates a sub-Dtmc of this Dtmc induced by the states specified by the bitvector.
	 * E.g. a Dtmc that is partial isomorph (on the given states) to this one.
	 * @param subSysStates A BitVector where all states that should be kept are indicated 
	 *                     by a set bit of the corresponding index.
	 *                     Waring: If the vector does not have the correct size, it will be resized.
	 * @return The sub-Dtmc.
	 */
	storm::models::Dtmc<T> getSubDtmc(storm::storage::BitVector& subSysStates) {


		// Is there any state in the subsystem?
		if(subSysStates.getNumberOfSetBits() == 0) {
			LOG4CPLUS_ERROR(logger, "No states in subsystem!");
			return storm::models::Dtmc<T>(storm::storage::SparseMatrix<T>(0),
					  	  	  	  	  	  storm::models::AtomicPropositionsLabeling(this->getStateLabeling(), subSysStates),
					  	  	  	  	  	  boost::optional<std::vector<T>>(),
					  	  	  	  	  	  boost::optional<storm::storage::SparseMatrix<T>>(),
					  	  	  	  	  	  boost::optional<std::vector<std::set<uint_fast64_t>>>());
		}

		// Does the vector have the right size?
		if(subSysStates.getSize() != this->getNumberOfStates()) {
			LOG4CPLUS_INFO(logger, "BitVector has wrong size. Resizing it...");
			subSysStates.resize(this->getNumberOfStates());
		}

		// Test if it is a proper subsystem of this Dtmc, i.e. if there is at least one state to be left out.
		if(subSysStates.getNumberOfSetBits() == subSysStates.getSize()) {
			LOG4CPLUS_INFO(logger, "All states are kept. This is no proper subsystem.");
			return storm::models::Dtmc<T>(*this);
		}

		// 1. Get all necessary information from the old transition matrix
		storm::storage::SparseMatrix<T> const & origMat = this->getTransitionMatrix();
		uint_fast64_t const stateCount = origMat.getColumnCount();

		// Iterate over all rows. Count the number of all transitions from the old system to be 
		// transfered to the new one. Also build a mapping from the state number of the old system 
		// to the state number of the new system.
		uint_fast64_t subSysTransitionCount = 0;
		uint_fast64_t row = 0;
		uint_fast64_t newRow = 0;
		std::vector<uint_fast64_t> stateMapping;
		for(auto iter = origMat.begin(); iter != origMat.end(); ++iter) {
			if(subSysStates.get(row)){
				for(auto colIter = iter.begin(); colIter != iter.end(); ++colIter) {
					if(subSysStates.get(colIter.column())) {
						subSysTransitionCount++;	
					} 
				}
				stateMapping.push_back(newRow);
				newRow++;
			} else {
				stateMapping.push_back((uint_fast64_t) -1);
			}
			row++;
		}

		// 2. Construct transition matrix

		// Take all states indicated by the vector as well as one additional state as target of 
		// all transitions that target a state that is not kept.
		uint_fast64_t const newStateCount = subSysStates.getNumberOfSetBits() + 1;
		storm::storage::SparseMatrix<T> newMat(newStateCount);

		// The number of transitions of the new Dtmc is the number of transitions transfered 
		// from the old one plus one transition for each state to the new one.
		newMat.initialize(subSysTransitionCount + newStateCount);

		// Now fill the matrix.
		newRow = 0;
		row = 0;
		T rest = 0;
		for(auto iter = origMat.begin(); iter != origMat.end(); ++iter) {
			if(subSysStates.get(row)){
				// Transfer transitions
				for(auto colIter = iter.begin(); colIter != iter.end(); ++colIter) {
					if(subSysStates.get(colIter.column())) {
						newMat.addNextValue(newRow, stateMapping[colIter.column()], colIter.value());
					} else {
						rest += colIter.value();
					}
				}

				// Insert the transition taking care of the remaining outgoing probability.
				newMat.addNextValue(newRow, newStateCount - 1, rest);
				rest = (T) 0;

				newRow++;
			}
			row++;
		}

		// Insert last transition: self loop on added state
		newMat.addNextValue(newStateCount - 1, newStateCount - 1, (T) 1);

		newMat.finalize(false);

		// 3. Take care of the labeling
		storm::models::AtomicPropositionsLabeling newLabeling = storm::models::AtomicPropositionsLabeling(this->getStateLabeling(), subSysStates);
		newLabeling.addState();
		//newLabeling.addAtomicProposition();

		// 4. Make Dtmc from the matrix and return it
		//TODO: test the labeling, handle optionals.
		return storm::models::Dtmc<T>(newMat, 
									  newLabeling,
									  boost::optional<std::vector<T>>(),
									  boost::optional<storm::storage::SparseMatrix<T>>(),
									  boost::optional<std::vector<std::set<uint_fast64_t>>>()
									  );

	}

private:
	/*!
	 *	@brief Perform some sanity checks.
	 *
	 *	Checks probability matrix if all rows sum up to one.
	 */
	bool checkValidityOfProbabilityMatrix() {
		// Get the settings object to customize linear solving.
		storm::settings::Settings* s = storm::settings::Settings::getInstance();
		double precision = s->getOptionByLongName("precision").getArgument(0).getValueAsDouble();

		if (this->getTransitionMatrix().getRowCount() != this->getTransitionMatrix().getColumnCount()) {
			// not square
			LOG4CPLUS_ERROR(logger, "Probability matrix is not square.");
			return false;
		}

		for (uint_fast64_t row = 0; row < this->getTransitionMatrix().getRowCount(); ++row) {
			T sum = this->getTransitionMatrix().getRowSum(row);
			if (sum == 0) {
				LOG4CPLUS_ERROR(logger, "Row " << row << " has sum 0");
				return false;
			}
			if (std::abs(sum - 1) > precision) {
				LOG4CPLUS_ERROR(logger, "Row " << row << " has sum " << sum);
				return false;
			}
		}
		return true;
	}
};

} // namespace models

} // namespace storm

#endif /* STORM_MODELS_DTMC_H_ */
