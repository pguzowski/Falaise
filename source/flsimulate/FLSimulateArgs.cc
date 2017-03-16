// Ourselves
#include "FLSimulateArgs.h"

// Third party:
// - Bayeux:
#include <bayeux/datatools/utils.h>
#include <bayeux/datatools/urn.h>
#include <bayeux/datatools/kernel.h>
#include <bayeux/datatools/urn_query_service.h>
#include <bayeux/mygsl/random_utils.h>

// This Project:
#include "falaise/property_reader.h"
#include "FLSimulateCommandLine.h"
#include "FLSimulateErrors.h"

namespace FLSimulate {

  void do_postprocess(FLSimulateArgs & flSimParameters);

  // static
  FLSimulateArgs FLSimulateArgs::makeDefault()
  {
    FLSimulateArgs params;

    // Application specific parameters:
    params.logLevel = datatools::logger::PRIO_ERROR;
    params.userProfile = "normal";

    // Identification of the simulation setup:
    params.experimentID           = "";
    params.simulationSetupVersion = "";
    params.simulationSetupUrn     = "urn:snemo:demonstrator:simulation:2.1";

    // Simulation manager internal parameters:
    params.simulationManagerParams.set_defaults();
    params.simulationManagerParams.interactive = false;
    params.simulationManagerParams.logging = "error";
    params.simulationManagerParams.manager_config_filename = "";
    // Seeding is auto (from system) unless explicit file supplied
    params.simulationManagerParams.input_prng_seeds_file = "";
    params.simulationManagerParams.vg_seed   = mygsl::random_utils::SEED_AUTO; // PRNG for the vertex generator
    params.simulationManagerParams.eg_seed   = mygsl::random_utils::SEED_AUTO; // PRNG for the primary event generator
    params.simulationManagerParams.shpf_seed = mygsl::random_utils::SEED_AUTO; // PRNG for the back end MC hit processors
    params.simulationManagerParams.mgr_seed  = mygsl::random_utils::SEED_AUTO; // PRNG for the Geant4 engine itself
    params.simulationManagerParams.output_profiles_activation_rule = "";

    // Variants support:
    params.variantSubsystemParams.config_filename = "";

    // Service support:
    params.servicesSubsystemConfigUrn = "";
    params.servicesSubsystemConfig = "";

    // Simulation control:
    params.numberOfEvents     = 1;
    params.outputMetadataFile = "";
    params.embeddedMetadata   = false;
    params.outputFile         = "";

    return params;
  }

  void do_configure(int argc, char *argv[], FLSimulateArgs& flSimParameters) {
    // - Default Config
    flSimParameters = FLSimulateArgs::makeDefault();

    // - CL Dialog Config
    FLSimulateCommandLine args;
    try {
      do_cldialog(argc, argv, args);
    } catch (FLDialogHelpRequested& e) {
      throw FLConfigHelpHandled();
    } catch (FLDialogOptionsError& e) {
      throw FLConfigUserError {"bad command line input"};
    }

    // Feed input from command line to params
    flSimParameters.logLevel           = args.logLevel;
    flSimParameters.outputMetadataFile = args.outputMetadataFile;
    flSimParameters.embeddedMetadata   = args.embeddedMetadata;
    flSimParameters.outputFile         = args.outputFile;
    flSimParameters.userProfile        = args.userProfile;

    if (flSimParameters.outputMetadataFile.empty()) {
      DT_THROW(FLConfigUserError, "Missing output metadata file path!");
    }

    // If a script was supplied, use that to override params
    if(!args.configScript.empty()) {
      datatools::multi_properties flSimConfig("section", "description");
      std::string configScript = args.configScript;
      datatools::fetch_path_with_env(configScript);
      flSimConfig.read(configScript);
      if (datatools::logger::is_debug(flSimParameters.logLevel)) {
        flSimConfig.tree_dump(std::cerr, "Simulation Configuration:", "[debug] ");
      }

      // Now extract and bind values as needed
      // Caution: some parameters are only available for specific user profile

      // Simulation subsystem:
      if(flSimConfig.has_section("SimulationSubsystem")) {
        datatools::properties simSubsystem = flSimConfig.get_section("SimulationSubsystem");
        // Bind properties in this section to the relevant ones in params:

        // Simulation setup URN:
        flSimParameters.simulationSetupUrn
          = falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                                "simulationUrn",
                                                                flSimParameters.simulationSetupUrn);

        // Experiment identifier:
        if (flSimParameters.userProfile == "production" && simSubsystem.has_key("experimentID")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "experimentID" << "' simulation configuration parameter!");
        }
        flSimParameters.experimentID
          = falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                                "experimentID",
                                                                flSimParameters.experimentID);

        // Simulation setup version:
        if (flSimParameters.userProfile == "production" && simSubsystem.has_key("simulationVersion")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "simulationVersion" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationSetupVersion
          = falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                                "simulationVersion",
                                                                flSimParameters.simulationSetupVersion);

        // Simulation manager main configuration file:
        if (flSimParameters.userProfile == "production" && simSubsystem.has_key("simulationConfig")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "simulationConfig" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.manager_config_filename
          = falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                                "simulationConfig",
                                                                flSimParameters.simulationManagerParams.manager_config_filename);

        // Number of simulated events:
        flSimParameters.numberOfEvents = falaise::properties::getValueOrDefault<int>(simSubsystem,
                                                                                     "numberOfEvents",
                                                                                     flSimParameters.numberOfEvents);


        // Printing rate for events:
        flSimParameters.simulationManagerParams.number_of_events_modulo =
          falaise::properties::getValueOrDefault<int>(simSubsystem,
                                                      "moduloEvents",
                                                      flSimParameters.simulationManagerParams.number_of_events_modulo);

        // File for loading internal PRNG's seeds:
        if (flSimParameters.userProfile != "expert" && !simSubsystem.has_key("rngSeedFile")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "must use the '" << "rngSeedFile" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.input_prng_seeds_file =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "rngSeedFile",
                                                              flSimParameters.simulationManagerParams.input_prng_seeds_file);

        // File for loading internal PRNG's states:
        if (flSimParameters.userProfile != "expert" && simSubsystem.has_key("inputRngStateFile")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "inputRngStateFile" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.input_prng_states_file =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "inputRngStateFile",
                                                              flSimParameters.simulationManagerParams.input_prng_states_file);

        // File for saving internal PRNG's states:
        flSimParameters.simulationManagerParams.output_prng_states_file =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "outputRngStateFile",
                                                              flSimParameters.simulationManagerParams.output_prng_states_file);

        // Saving rate for internal PRNG's states:
        flSimParameters.simulationManagerParams.prng_states_save_modulo =
          falaise::properties::getValueOrDefault<int>(simSubsystem,
                                                      "rngStateModuloEvents",
                                                      flSimParameters.simulationManagerParams.prng_states_save_modulo);

        // Simulation output profile:
        if (flSimParameters.userProfile == "production" && simSubsystem.has_key("outputProfile")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "outputProfile" << "' simulation configuration parameter!");
        }
        flSimParameters.simulationManagerParams.output_profiles_activation_rule =
          falaise::properties::getValueOrDefault<std::string>(simSubsystem,
                                                              "outputProfile",
                                                              flSimParameters.simulationManagerParams.output_profiles_activation_rule);
      }

      // Variants subsystem:
      if (flSimConfig.has_section("VariantSubsystem")) {
        datatools::properties variantSubsystem = flSimConfig.get_section("VariantSubsystem");
        // Bind properties to relevant ones on params

        // Variant configuration URN:
        flSimParameters.variantConfigUrn
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "configUrn",
                                                                flSimParameters.variantConfigUrn);

        // Variant configuration:
        if (flSimParameters.userProfile == "production" && variantSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "config" << "' variants configuration parameter!");
        }
        flSimParameters.variantSubsystemParams.config_filename
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "config",
                                                                flSimParameters.variantSubsystemParams.config_filename);

        // Variant profile URN:
        flSimParameters.variantProfileUrn
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "profileUrn",
                                                                flSimParameters.variantProfileUrn);


        // Variant profile:
        flSimParameters.variantSubsystemParams.profile_load
          = falaise::properties::getValueOrDefault<std::string>(variantSubsystem,
                                                                "profile",
                                                                flSimParameters.variantSubsystemParams.profile_load);

        // Variant settings:
        if (flSimParameters.userProfile != "expert" && variantSubsystem.has_key("settings")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "settings" << "' variants configuration parameter!");
        }
        flSimParameters.variantSubsystemParams.settings
          = falaise::properties::getValueOrDefault<std::vector<std::string> >(variantSubsystem,
                                                                              "settings",
                                                                              flSimParameters.variantSubsystemParams.settings);
      }

      // Services subsystem:
      if (flSimConfig.has_section("ServicesSubsystem")) {
        datatools::properties servicesSubsystem = flSimConfig.get_section("ServicesSubsystem");

        // Services manager configuration URN:
        flSimParameters.servicesSubsystemConfigUrn =
          falaise::properties::getValueOrDefault<std::string>(servicesSubsystem,
                                                              "configUrn",
                                                              flSimParameters.servicesSubsystemConfigUrn);

        // Services manager main configuration file:
        if (flSimParameters.userProfile == "production" && servicesSubsystem.has_key("config")) {
          DT_THROW(FLConfigUserError,
                   "User profile '" << flSimParameters.userProfile << "' "
                   << "does not allow to use the '" << "config" << "' services configuration parameter!");
        }
        flSimParameters.servicesSubsystemConfig =
          falaise::properties::getValueOrDefault<std::string>(servicesSubsystem,
                                                              "config",
                                                              flSimParameters.servicesSubsystemConfig);
      }

    }

    do_postprocess(flSimParameters);
    return;
  }


  void do_postprocess(FLSimulateArgs & flSimParameters)
  {
    datatools::kernel & dtk = datatools::kernel::instance();
    const datatools::urn_query_service & dtkUrnQuery = dtk.get_urn_query();

    if (!flSimParameters.simulationManagerParams.manager_config_filename.empty()) {
      // Hardcoded path to the main simulation configuration file

      // Variants configuration must also be hardcoded:
      DT_THROW_IF(flSimParameters.variantSubsystemParams.config_filename.empty(),
                  std::logic_error,
                  "Missing variants configuration file!");

      // // Services configuration must also be hardcoded:
      // DT_THROW_IF(flSimParameters.servicesSubsystemConfig.empty(),
      //             std::logic_error,
      //             "Missing services configuration file!");

    } else {

      // Build the URN from experiment identifier and simulation setup version, if any:
      if (! flSimParameters.experimentID.empty()) {
        DT_THROW_IF(!flSimParameters.simulationSetupUrn.empty(),
                    std::logic_error,
                    "Simulation setup URN='" << flSimParameters.simulationSetupUrn << "' conflicts with experiment='"
                    << flSimParameters.experimentID << "' parameter!");
        DT_THROW_IF(flSimParameters.simulationSetupVersion.empty(),
                    std::logic_error,
                    "Missing simulation setup version!");
        // Build URN from experimentID/version labels:
        datatools::urn u;
        if (flSimParameters.experimentID == "demonstrator") {
          u.append_segment("snemo");
          u.append_segment("demonstrator");
        }
        u.append_segment("simulation");
        u.append_segment(flSimParameters.simulationSetupVersion);
        if (u.is_valid()) {
          flSimParameters.simulationSetupUrn = u.to_string();
        }
      }

      // Now the URN is set, we try to extract automatically the path of the
      // components associated to it (variants, services...)
      if (!flSimParameters.simulationSetupUrn.empty()) {
        // Check URN registration from the system URN query service:
        {
          std::string conf_category = "configuration";
          DT_THROW_IF(!dtkUrnQuery.check_urn_info(flSimParameters.simulationSetupUrn, conf_category),
                      std::logic_error,
                      "Cannot query URN='" << flSimParameters.simulationSetupUrn << "'!");
        }
        const datatools::urn_info & conf_simu_ui = dtkUrnQuery.get_urn_info(flSimParameters.simulationSetupUrn);

        // Simulation:
        {
          // Resolve simulation config file path:
          std::string conf_simu_category = "configuration";
          std::string conf_simu_mime;
          std::string conf_simu_path;
          DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.simulationSetupUrn,
                                                       conf_simu_category,
                                                       conf_simu_mime,
                                                       conf_simu_path),
                      std::logic_error,
                      "Cannot resolve URN='" << flSimParameters.simulationSetupUrn << "'!");
          flSimParameters.simulationManagerParams.manager_config_filename = conf_simu_path;
        }

        // Variants:
        if (flSimParameters.variantConfigUrn.empty()) {
          // Automatically determine the variants configuration components:
          if (conf_simu_ui.get_components_by_topic("variants").size() == 1) {
            flSimParameters.variantConfigUrn = conf_simu_ui.get_component("variants");
          }
        }
        {
          // Resolve variants file:
          std::string conf_variants_category = "configuration";
          std::string conf_variants_mime;
          std::string conf_variants_path;
          DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.variantConfigUrn,
                                                       conf_variants_category,
                                                       conf_variants_mime,
                                                       conf_variants_path),
                      std::logic_error,
                      "Cannot resolve URN='" << flSimParameters.variantConfigUrn << "'!");
          flSimParameters.variantSubsystemParams.config_filename = conf_variants_path;
        }

        // Services:
        if (!flSimParameters.servicesSubsystemConfig.empty()) {
          // Force the services config path:
          DT_THROW_IF(!flSimParameters.servicesSubsystemConfigUrn.empty(),
                      std::logic_error,
                      "Service configuration URN='" << flSimParameters.servicesSubsystemConfigUrn << "' conflicts with services configuration path='"
                      << flSimParameters.servicesSubsystemConfig << "'!");
        } else {
          // Try to set the services setup from a blessed services configuration URN:
          if (flSimParameters.servicesSubsystemConfigUrn.empty()) {
            if (conf_simu_ui.get_components_by_topic("services").size() == 1) {
              // If sthe simulation setup URN implies a "services" component, fetch it!
              flSimParameters.servicesSubsystemConfigUrn = conf_simu_ui.get_component("services");
            }
          }
          if (!flSimParameters.servicesSubsystemConfigUrn.empty()) {
            // Resolve services file:
            std::string conf_services_category = "configuration";
            std::string conf_services_mime;
            std::string conf_services_path;
            DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.servicesSubsystemConfigUrn,
                                                         conf_services_category,
                                                         conf_services_mime,
                                                         conf_services_path),
                        std::logic_error,
                        "Cannot resolve URN='" << flSimParameters.servicesSubsystemConfigUrn << "'!");
            flSimParameters.servicesSubsystemConfig = conf_services_path;
          }
        }
      }
    }

    // Variants profile:
    if (!flSimParameters.variantSubsystemParams.profile_load.empty()) {
      // Force the variant profile path:
      DT_THROW_IF(!flSimParameters.variantProfileUrn.empty(),
                  std::logic_error,
                  "Variants profile URN='" << flSimParameters.variantProfileUrn << "' conflicts with variants profile path='"
                  << flSimParameters.variantSubsystemParams.profile_load << "'!");
    } else if (!flSimParameters.variantProfileUrn.empty()) {
      // Determine the variant profile path from a blessed variant profile URN:
      std::string conf_variantsProfile_category = "configuration";
      std::string conf_variantsProfile_mime;
      std::string conf_variantsProfile_path;
      DT_THROW_IF(!dtkUrnQuery.resolve_urn_to_path(flSimParameters.variantProfileUrn,
                                                   conf_variantsProfile_category,
                                                   conf_variantsProfile_mime,
                                                   conf_variantsProfile_path),
                  std::logic_error,
                  "Cannot resolve variants profile URN='" << flSimParameters.variantProfileUrn << "'!");
      flSimParameters.variantSubsystemParams.profile_load = conf_variantsProfile_path;
    }

    // Additional variants settings may be allowed but should be compatible with selected variants config and profile

    if (flSimParameters.servicesSubsystemConfig.empty()) {
      DT_LOG_WARNING(flSimParameters.logLevel, "No services configuration is provided.");
    }

    return;
  }

} // namespace FLSimulate