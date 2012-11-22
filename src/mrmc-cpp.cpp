/*
 *	MRMC - C++ Rebuild
 *	
 *	MRMC is a model checker for discrete-time and continuous-time Markov
 *	reward models. It supports reward extensions of PCTL and CSL (PRCTL
 *	and CSRL), and allows for the automated verification of properties
 *	concerning long-run and instantaneous rewards as well as cumulative
 *	rewards.
 *	
 *  Authors: Philipp Berger
 *
 *  Description: Central part of the application containing the main() Method
 */

#include <iostream>
#include <cstdio>

/* PlatformSTL Header Files */
#include <platformstl/performance/performance_counter.hpp>

#include <pantheios/pantheios.hpp>
#include <pantheios/backends/bec.file.h>
PANTHEIOS_EXTERN_C PAN_CHAR_T const PANTHEIOS_FE_PROCESS_IDENTITY[] = "mrmc-cpp";

#include "MRMCConfig.h"
#include "src/models/dtmc.h"
#include "src/sparse/static_sparse_matrix.h"
#include "src/models/atomic_propositions_labeling.h"
#include "src/parser/read_lab_file.h"
#include "src/parser/read_tra_file.h"
#include "src/utility/settings.h"
#include "Eigen/Sparse"

#include "src/exceptions/InvalidSettings.h"
 
int main(const int argc, const char* argv[]) {
	// Logging init
	pantheios_be_file_setFilePath("log.all");
	pantheios::log_INFORMATIONAL("MRMC-Cpp started.");
	
	mrmc::settings::Settings* s = NULL;
	
	try
	{
		s = mrmc::settings::Settings::instance(argc, argv, NULL);
	}
	catch (mrmc::exceptions::InvalidSettings)
	{
		std::cout << "Could not recover from settings error, terminating." << std::endl << std::endl;
		std::cout << mrmc::settings::help << std::endl;
		return 1;
	}
	
	if (s->isSet("help"))
	{
		std::cout << mrmc::settings::help << std::endl;
		return 0;
	}
	if (s->isSet("help-config"))
	{
		std::cout << mrmc::settings::helpConfigfile << std::endl;
		return 0;
	}

	mrmc::sparse::StaticSparseMatrix<double>* probMatrix = mrmc::parser::read_tra_file(s->getString("trafile").c_str());
	mrmc::models::AtomicPropositionsLabeling* labeling = mrmc::parser::read_lab_file(probMatrix->getRowCount(), s->getString("labfile").c_str());
	mrmc::models::Dtmc<double> dtmc(probMatrix, labeling);



	return 0;
}

