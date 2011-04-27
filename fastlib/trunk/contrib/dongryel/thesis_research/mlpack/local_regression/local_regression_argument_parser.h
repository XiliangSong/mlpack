/** @file local_regression_argument_parser.h
 *
 *  The argument parsing for local regression is handled here.
 *
 *  @author Dongryeol Lee (dongryel@cc.gatech.edu)
 */

#ifndef MLPACK_LOCAL_REGRESSION_LOCAL_REGRESSION_ARGUMENT_PARSER_H
#define MLPACK_LOCAL_REGRESSION_LOCAL_REGRESSION_ARGUMENT_PARSER_H

#include <boost/program_options.hpp>
#include "mlpack/local_regression/local_regression_arguments.h"

namespace mlpack {
namespace local_regression {

/** @brief The class for parsing the necessary arguments for the local
 *         regression.
 */
class LocalRegressionArgumentParser {
  public:
    static bool ConstructBoostVariableMap(
      const std::vector<std::string> &args,
      boost::program_options::variables_map *vm) {

      boost::program_options::options_description desc("Available options");
      desc.add_options()(
        "help", "Print this information."
      )(
        "bandwidth",
        boost::program_options::value<double>(),
        "OPTIONAL kernel bandwidth, if you set --bandwidth_selection flag, "
        "then the --bandwidth will be ignored."
      )(
        "kernel",
        boost::program_options::value<std::string>()->default_value("gaussian"),
        "Kernel function used by KDE.  One of:\n"
        "  epan, gaussian"
      )(
        "leaf_size",
        boost::program_options::value<int>()->default_value(20),
        "Maximum number of points at a leaf of the tree."
      )(
        "predictions_out",
        boost::program_options::value<std::string>()->default_value(
          "densities_out.csv"),
        "OPTIONAL file to store the predicted discrete choices."
      )(
        "prescale",
        boost::program_options::value<std::string>()->default_value("none"),
        "OPTIONAL scaling option. One of:\n"
        "  none, hypercube, standardize"
      )(
        "queries_in",
        boost::program_options::value<std::string>(),
        "OPTIONAL file containing query positions.  If omitted, local "
        "regression computes the leave-one-out density at each reference point."
      )(
        "references_in",
        boost::program_options::value<std::string>(),
        "The file containing the reference points."
      )(
        "relative_error",
        boost::program_options::value<double>()->default_value(0.1),
        "Relative error for the approximation of KDE."
      );

      boost::program_options::command_line_parser clp(args);
      clp.style(boost::program_options::command_line_style::default_style
                ^ boost::program_options::command_line_style::allow_guessing);
      try {
        boost::program_options::store(clp.options(desc).run(), *vm);
      }
      catch(const boost::program_options::invalid_option_value &e) {
        std::cerr << "Invalid Argument: " << e.what() << "\n";
        exit(0);
      }
      catch(const boost::program_options::invalid_command_line_syntax &e) {
        std::cerr << "Invalid command line syntax: " << e.what() << "\n";
        exit(0);
      }
      catch(const boost::program_options::unknown_option &e) {
        std::cerr << "Unknown option: " << e.what() << "\n";
        exit(0);
      }

      boost::program_options::notify(*vm);
      if(vm->count("help")) {
        std::cout << desc << "\n";
        return true;
      }

      // Validate the arguments. Only immediate quitting is allowed
      // here, the parsing is done later.
      if(vm->count("bandwidth") > 0 && (*vm)["bandwidth"].as<double>() <= 0) {
        std::cerr << "The --bandwidth requires a positive real number.\n";
        exit(0);
      }
      if(vm->count("bandwidth") == 0) {
        std::cerr << "Missing required --bandwidth.\n";
        exit(0);
      }
      if((*vm)["kernel"].as<std::string>() != "gaussian" &&
          (*vm)["kernel"].as<std::string>() != "epan") {
        std::cerr << "We support only epan or gaussian for the kernel.\n";
        exit(0);
      }
      if((*vm)["leaf_size"].as<int>() <= 0) {
        std::cerr << "The --leaf_size requires a positive integer.\n";
        exit(0);
      }
      if((*vm)["prescale"].as<std::string>() != "none" &&
          (*vm)["prescale"].as<std::string>() != "hypercube" &&
          (*vm)["prescale"].as<std::string>() != "standardize") {
        std::cerr << "The --prescale supports only one of: none, hypercube, " <<
                  "standardize.\n";
        exit(0);
      }

      if(vm->count("references_in") == 0) {
        std::cerr << "Missing required --references_in.\n";
        exit(0);
      }
      if((*vm)["relative_error"].as<double>() < 0.0) {
        std::cerr << "The --relative_error requires a positive real number.\n";
        exit(0);
      }

      return false;
    }

    template<typename TableType>
    static void ParseArguments(
      const std::vector<std::string> &args,
      mlpack::local_regression::LocalRegressionArguments <
      TableType > *arguments_out) {

      // Construct the Boost variable map.
      boost::program_options::variables_map vm;
      if(ConstructBoostVariableMap(args, &vm)) {
        exit(0);
      }

      ParseArguments(vm, arguments_out);
    }

    template<typename TableType>
    static void ParseArguments(
      boost::program_options::variables_map &vm,
      mlpack::local_regression::LocalRegressionArguments <
      TableType > *arguments_out) {

      // Parse bandwidth.
      arguments_out->bandwidth_ =  vm["bandwidth"].as<double>();

      // Parse the kernel.
      arguments_out->kernel_ = vm["kernel"].as<std::string>();

      // Parse the leaf size.
      arguments_out->leaf_size_ = vm["leaf_size"].as<int>();

      // Parse the predictions out file.
      arguments_out->predictions_out_ = vm["predictions_out"].as<std::string>();

      // Parse the prescale option.
      arguments_out->prescale_ = vm["prescale"].as<std::string>();

      // Read in the reference set.
      arguments_out->reference_table_ = new TableType();
      arguments_out->reference_table_->Init(
        vm["references_in"].as<std::string>());

      // Read in the optional query set.
      if(vm.count("queries_in") > 0) {
        arguments_out->queries_in_ = new TableType();
        arguments_out->queries_in_->Init(vm["queries_in"].as<std::string>());
      }
      else {
        arguments_out->queries_in_ = arguments_out->references_in_;
      }

      // Read in the relative error.
      arguments_out->relative_error_ = vm["relative_error"].as<double>();
    }

    template<typename TableType>
    static void ParseArguments(
      int argc,
      char *argv[],
      mlpack::local_regression::LocalRegressionArguments <
      TableType > *arguments_out) {

      // Convert C input to C++; skip executable name for Boost.
      std::vector<std::string> args(argv + 1, argv + argc);

      ParseArguments(args, arguments_out);
    }
};
}
}

#endif