/*
* AntiDupl Dynamic-Link Library.
*
* Copyright (c) 2016 Borisov Dmitry.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy 
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
* copies of the Software, and to permit persons to whom the Software is 
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in 
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "adNeuralNetwork.h"
#include "adStrings.h"
#include "adStatisticsOfDeleting.h"
#include "OpenNN/opennn.h"

using namespace OpenNN;

namespace ad
{
	TNeuralNetwork::TNeuralNetwork(void)
	{
	}

	TNeuralNetwork::~TNeuralNetwork(void)
	{
	}

	bool TNeuralNetwork::GetPredict(TResult *pResult)
	{
		return true;
	}

	adError TNeuralNetwork::Train()
	{
		TString path = TStatisticsOfDeleting::GetStatisticsPath();

		DataSet data_set;
	      
		data_set.load_data(path.ToString());

		  // Variables information

		  /*VariablesInformation* variables_information_pointer = data_set.get_variables_information_pointer(); 

		  //variables_information_pointer->set(1, 8);

		  variables_information_pointer->set_name(0, "pregnant");

		  variables_information_pointer->set_name(1, "glucose");

		  variables_information_pointer->set_name(2, "pressure");
		  variables_information_pointer->set_units(2, "mmHg");

		  variables_information_pointer->set_name(3, "thickness");
		  variables_information_pointer->set_units(3, "mm");

		  variables_information_pointer->set_name(4, "insulin");
		  variables_information_pointer->set_units(4, "muU/ml");

		  variables_information_pointer->set_name(5, "mass_index");
		  variables_information_pointer->set_units(5, "kg/m2");

		  variables_information_pointer->set_name(6, "pedigree");	  

		  variables_information_pointer->set_name(7, "age");	  

		  variables_information_pointer->set_name(8, "diabetes");	  

		  const Vector< Vector<std::string> > inputs_targets_information = variables_information_pointer->arrange_inputs_targets_information();

		  // Instances information

		  InstancesInformation* instances_information_pointer = data_set.get_instances_information_pointer();

		  instances_information_pointer->split_random_indices(0.75, 0.0, 0.25);

		  //const Vector< Vector<double> > inputs_targets_statistics = data_set.scale_inputs_minimum_maximum();
		  const Vector< Vector<double> > inputs_targets_statistics = data_set.scale_inputs_targets();  

		  // Neural network 
		  
		  //NeuralNetwork neural_network(8, 6, 1);

		  const unsigned int inputs_number = data_set.get_variables_information().count_inputs_number();
		  const unsigned int hidden_neurons_number = 6;
		  const unsigned int outputs_number = data_set.get_variables_information().count_targets_number();

		  NeuralNetwork neural_network(inputs_number, hidden_neurons_number, outputs_number);

		  neural_network.set_inputs_outputs_information(inputs_targets_information); 
		  neural_network.set_inputs_outputs_statistics(inputs_targets_statistics); 

		  neural_network.set_scaling_unscaling_layers_flag(false);
		  //neural_network.set_scaling_unscaling_layers_flag(true);

		  // Performance functional

		  PerformanceFunctional performance_functional(&neural_network, &data_set);

		  // Training strategy

		  TrainingStrategy training_strategy(&performance_functional);

		  QuasiNewtonMethod* quasi_Newton_method_pointer = new QuasiNewtonMethod(&performance_functional);
		  quasi_Newton_method_pointer->set_minimum_performance_increase(1.0e-6);
		  quasi_Newton_method_pointer->set_reserve_evaluation_history(true);
		  //quasi_Newton_method_pointer->set_maximum_epochs_number(10000);
		  training_strategy.set_main_training_algorithm_pointer(quasi_Newton_method_pointer);
		  
		  std::cout << "initialization training: " << training_strategy.write_initialization_training_algorithm_type() << std::endl;
		  std::cout << "main training: " << training_strategy.write_main_training_algorithm_type() << std::endl;
		  std::cout << "refinement training: " << training_strategy.write_refinement_training_algorithm_type() << std::endl;

		  TrainingStrategy::Results training_strategy_results = training_strategy.perform_training();

		  neural_network.set_inputs_scaling_outputs_unscaling_methods("MinimumMaximum");
		  neural_network.set_scaling_unscaling_layers_flag(true);

		  // Testing analysis

		  TestingAnalysis testing_analysis(&neural_network, &data_set);

		  testing_analysis.construct_pattern_recognition_testing();

		  PatternRecognitionTesting* pattern_recognition_testing_pointer = testing_analysis.get_pattern_recognition_testing_pointer();

		  // Save results

		  data_set.save("pima_indians_diabetes/data_set.xml");

		  neural_network.save("pima_indians_diabetes/neural_network.xml");
		  neural_network.save_expression("pima_indians_diabetes/expression.txt");

		  training_strategy.save("pima_indians_diabetes/training_strategy.xml");
		  training_strategy_results.save("pima_indians_diabetes/training_strategy_results.dat");

		  pattern_recognition_testing_pointer->save_confusion("pima_indians_diabetes/confusion.dat");   
		  pattern_recognition_testing_pointer->save_binary_classification_test("pima_indians_diabetes/binary_classification_test.dat");  
		  */
		return adError::AD_OK;
	}
}