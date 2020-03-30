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

	co::sanity::FolderManager::folder_sanity_check("color");
	co::sanity::FolderManager::folder_sanity_check("depth");

	/** @brief Recorder for the RGBD data
*/
	co::io::RGBDWriter rgbd_recorder;

	// dummy intrinsic
	std::vector<std::string> v_intrinsic;
	v_intrinsic.push_back("640 480 110 110 320 240"); 
	v_intrinsic.push_back("0.1 0.2 0.3");
	rgbd_recorder.save_intrinsic(".", v_intrinsic);

	// These record mode are left  only for legacy.
	// For faster recording, please use !do_use_bagfile
	// Record as hi-res
	// An example to read the file is in eLearningElaboration/sample/ExtractPointCloud

	std::string path_where_to_save = ".";
	int num_frame_save = 0;
	cv::Mat color(480, 640, CV_8UC3);
	cv::Mat depth(480, 640, CV_32FC1);
	std::vector<float> vertices(300);
	std::vector<float> texture_coordinates(200);
	rgbd_recorder.add_rgbxyzuv_as_binary(
		path_where_to_save, num_frame_save,
		color.data,
		color.rows * color.cols * 3 * sizeof(uchar),
		&vertices.data()[0],
		vertices.size() * sizeof(float),
		&texture_coordinates.data()[0],
		texture_coordinates.size() * sizeof(float));
	// Record as lo-res
	num_frame_save = 1;
	rgbd_recorder.save_rgbd_as_binary(
		path_where_to_save, num_frame_save,
		color.data,
		color.rows * color.cols * 3 * sizeof(uchar),
		depth.data,
		depth.rows * depth.cols * 1 * sizeof(unsigned short));
	// Record as encoded
	num_frame_save = 2;
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


void test_reader() {
	std::cout << "read_rgbd_recorded: " << __DATE__ << std::endl;

	std::string path_where_to_read = ".";
	int current_frame = 0;
	int bin = 10; // frequency of the xyz data estimated (min 1)
	std::string file_where_to_save = "sample_rgbd.txt";
	std::string file_where_to_save_xml = "sample_rgbd.xml";

	cv::Mat depth;
	co::io::RGBDReader<uchar>::extract_depth(".\\depth\\000002.depth_raw", cv::Size(640, 480), depth);
	
	// Example
	//std::string path_where_to_read = "F:\\20190711\\position2\\20190711_111504";
	//int current_frame = 100;
	//int bin = 5;
	//std::string path_where_to_save = "PCL3D0.txt";
	//std::string path_where_to_save_xml = "PCL3D0.xml";


	// The current frame is smaller than the stop frame.
	std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
	if (co::io::RGBDReader<uchar>::create_3Dpointcloud_imgsource(
		path_where_to_read,
		current_frame, bin, p3dcolor)) {
		std::cout << "[+] " << path_where_to_read << std::endl;
		co::io::PCLViewerIONaive<cv::Point3f>::save_naive_pointcloud(
			file_where_to_save, cv::Point3f(0, 0, 0), cv::Point3f(0, 0, 0),
			p3dcolor);
	}
	else {
		std::cout << "[-] " << path_where_to_read << std::endl;
	}

	// It saves the current image and 3D representation in a xml format
	cv::Mat rgb, xyz;
	if (co::io::RGBDReader<uchar>::create_3Dpointcloud_imgsource(
		path_where_to_read,
		current_frame, bin, rgb, xyz)) {
		std::cout << "[+] " << path_where_to_read << std::endl;
		save_points_info(file_where_to_save_xml, rgb, xyz);
	}
	else {
		std::cout << "[-] " << path_where_to_read << std::endl;
	}
}

void test_recorder() {
	std::cout << "RGBDRecorder is deprecated" << std::endl;
}

void main() {
	std::cout << "io_rgbd" << std::endl;
	test_writer();
	test_reader();
}