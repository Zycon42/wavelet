/**
 * @file main.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "wlfimage.h"
#include "utils.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <map>
#include <stdexcept>
#include <fstream>
#include <sstream>

typedef std::map<std::string, std::string> OptionsMap;
typedef std::map<std::string, WlfImage::PixelFormat::Type> PixelFormatMap;

const PixelFormatMap pfMap = create_map<std::string, WlfImage::PixelFormat::Type>
	("rgb", WlfImage::PixelFormat::Type::RGB)("ycbcr444", WlfImage::PixelFormat::Type::YCbCr444);

template <typename T>
T extractFromString(const std::string& str) {
	std::istringstream iss(str);
	T result;
	iss >> result;
	return result;
}

void decompress(const std::string& in, const std::string& out) {
	auto img = WlfImage::read(in.c_str());
	cv::imwrite(out, img);
}

void compress(const std::string& in, const std::string& out, const OptionsMap& options) {
	auto img = cv::imread(in);
	if (!img.data)
		throw std::runtime_error("cv::imread failed on input file \"" + in + "\"");

	WlfImage::Params params;
	params.dwtLevels = extractFromString<decltype(params.dwtLevels)>(options.at("l"));
	params.compressRate = extractFromString<decltype(params.compressRate)>(options.at("c"));
	params.pf = pfMap.at(options.at("f"));
	WlfImage::save(out.c_str(), img, params);
}

void printUsage() {
	std::cout << "wlfconv [-f FORMAT -l DWTLEVELS -c RATE] INPUT OUTPUT\n"
		<< "wlfconv -d INPUT OUTPUT\n"
		<< "  -f FORMAT     pixel format one of [rgb, ycbcr444(default)]\n"
		<< "  -l DWTLEVELS  resolution of discrete wavelet transfom default(4)\n"
		<< "  -c RATE       number of bitplanes that will be discarted default(0)\n"
		<< "  -d            this option means decompression instead compression\n"
		<< "  INPUT         input file in standard raster format (that opencv can handle)\n"
		<< "  OUTPUT        output file in wlf format\n";
}

void addOption(const std::string& option, const std::string& arg, OptionsMap& options) {
	auto iter = options.find(option);
	if (iter == options.end())
		throw std::runtime_error("Unknown option \"" + option + "\"");

	iter->second = arg;
}

std::vector<std::string> parseCmdline(int argc, char* argv[], OptionsMap& options) {
	std::vector<std::string> leftovers;
	bool expectArg = false;
	std::string curOpt;

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];
		// option
		if (arg.size() > 1 && arg[0] == '-') {
			// there was option without argument
			if (expectArg) {
				addOption(curOpt, "true", options);
				expectArg = false;
			}

			curOpt = arg.substr(1);
			expectArg = true;
		// argument
		} else {
			// if there was option
			if (expectArg) {
				addOption(curOpt, arg, options);
				expectArg = false;
			} else
				leftovers.push_back(arg);
		}
	}

	return leftovers;
}

int main(int argc, char* argv[]) {
	std::string input, output;
	OptionsMap options = create_map<OptionsMap::key_type, OptionsMap::mapped_type>
		("d", "false")("f", "ycbcr444")("c", "0")("l", "4");
	try {
		auto lefovers = parseCmdline(argc, argv, options);
		if (lefovers.size() != 2)
			throw std::runtime_error("Missing leftover args");
		input = lefovers[0];
		output = lefovers[1];
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		printUsage();
		return 2;
	}

	try {
		if (options["d"] == "true") {
			decompress(input, output);
		} else {
			compress(input, output, options);
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}