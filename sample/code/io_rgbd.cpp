/**
* @file io_rgbd.cpp
* @brief Example of the referred class.
*
* @section LICENSE
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR/AUTHORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* @author uknown
* @bug No known bugs.
* @version 0.1.0.0
*
*/


#include <algorithm>            // std::min, std::max
#include <string>
#include <ctime>
#include <regex>
#include <iostream>
#include <sstream>      // std::stringstream

#include <opencv2/opencv.hpp>

#include "../../commonobjects/sanity/sanity.hpp"
#include "../../commonobjects/io/RGBDReader.hpp"
#include "../../commonobjects/io/RGBDWriter.hpp"
#include "../../commonobjects/io/RGBDRecorder.hpp"

void test_writer() {

	co::sanity::FolderManager::folder_sanity_check("dataHiRes");
	co::sanity::FolderManager::folder_sanity_check("dataLoRes\\color");
	co::sanity::FolderManager::folder_sanity_check("dataLoRes\\depth");
	co::sanity::FolderManager::folder_sanity_check("dataBinary\\color");
	co::sanity::FolderManager::folder_sanity_check("dataBinary\\depth");

	/** @brief Recorder for the RGBD data
*/
	co::io::RGBDWriter rgbd_recorder;

	// dummy intrinsic
	std::vector<std::string> v_intrinsic;
	v_intrinsic.push_back("640 480"); 
	v_intrinsic.push_back("644.557129 644.557129 315.785370 244.418182");
	v_intrinsic.push_back("0.000000 0.000000 0.000000 0.000000 0.000000");

	// These record mode are left  only for legacy.
	// For faster recording, please use !do_use_bagfile
	// Record as hi-res
	// An example to read the file is in eLearningElaboration/sample/ExtractPointCloud

	std::string path_where_to_save = "dataHiRes/";
	int num_frame_save = 0;
	cv::Mat color(480, 640, CV_8UC3);
	cv::Mat depth(480, 640, CV_32FC1);
	std::vector<float> vertices(300);
	std::vector<float> texture_coordinates(200);
	rgbd_recorder.save_intrinsic(path_where_to_save, v_intrinsic);
	rgbd_recorder.add_rgbxyzuv_as_binary(
		path_where_to_save, num_frame_save,
		color.data,
		color.rows * color.cols * 3 * sizeof(uchar),
		&vertices.data()[0],
		vertices.size() * sizeof(float),
		&texture_coordinates.data()[0],
		texture_coordinates.size() * sizeof(float));
	// Record as lo-res
	path_where_to_save = "dataLoRes/";
	num_frame_save = 0;
	rgbd_recorder.save_intrinsic(path_where_to_save, v_intrinsic);
	rgbd_recorder.save_rgbd_as_binary(
		path_where_to_save, num_frame_save,
		color.data,
		color.rows * color.cols * 3 * sizeof(uchar),
		depth.data,
		depth.rows * depth.cols * 1 * sizeof(unsigned short));
	// Record as encoded
	path_where_to_save = "dataBinary/";
	num_frame_save = 0;
	rgbd_recorder.save_intrinsic(path_where_to_save, v_intrinsic);
	rgbd_recorder.save_rgbd_as_binary(
		path_where_to_save, num_frame_save,
		color.data,
		depth.data,
		color.cols,
		color.rows);
}

/** @brief It saves the image and the point cloud
*/
void save_points_info(
	const std::string &fname_reference,
	cv::Mat &m, cv::Mat &xyz) {

	// Declare what you need
	cv::FileStorage file(fname_reference, cv::FileStorage::WRITE);

	file << "img" << m;
	file << "xyz" << xyz;

	std::cout << "File saved: " << fname_reference << std::endl;
}

void test_extract_depth() {
	std::cout << "test_extract_depth: " << __DATE__ << std::endl;
	cv::Mat depth;
	co::io::RGBDReader<uchar>::extract_depth("dataBinary\\depth\\000000.depth_raw", 
		cv::Size(640, 480), depth);
}

void test_read_saved_data_hires() {
	std::cout << "test_read_saved_data_hires: " << __DATE__ << std::endl;

	std::string path_where_to_read = "dataHiRes/";
	int current_frame = 0;
	int bin = 10; // frequency of the xyz data estimated (min 1)
	cv::Mat depth;
	co::io::RGBDReader<uchar>::test_read_saved_data_hires(path_where_to_read, 
		cv::Size(640, 480), bin, 0, 1);
}


void test_create_3Dpointcloud_lores() {
	std::cout << "test_create_3Dpointcloud_lores: " << __DATE__ << std::endl;

	std::string path_where_to_read = "dataLoRes/";
	int current_frame = 0;
	int bin = 10; // frequency of the xyz data estimated (min 1)
	std::string file_where_to_save = "sample_rgbd.txt";
	std::string file_where_to_save_xml = "sample_rgbd.xml";

	cv::Mat rgb, map3D;
	co::io::RGBDReader<uchar>::create_3Dpointcloud_lores(path_where_to_read, 
		cv::Size(640, 480), 0, rgb, map3D);
}

void test_binary() {
	std::cout << "test_binary: " << __DATE__ << std::endl;
	std::cout << "check: eLearning::TransformSourceListBinary2EncodeImage" << 
		std::endl;
}


void test_recorder() {
	std::cout << "test_recorder" << std::endl;
}

void main() {
	std::cout << "io_rgbd" << std::endl;
	test_writer();
	test_extract_depth();
	test_read_saved_data_hires();
	test_read_saved_data_hires();
	test_binary();
	test_recorder();
}