/*
 QHY camera tool

 Copyright 2016  Oleg Kutkov <kutkov.o@yandex.ru>

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.  
*/

#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <getopt.h>
#include "fits_handler.hpp"
#include "qhycam.hpp"
#include "logger.h"

void show_usage()
{
	std::cout << "Usage:" << std::endl;
	

	std::cout << std::endl;
}

void parse_resolution_str(const char* resolution_str, int& res_w, int& res_h)
{
	std::string string_to_parse = resolution_str;

	size_t coma_pos = string_to_parse.find(',');

	if (coma_pos == std::string::npos) {
		throw std::runtime_error("Invalid format of the resolution string!");
	}

	std::string res_w_str = string_to_parse.substr(0, coma_pos);
	std::string res_h_str = string_to_parse.substr(coma_pos + 1);

	res_w = atoi(res_w_str.c_str());
	res_h = atoi(res_h_str.c_str());


	if (res_w == 0 || res_h == 0) {
		throw std::runtime_error("Invalid resolution!");
	}
}

DEVICE_TYPE get_device_type_from_model_name(const std::string& model_camera)
{
	std::string model = model_camera;

	std::transform(model.begin(), model.end(), model.begin(), ::tolower);

	if (model == "qhy5") {
		return DEVICE_QHY5;
	}

	if (model == "qhy6") {
		return DEVICE_QHY6;
	}

	if (model == "qhy5ii") {
		return DEVICE_QHY5II;
	}

	return DEVICE_UNKNOWN_DEVICE;
}

void LoadFitsHeaderData(std::string &file, std::vector<fits_header_data_t> &dst_data)
{
	std::ifstream datfile(file);

	if (!datfile.is_open()) {
		std::string error = "Unable to open header data file " + file;
		throw std::runtime_error(error.c_str());
	}

	std::string line;
	fits_header_data_t fdata;

	while (std::getline(datfile, line)) {
		std::string key = line.substr(0, line.find(' '));
		std::string val = line.substr(line.find(' ') + 1, line.size());

		strncpy(fdata.key, key.c_str(), key.size());
		fdata.key[key.size()] = '\0';

		strncpy(fdata.val, val.c_str(), val.size());
		fdata.val[val.size()] = '\0';

		dst_data.push_back(fdata);
	}
}

int main(int argc, char **argv)
{
	logger_reset_state();
	logger_set_out_stdout();

	QhyCam::InitializeSystem();

	static struct option long_options[] = {
		{ "help", no_argument, NULL, 'h' },
		{ "debug", no_argument, NULL, 'd' },
		{ "list", no_argument, NULL, 'l' },
		{ "model_camera", required_argument, NULL, 'm' },
		{ "exposure_time", required_argument, NULL, 'e' },
		{ "gain", required_argument, NULL, 'g' },
		{ "resolution", required_argument, NULL, 'r' },
		{ "output_file", required_argument, NULL, 'o' },
		{ "color_required", no_argument, NULL, 'c' },
		{ "darkframe_subtraction", required_argument, NULL, 'k' },
		{ "extra_header_data", required_argument, NULL, 'x' }
	};

	int option_index = 0;
	int opt = getopt_long(argc, argv, "hdlm:e:g:r:o:ck:x:", long_options, &option_index);

	std::string model_camera, output_filename, darkframe_file, header_data_file;
	int expo_time = 100;
	unsigned short gain = 0;
	int res_w = 1280, res_h = 960;
	bool color_mode = false;
	bool substract_dark = false;

	std::vector<fits_header_data_t> fits_head_data;

    while (opt != -1) {
		switch (opt) {
			case 'h':
				show_usage();
				QhyCam::ReleaseSystem();
				return 0;

			case 'd':
				logger_set_log_level(LOG_MAX_LEVEL_ERROR_WARNING_STATUS_DEBUG);
				break;

			case 'l':
				QhyCam::ScanQhy();
				QhyCam::ReleaseSystem();
				return 0;

			case 'm':
				model_camera = optarg;
				break;

			case 'e':
				expo_time = atoi(optarg);

				if (expo_time < 1 or expo_time > 30000) {
					log_warning("Invalid exposure time, correcting to default 10 ms");
					expo_time = 10;
				}

				break;

			case 'g':
				gain = atoi(optarg);
				break;

			case 'r':
				parse_resolution_str(optarg, res_w, res_h);
				break;

			case 'o':
				output_filename = optarg;
				break;

			case 'c':
				color_mode = true;
				break;

			case 'k':
				darkframe_file = optarg;
				break;

			case 'x':
				header_data_file = optarg;
				LoadFitsHeaderData(header_data_file, fits_head_data);
				break;
	
			default:
				show_usage();
				abort();
		}

		opt = getopt_long(argc, argv, "dlm:e:g:r:o:ck:s:b:x:", long_options, &option_index);
	}

	if (!model_camera.size() || !output_filename.size()) {
		log_error("Please set mandatory params\n");
		show_usage();
		QhyCam::ReleaseSystem();
		return -1;
	}

	DEVICE_TYPE dev_type = get_device_type_from_model_name(model_camera);

	if (dev_type == DEVICE_UNKNOWN_DEVICE) {
		log_error("Unknown camera model %s", model_camera.c_str());
		QhyCam::ReleaseSystem();
		return -1;
	}

	std::unique_ptr<FitsHandler> output_fits;
	std::unique_ptr<FitsHandler> dark_fits;

	try {
		output_fits = std::unique_ptr<FitsHandler>(new FitsHandler(output_filename));
	} catch (FitsException& ex) {
		log_error("Failed to create FitsHandler, %s", ex.what());
		QhyCam::ReleaseSystem();
		return -1;
	}

	if (darkframe_file.size()) {
		dark_fits = std::unique_ptr<FitsHandler>(new FitsHandler(darkframe_file, false));
		dark_fits->LoadImageData();
		substract_dark = true;
	}

	QhyCam qcam;

	int result = qcam.ConnectCamera(dev_type);

	if (result != QHYCCD_SUCCESS) {
		log_error("Failed to connect to the selected camera, error %i", result);
		QhyCam::ReleaseSystem();
		return -1;
	}

	if (color_mode) {
		qcam.SetColorMode();
	}

	qcam.SetCameraResolution(res_w, res_h);
	qcam.SetCameraExposureTime(expo_time);
	qcam.SetCameraGain(gain);

	log_status("Starting capture");

	qcam.StartCapture();

	qcam.GetFrame(*output_fits);

	log_status("Capture finished");

	qcam.DisconnectCamera();
	QhyCam::ReleaseSystem();

	log_status("Creating new FITS image with header");
	output_fits->CreateNewImage(8);

	output_fits->SetHeader(fits_head_data);

	if (substract_dark) {
		log_status("Substracting dark file %s", darkframe_file.c_str());
		output_fits->Substract(*dark_fits);
	}

	log_status("Saving image as %s", output_filename.c_str());

	output_fits->SaveImageData();

	return 0;
}
