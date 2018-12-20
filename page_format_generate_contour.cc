#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/exception.h>
#include <boost/thread/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
//#include "image.hpp"
#include "page_file.hpp"

namespace po = boost::program_options;
using namespace std;
using namespace boost::filesystem;
using namespace log4cxx;
using namespace log4cxx::helpers;

LoggerPtr logger(Logger::getLogger("PRHLT"));

void setVerbosity(int verb){

	switch(verb){
		case 0:
			logger->setLevel(log4cxx::Level::getError());
			break;	
		case 1:
			logger->setLevel(log4cxx::Level::getInfo());
			break;
		case 2:
			logger->setLevel(log4cxx::Level::getDebug());
			break;
		default:
			logger->setLevel(log4cxx::Level::getError());
	}
}

bool areValidOptions(po::variables_map vm){
	
	if (  vm["operation_mode"].as<string>() != "CLIP" and vm["operation_mode"].as<string>() != "FULL" and  vm["operation_mode"].as<string>() != "LINE"){
		LOG4CXX_ERROR(logger,"The operation method specified: \"" << vm["operation_mode"].as<string>() << "\" is not valid. Operation method must be either DISPLAY or FILE");
		return false;
	}
	if (  vm["output_file"].as<string>() == "" ){
		LOG4CXX_ERROR(logger,"The output file parameter can not be empty!");
		return false;
	}
	if (  !exists(vm["page_file"].as<string>())){
		LOG4CXX_ERROR(logger,"The page file specified: \"" << vm["page_file"].as<string>() << "\" does not exist.");
		return false;
	}
	
	if ( vm["verbosity"].as<int>() < 0 or vm["verbosity"].as<int>() > 2) {
		LOG4CXX_ERROR(logger,"Verbosity value must be betwee 0 and 2 ");
		return false;
	}
	
	return true;
}

void displayInputedValues(po::variables_map vm){
	LOG4CXX_INFO(logger,"<<INPUTED PARAMETERS>>");
	LOG4CXX_INFO(logger,"Page file              : " << vm["page_file"].as<string>());
	LOG4CXX_INFO(logger,"Output file            : " << vm["output_file"].as<string>());
	LOG4CXX_INFO(logger,"Line id                : " << vm["line_id"].as<string>());
	LOG4CXX_INFO(logger,"Operation mode         : " << vm["operation_mode"].as<string>());
	LOG4CXX_INFO(logger,"Verbosity              : " << vm["verbosity"].as<int>());	
}

int main(int argc, char **argv){   
	
	string input_image,page_file,output_file,line_file,operation_mode,line_id;
	int verbosity, asc_offset, desc_offset;

	BasicConfigurator::configure();
	po::options_description desc( "Allowed options" );
	desc.add_options()
		( "help,h", "Generates this help message" )
		//( "input_image,i", po::value<string>(&input_image)->default_value("image.jpg"),"Input image on wich to calculate the line extraction(by default ./image.jpg)" )
		( "page_file,p", po::value<string>(&page_file)->default_value("page.xml"),"Regions file path (by default ./page.xml)" )
		( "output_file,o", po::value<string>(&output_file)->default_value("contours.xml"),"Regions file path (by default contours.xml)" )
		( "line_id,l", po::value<string>(&line_id)->default_value("l0"),"Regions file path (by default l0)" )
		( "operation_mode,m", po::value<string>(&operation_mode)->default_value("FULL"), "Operation mode of the command line tool, full generation of all contours (FULL) or single baseline generation (LINE), or CLIP to just clip all baselines to fit the containing text region (default value is FULL)")
		( "asc_offset,a", po::value<int>(&asc_offset)->default_value(70), "\% Offset to baseline points to include ascendants (Default: 70)")
		( "desc_offset,d", po::value<int>(&desc_offset)->default_value(30), "\% Offset to baseline points to include descendants (Default: 30)")
		( "verbosity,v", po::value<int>(&verbosity)->default_value(0), "Verbosity os messages during execution [0-2]");
	po::variables_map vm;
	po::store( po::parse_command_line( argc, argv, desc ), vm ); // to read command line options
	po::notify( vm );

	setVerbosity(vm["verbosity"].as<int>());

	if (vm.count("help"))
		std::cout << desc << std::endl;
	else{
		if(areValidOptions(vm)){
		    displayInputedValues(vm);
			prhlt::Page_File page(vm["page_file"].as<string>());
        
        if(vm["operation_mode"].as<string>()=="FULL"){
            	LOG4CXX_INFO(logger,"<<FULL MODE>>");
            	page.generate_countour_from_baseline(vm["desc_offset"].as<int>(),-vm["asc_offset"].as<int>());
            	LOG4CXX_INFO(logger,"<<Generating contours XML>>");
				page.save_xml(vm["output_file"].as<string>());
        }else{
        	if(vm["operation_mode"].as<string>()=="LINE"){
            	LOG4CXX_INFO(logger,"<<LINE MODE>>");
            	page.generate_countour_from_baseline(vm["line_id"].as<string>(),vm["desc_offset"].as<int>(),-vm["asc_offset"].as<int>());
            	LOG4CXX_INFO(logger,"<<Generating contours XML>>");
				page.save_xml(vm["output_file"].as<string>());
        	}
			else
        	if(vm["operation_mode"].as<string>()=="CLIP"){
            	page.clip_baselines();
				page.save_xml(vm["output_file"].as<string>());
        	}
			else
            	LOG4CXX_INFO(logger,"<<MODE CURRENTLY NOT SUPPORTED>>");
        }
        
		}
		else{
			LOG4CXX_ERROR(logger,"Exiting due to incorrect input parameters");
			exit(EXIT_FAILURE);
		};
	}

	LOG4CXX_INFO(logger,"<<APPLICATION EXITING CORRECTLY>>");
	exit(EXIT_SUCCESS);
}
