//test_calo_trigger_algorithm.cxx
// Standard libraries :
#include <iostream>

// GSL:
#include <bayeux/mygsl/rng.h>

// - Bayeux/datatools:
#include <datatools/utils.h>
#include <datatools/io_factory.h>
#include <datatools/clhep_units.h>
// - Bayeux/mctools:
#include <mctools/simulated_data.h>
// - Bayeux/dpp:
#include <dpp/input_module.h>

// Falaise:
#include <falaise/falaise.h>

// This project :
#include <snemo/digitization/clock_utils.h>
#include <snemo/digitization/sd_to_calo_signal_algo.h>
#include <snemo/digitization/signal_to_calo_tp_algo.h>
#include <snemo/digitization/calo_tp_to_ctw_algo.h>
#include <snemo/digitization/calo_trigger_algorithm.h>

int main( int  argc_ , char **argv_  )
{
  FALAISE_INIT();
  int error_code = EXIT_SUCCESS;
  datatools::logger::priority logging = datatools::logger::PRIO_FATAL;


 int iarg = 1;
  bool is_input_file = false;
  std::string input_filename;
  while (iarg < argc_) {
    std::string arg = argv_[iarg];
    if (arg == "-i" || arg == "--input") {
      is_input_file = true;
      input_filename=argv_[++iarg];
    } else if (arg == "-f" || arg == "--filename") {
      input_filename=argv_[++iarg];
    }
    iarg++;
  }
  

  try {
    std::clog << "Test program for class 'snemo::digitization::calo_trigger_algorithm' !" << std::endl;
    int32_t seed = 314159;
    mygsl::rng random_generator;
    random_generator.initialize(seed);
    
    std::string manager_config_file;
    
    manager_config_file = "@falaise:config/snemo/demonstrator/geometry/3.0/manager.conf";
    datatools::fetch_path_with_env(manager_config_file);
    datatools::properties manager_config;
    datatools::properties::read_config (manager_config_file,
					manager_config);
    geomtools::manager my_manager;
    manager_config.update ("build_mapping", true);
    if (manager_config.has_key ("mapping.excluded_categories"))
      {
	manager_config.erase ("mapping.excluded_categories");
      }
    my_manager.initialize (manager_config);

    std::string pipeline_simulated_data_filename;
    std::string SD_bank_label = "SD";

    if(is_input_file){
      pipeline_simulated_data_filename = input_filename;
    }else{
      //pipeline_simulated_data_filename = "${FALAISE_DIGITIZATION_TESTING_DIR}/data/Se82_0nubb-source_strips_bulk_SD_10_events.brio";
      pipeline_simulated_data_filename = "/home/guillaume/data/my_falaise/outputs/Se82_0nubb-source_strips_bulk_SD.brio";
    }
    datatools::fetch_path_with_env(pipeline_simulated_data_filename);

    
    dpp::input_module reader;
    datatools::properties reader_config;
    reader_config.store ("logging.priority", "debug");
    reader_config.store ("max_record_total", 30);
    reader_config.store ("files.mode", "single");
    reader_config.store ("files.single.filename", pipeline_simulated_data_filename);
    reader.initialize_standalone (reader_config);
    reader.tree_dump (std::clog, "Simulated data reader module");

    datatools::things ER;

    // Loading memory from external files
    // std::string memory_mult_layer   = "${FALAISE_DIGITIZATION_TESTING_DIR}/config/trigger/tracker/A5_D1_default_min_mult_memory.data";
    // datatools::fetch_path_with_env(memory_lvl1_to_lvl2);

    snemo::digitization::electronic_mapping my_e_mapping;
    my_e_mapping.set_geo_manager(my_manager);
    my_e_mapping.set_module_number(snemo::digitization::mapping::DEMONSTRATOR_MODULE_NUMBER);
    my_e_mapping.initialize();

    snemo::digitization::clock_utils my_clock_manager;
    my_clock_manager.initialize();
    
    int psd_count = 0;
    while (!reader.is_terminated())
      {
	reader.process(ER);
	// A plain `mctools::simulated_data' object is stored here :
	if (ER.has(SD_bank_label) && ER.is_a<mctools::simulated_data>(SD_bank_label)) 
	  {
	    // Access to the "SD" bank with a stored `mctools::simulated_data' :
	    const mctools::simulated_data & SD = ER.get<mctools::simulated_data>(SD_bank_label);
	    snemo::digitization::sd_to_calo_signal_algo sd_2_calo_signal(my_manager);
	    sd_2_calo_signal.initialize();

	    my_clock_manager.compute_clockticks_ref(random_generator);
	    int32_t clocktick_25_reference  = my_clock_manager.get_clocktick_25_ref();
	    double  clocktick_25_shift      = my_clock_manager.get_clocktick_25_shift();
	    int32_t clocktick_800_reference = my_clock_manager.get_clocktick_800_ref();
	    double  clocktick_800_shift     = my_clock_manager.get_clocktick_800_shift();

	    snemo::digitization::signal_to_calo_tp_algo signal_2_calo_tp;
	    signal_2_calo_tp.initialize(my_e_mapping);
	    signal_2_calo_tp.set_clocktick_reference(clocktick_25_reference);
	    signal_2_calo_tp.set_clocktick_shift(clocktick_25_shift);	 
	    snemo::digitization::signal_data signal_data;
	    if (SD.has_step_hits("calo") || SD.has_step_hits("xcalo"))
	      {
		sd_2_calo_signal.process(SD, signal_data);
	      
		snemo::digitization::calo_tp_data my_calo_tp_data;

		if( signal_data.has_calo_signals())
		  {
		    signal_2_calo_tp.process(signal_data, my_calo_tp_data);
		    my_calo_tp_data.tree_dump(std::clog, "Calorimeter TP(s) data : ", "INFO : ");
		  }
	    
		snemo::digitization::calo_ctw_data my_calo_ctw_data;
		snemo::digitization::calo_tp_to_ctw_algo calo_tp_2_ctw;
		calo_tp_2_ctw.initialize();

		calo_tp_2_ctw.process(my_calo_tp_data, my_calo_ctw_data);
		my_calo_ctw_data.tree_dump(std::clog, "Calorimeter CTW(s) data : ", "INFO : ");
		snemo::digitization::calo_trigger_algorithm my_calo_algo;
		my_calo_algo.set_electronic_mapping(my_e_mapping);
		unsigned int calo_circular_buffer_depth = 4;
		my_calo_algo.set_calo_circular_buffer_depth(calo_circular_buffer_depth);
		my_calo_algo.initialize();

	 	my_calo_algo.process(my_calo_ctw_data);
	      } // end of if has "calo" || "xcalo" step hits
	    
	  }
	ER.clear();

	psd_count++;
	std::clog << "DEBUG : psd count " << psd_count << std::endl;
	DT_LOG_NOTICE(logging, "Simulated data #" << psd_count);
      }
    
    std::clog << "The end." << std::endl;
  }
  catch (std::exception & error) {
    DT_LOG_FATAL(logging, error.what());
    error_code = EXIT_FAILURE;
  }

  catch (...) {
    DT_LOG_FATAL(logging, "Unexpected error!");
    error_code = EXIT_FAILURE;
  }

  FALAISE_FINI();
  return error_code;
}
