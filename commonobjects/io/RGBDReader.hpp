/**
* @file RGBDReader.hpp
* @brief Header of the relative class
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
* @original  Alessandro Moro
* @bug No known bugs.
* @version 0.3.0.0
*
* @changelog
*  2019/10/08 Renamed: RGBDData -> RGBDReader
*/
#ifndef COMMONOBJECTS_IO_RGBDREADER_HPP__
#define COMMONOBJECTS_IO_RGBDREADER_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <opencv2/opencv.hpp>

#include "../string_common/StringOp.hpp"
#include "../enum/enum_helper.hpp"
#include "PCLViewerIONaive.hpp"

namespace co
{
namespace io
{
/** @brief It defines the way the data is saved
*/
enum SaveMode {
	sm_NotSet = 0,
	sm_HiRes = 1,
	sm_LoRes = 2,
	sm_Encoded = 3
};

/** @brief Class to perform some data operation over a binary comressed data
*/
template <typename _TyByte>
class RGBDReader
{
public:

	/** @brief Example to read saved data

		The data is expected to be collected in 3 files (image, xyz, uv)
	*/
	static void test_read_saved_data_hires(
		const std::string &path,
		const cv::Size &size,
		int fromID, int toID) {

		for (int i = fromID; i < toID; ++i) {
			// read image
			cv::Mat rgb;
			extract_rgb(path + "frame_" + std::to_string(i) + ".data", 
				size, rgb);

			// read xyz
			std::vector<cv::Point3f> xyz;
			std::vector<cv::Point2f> uv;
			std::map<std::pair<int, int>, cv::Point3f> uvxyz;
			extract_xyzuv(
				path + "xyz_" + std::to_string(i) + ".data",
				path + "uv_" + std::to_string(i) + ".data",
				size, uv, xyz);
			std::cout << "XYZ: " << xyz.size() << std::endl;
			// get the p3d with color
			std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
			convertTo(rgb, xyz, uv, 10, p3dcolor);
			std::cout << "p3dcolor size: " << p3dcolor.size() << std::endl;
			// save the 3D points that describe 1 or multiple bodies
			PCLViewerIONaive<cv::Point3f>::save_naive_pointcloud<cv::Scalar>(
				"my_3d_pose.txt",
				cv::Point3f(0, 0, 0), cv::Point3f(0, 0, 0), p3dcolor);
			// read uv
			if (!rgb.empty()) {
				cv::imshow("rgb", rgb);
				if (cv::waitKey(0) == 27) break;
			}
		}
	}

	/** @brief Example to read saved data

		The data structure is expected to be in the format:
		path\color
		    \depth
	*/
	static void test_read_saved_data_lores(
		const std::string &path,
		const cv::Size &size,
		int fromID, int toID) {

		//// read the intrinsic parameters
		//std::ifstream fin_intrinsic(path + "\\intrinsic.txt");
		//float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0;
		//fin_intrinsic >> width >> height;
		//fin_intrinsic >> fx >> fy >> ppx >> ppy;
		//std::cout << "Intrinsic params: " <<
		//	width << " " << height << " " << fx << " " << fy << " " <<
		//	ppx << " " << ppy << std::endl;
		//float focal_input = (fx + fy) / 2;
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		// scan all the frames
		for (int i = fromID; i < toID; ++i) {
			std::cout << "Frame: " << i << std::endl;
			// copy the image
			//int n_zero = 6;
			//std::string old_string = std::to_string(i);
			//std::string new_string = 
			//	std::string(n_zero - old_string.length(), '0') + old_string;
			std::string new_string =
				co::string::StringOp::append_front_chars(6, i, '0');
			// read image
			std::string fname = path + "\\color\\" + new_string + ".data";
			cv::Mat rgb;
			extract_rgb(fname, size, rgb);
			// read depth
			fname = path + "\\depth\\" + new_string + ".data";
			cv::Mat depth;
			extract_depth(fname, size, depth);

			// create 3D points
			// get the p3d with color
			std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
			for (int y = 0; y < rgb.rows; ++y) {
				for (int x = 0; x < rgb.cols; ++x) {
					cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth, 
						ppx, ppy, focal_input);
					p *= 0.001;
					cv::Scalar color(rgb.at<cv::Vec3b>(y, x)[2],
						rgb.at<cv::Vec3b>(y, x)[1],
						rgb.at<cv::Vec3b>(y, x)[0]);
					p3dcolor.push_back(std::make_pair(p, color));
				}
			}
			// save the 3D points that describe 1 or multiple bodies
			PCLViewerIONaive<cv::Point3f>::save_naive_pointcloud<cv::Scalar>(
				"my_3d_pose.txt",
				cv::Point3f(0, 0, 0), cv::Point3f(0, 0, 0), p3dcolor);

			if (!rgb.empty()) {
				cv::imshow("rgb", rgb);
			}
			if (!depth.empty()) cv::imshow("depth", depth);
			if (cv::waitKey(0) == 27) break;
		}
	}


	/** @brief It creates a 3D point cloud from data saved as low resolution.

		The data structure is expected to be in the format:
		path\color
			\depth
	*/
	static void create_3Dpointcloud_lores(
		const std::string &path,
		const cv::Size &size,
		int fromID, 
		cv::Mat &rgb,
		cv::Mat &map3D) {

		//// read the intrinsic parameters
		//std::ifstream fin_intrinsic(path + "\\intrinsic.txt");
		//float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0;
		//fin_intrinsic >> width >> height;
		//fin_intrinsic >> fx >> fy >> ppx >> ppy;
		//std::cout << "Intrinsic params: " <<
		//	width << " " << height << " " << fx << " " << fy << " " <<
		//	ppx << " " << ppy << std::endl;
		//float focal_input = (fx + fy) / 2;
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		// scan all the frames
		{
			int i = fromID;
			std::cout << "Frame: " << i << std::endl;
			// copy the image
			//int n_zero = 6;
			//std::string old_string = std::to_string(i);
			//std::string new_string =
			//	std::string(n_zero - old_string.length(), '0') + old_string;
			std::string new_string =
				co::string::StringOp::append_front_chars(6, i, '0');
			// read image
			std::string fname = path + "\\color\\" + new_string + ".data";
			extract_rgb(fname, size, rgb);
			// read depth
			fname = path + "\\depth\\" + new_string + ".data";
			cv::Mat depth;
			extract_depth(fname, size, depth);

			// create 3D points
			if (map3D.empty()) {
				map3D = cv::Mat(rgb.size(), CV_32FC3, cv::Scalar::all(0));
			}
			// get the p3d with color
			//std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
			for (int y = 0; y < rgb.rows; ++y) {
				for (int x = 0; x < rgb.cols; ++x) {
					cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth,
						ppx, ppy, focal_input);
					p *= 0.001;
					map3D.at<cv::Vec3f>(y, x) =
						cv::Vec3f(p.x, p.y, p.z);
				}
			}
		}
	}


	/** @brief It creates a 3D point cloud from images.

		The data structure is expected to be in the format:
		path\color
			\depth

		The files are in the format 000000.ext
		where 000000 is total of 6 numbers
		.ext is the extension (.jpg rgb and .png depth)

		@param[in] path Where the files are located.
		@param[in] fromID Which file ID
		@param[in] bin binning of the 3D map
		@param[out] rgb RGB image (8bit 3 channels)
		@param[out] depth depth image (16bit 1 channel)
		@param[out] map3D XYZ coordinates (float 3 channels)
	*/
	static bool create_3Dpointcloud_imgsource(
		const std::string &path, int fromID, int bin,
		cv::Mat &rgb, cv::Mat &depth, cv::Mat &map3D) {

		// read the intrinsic parameters
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		std::string new_string =
			co::text::StringOp::append_front_chars(6, fromID, '0');
		// read image
		std::string fname = path + "\\color\\" + new_string + ".jpg";
		rgb = cv::imread(fname);
		if (rgb.empty()) return false;
		// read depth
		fname = path + "\\depth\\" + new_string + ".png";
		depth = cv::imread(fname, cv::IMREAD_UNCHANGED);
		if (depth.empty()) return false;
		// Get the xyz data in the form of map3D
		get_xyzrgb(rgb, depth, ppx, ppy, focal_input, bin, map3D);
		return true;
	}


	/** @brief It creates a 3D point cloud from images.

		The data structure is expected to be in the format:
		path\color
			\depth
	*/
	static bool create_3Dpointcloud_imgsource(
		const std::string &path, int fromID, int bin,
		cv::Mat &rgb, cv::Mat &map3D) {

		//// read the intrinsic parameters
		//std::ifstream fin_intrinsic(path + "\\intrinsic.txt");
		//float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0;
		//fin_intrinsic >> width >> height;
		//fin_intrinsic >> fx >> fy >> ppx >> ppy;
		//std::cout << "Intrinsic params: " <<
		//	width << " " << height << " " << fx << " " << fy << " " <<
		//	ppx << " " << ppy << std::endl;
		//float focal_input = (fx + fy) / 2;
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		//std::cout << "Frame: " << fromID << std::endl;
		// copy the image
		//int n_zero = 6;
		//std::string old_string = std::to_string(fromID);
		//std::string new_string =
		//	std::string(n_zero - old_string.length(), '0') + old_string;
		std::string new_string =
			co::text::StringOp::append_front_chars(6, fromID, '0');
		// read image
		std::string fname = path + "\\color\\" + new_string + ".jpg";
		rgb = cv::imread(fname);
		if (rgb.empty()) return false;
		// read depth
		fname = path + "\\depth\\" + new_string + ".png";
		cv::Mat depth = cv::imread(fname, cv::IMREAD_UNCHANGED);
		if (depth.empty()) return false;

		// Get the xyz data in the form of map3D
		get_xyzrgb(rgb, depth, ppx, ppy, focal_input, bin, map3D);

		return true;
	}


	/** @brief It creates a 3D point cloud from images.

		The data structure is expected to be in the format:
		path\color
			\depth
	*/
	static bool create_3Dpointcloud_imgsource(
		const std::string &path,
		int fromID, int bin,
		std::vector<std::pair<cv::Point3f, cv::Scalar>> &p3dcolor) {

		//// read the intrinsic parameters
		//std::ifstream fin_intrinsic(path + "\\intrinsic.txt");
		//float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0;
		//fin_intrinsic >> width >> height;
		//fin_intrinsic >> fx >> fy >> ppx >> ppy;
		//std::cout << "Intrinsic params: " <<
		//	width << " " << height << " " << fx << " " << fy << " " <<
		//	ppx << " " << ppy << std::endl;
		//float focal_input = (fx + fy) / 2;
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		std::cout << "Frame: " << fromID << std::endl;
		// copy the image
		//int n_zero = 6;
		//std::string old_string = std::to_string(fromID);
		//std::string new_string =
		//	std::string(n_zero - old_string.length(), '0') + old_string;
		std::string new_string =
			co::text::StringOp::append_front_chars(6, fromID, '0');
		// read image
		std::string fname = path + "\\color\\" + new_string + ".jpg";
		cv::Mat rgb = cv::imread(fname);
		if (rgb.empty()) {
			std::cout << "[-] " << fname << std::endl;
			return false;
		}
		// read depth
		fname = path + "\\depth\\" + new_string + ".png";
		cv::Mat depth = cv::imread(fname, cv::IMREAD_UNCHANGED);
		if (depth.empty()) {
			std::cout << "[-] " << fname << std::endl;
			return false;
		}

		// Get the xyz data in the form of map3D
		get_xyzrgb(rgb, depth, ppx, ppy, focal_input, bin, p3dcolor);

		return true;
	}


	/** @brief It gets the xyzrgb data
	*/
	static void get_xyzrgb(
		cv::Mat &rgb,
		cv::Mat &depth,
		float ppx, float ppy, float focal_input, int bin,
		cv::Mat &map3D) {

		// create 3D points
		if (map3D.empty() || map3D.size() != rgb.size()) {
			map3D = cv::Mat(rgb.size(), CV_32FC3, cv::Scalar::all(0));
		}
		// get the p3d with color
		//std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
		for (int y = 0; y < rgb.rows; y += bin) {
			for (int x = 0; x < rgb.cols; x += bin) {
				cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth,
					ppx, ppy, focal_input);
				p *= 0.001;
				map3D.at<cv::Vec3f>(y, x) =
					cv::Vec3f(p.x, p.y, p.z);
			}
		}
	}


	/** @brief It gets the xyzrgb data
	*/
	static void get_xyzrgb(
		cv::Mat &rgb,
		cv::Mat &depth,
		float ppx, float ppy, float focal_input, int bin,
		std::vector<std::pair<cv::Point3f, cv::Scalar>> &p3dcolor) {
			
		// create 3D points
		// get the p3d with color
		for (int y = 0; y < rgb.rows; y += bin) {
			for (int x = 0; x < rgb.cols; x += bin) {
				cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth,
					ppx, ppy, focal_input);
				p *= 0.001;
				cv::Scalar color(rgb.at<cv::Vec3b>(y, x)[2],
					rgb.at<cv::Vec3b>(y, x)[1],
					rgb.at<cv::Vec3b>(y, x)[0]);
				p3dcolor.push_back(std::make_pair(p, color));
			}
		}
	}

	/** @brief It creates a 3D point cloud from images.

		The data structure is expected to be in the format:
		path\color
			\depth
	*/
	//static bool create_3Dpointcloud_imgsource(
	//	cv::Mat &rgb, cv::Mat &depth,
	//	float ppx, float ppy, float focal_input,
	//	cv::Mat &map3D) {

	//	// create 3D points
	//	if (map3D.empty() || map3D.size() != rgb.size()) {
	//		map3D = cv::Mat(rgb.size(), CV_32FC3, cv::Scalar::all(0));
	//	}
	//	// get the p3d with color
	//	std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
	//	for (int y = 0; y < rgb.rows; ++y) {
	//		for (int x = 0; x < rgb.cols; ++x) {
	//			cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth,
	//				ppx, ppy, focal_input);
	//			p *= 0.001;
	//			map3D.at<cv::Vec3f>(y, x) =
	//				cv::Vec3f(p.x, p.y, p.z);
	//		}
	//	}
	//	return true;
	//}



	/** @brief It reads some data from a set of files.

		The data is in the format XYZRGBA where XYZ is float and RGBA is a 4 byte
		data.
	*/
	static void extract_rgb(
		const std::string &fname_in,
		const cv::Size &size_in,
		cv::Mat &rgb) {
		std::vector<_TyByte> vdata = readFile<_TyByte>(fname_in.c_str());
		//std::cout << "vdata: " << vdata.size() << std::endl;
		if (vdata.size() == 0) return;
		rgb = cv::Mat(size_in, CV_8UC3);
		memcpy(rgb.data, &vdata[0], vdata.size() * sizeof(uchar));
#ifdef CV_VERSION_MAJOR == 4
		cv::cvtColor(rgb, rgb, cv::COLOR_BGR2RGB);
#else
		cv::cvtColor(rgb, rgb, CV_BGR2RGB);
#endif
	}
	/** @brief Extract the depth map
	*/
	static void extract_depth(
		const std::string &fname_in,
		const cv::Size &size_in,
		cv::Mat &depth) {
		std::vector<_TyByte> vdata = readFile<_TyByte>(fname_in.c_str());
		//std::cout << "vdata: " << vdata.size() << std::endl;
		if (vdata.size() == 0) return;
		depth = cv::Mat(size_in, CV_16UC1);
		memcpy(depth.data, &vdata[0], vdata.size() * sizeof(uchar));
	}

	/** @brief It extracts the XYZUV data
	*/
	static void extract_xyzuv(
		const std::string &fname_xyz,
		const std::string &fname_uv,
		int bin,
		std::vector<cv::Point3f> &xyz,
		std::vector<cv::Point2f> &uv) {
		std::vector<_TyByte> xyz_raw_data = readFile<_TyByte>(fname_xyz.c_str());
		std::vector<_TyByte> uv_raw_data = readFile<_TyByte>(fname_uv.c_str());
		std::vector<float> xyzdata = std::vector<float>(xyz_raw_data.size() / sizeof(float));
		std::vector<float> uvdata = std::vector<float>(uv_raw_data.size() / sizeof(float));

		memcpy(xyzdata.data(), xyz_raw_data.data(), sizeof(uchar) * xyz_raw_data.size());
		memcpy(uvdata.data(), uv_raw_data.data(), sizeof(uchar) * uv_raw_data.size());

		//std::cout << "# xyz points: " << xyzdata.size() / 3 << std::endl;
		//std::cout << "# uv points: " << uvdata.size() / 2 << std::endl;

		size_t s = xyzdata.size() / 3;
		for (size_t i = 0; i < s; i += bin) {
			xyz.push_back(cv::Point3f(xyzdata[i * 3], xyzdata[i * 3 + 1],
				xyzdata[i * 3 + 2]));
			uv.push_back(cv::Point2f(uvdata[i * 2], uvdata[i * 2 + 1]));
		}
	}


	/** @brief It extracts the XYZUV data
	*/
	static void extract_xyzuv(
		const std::string &fname_xyz,
		const std::string &fname_uv,
		const cv::Size &size,
		int bin,
		std::map<std::pair<int, int>, cv::Point3f> &uvxyz) {
		std::vector<_TyByte> xyz_raw_data = readFile<_TyByte>(fname_xyz.c_str());
		std::vector<_TyByte> uv_raw_data = readFile<_TyByte>(fname_uv.c_str());
		std::vector<float> xyzdata = std::vector<float>(xyz_raw_data.size() / sizeof(float));
		std::vector<float> uvdata = std::vector<float>(uv_raw_data.size() / sizeof(float));

		memcpy(xyzdata.data(), xyz_raw_data.data(), sizeof(uchar) * xyz_raw_data.size());
		memcpy(uvdata.data(), uv_raw_data.data(), sizeof(uchar) * uv_raw_data.size());

		//std::cout << "# xyz points: " << xyzdata.size() / 3 << std::endl;
		//std::cout << "# uv points: " << uvdata.size() / 2 << std::endl;

		size_t s = xyzdata.size() / 3;
		for (size_t i = 0; i < s; i += bin) {
			uvxyz[std::make_pair(uvdata[i * 2] * size.width, uvdata[i * 2 + 1] * size.height)] =
				cv::Point3f(xyzdata[i * 3], xyzdata[i * 3 + 1],
					xyzdata[i * 3 + 2]);
		}
	}

	static void extract_xyzuv(
		const std::string &fname_xyz,
		const std::string &fname_uv,
		const cv::Size &size,
		int bin,
		std::vector<cv::Point2f> &uv,
		std::vector<cv::Point3f> &xyz) {
		std::vector<_TyByte> xyz_raw_data = readFile<_TyByte>(fname_xyz.c_str());
		std::vector<_TyByte> uv_raw_data = readFile<_TyByte>(fname_uv.c_str());
		std::vector<float> xyzdata = std::vector<float>(xyz_raw_data.size() / sizeof(float));
		std::vector<float> uvdata = std::vector<float>(uv_raw_data.size() / sizeof(float));

		memcpy(xyzdata.data(), xyz_raw_data.data(), sizeof(uchar) * xyz_raw_data.size());
		memcpy(uvdata.data(), uv_raw_data.data(), sizeof(uchar) * uv_raw_data.size());

		//std::cout << "# xyz points: " << xyzdata.size() / 3 << std::endl;
		//std::cout << "# uv points: " << uvdata.size() / 2 << std::endl;

		size_t s = xyzdata.size() / 3;
		for (size_t i = 0; i < s; i += bin) {
			uv.push_back(cv::Point2f(uvdata[i * 2], uvdata[i * 2 + 1]));
			xyz.push_back(cv::Point3f(xyzdata[i * 3], xyzdata[i * 3 + 1],
				xyzdata[i * 3 + 2]));
		}
	}

	/** @brief It extracts the XYZUV data
*/
	static void extract_xyzuv_lores(
		const std::string &path,
		const cv::Size &size,
		int fromID, int bin,
		std::map<std::pair<int, int>, cv::Point3f> &uvxyz) {

		//// read the intrinsic parameters
		//std::ifstream fin_intrinsic(path + "\\intrinsic.txt");
		//float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0;
		//fin_intrinsic >> width >> height;
		//fin_intrinsic >> fx >> fy >> ppx >> ppy;
		//std::cout << "Intrinsic params: " <<
		//	width << " " << height << " " << fx << " " << fy << " " <<
		//	ppx << " " << ppy << std::endl;
		//float focal_input = (fx + fy) / 2;
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		// scan all the frames
		{
			int i = fromID;
			std::cout << "Frame: " << i << std::endl;
			// copy the image
			//int n_zero = 6;
			//std::string old_string = std::to_string(i);
			//std::string new_string =
			//	std::string(n_zero - old_string.length(), '0') + old_string;
			std::string new_string =
				co::text::StringOp::append_front_chars(6, i, '0');
			// read depth
			std::string fname = path + "\\depth\\" + new_string + ".data";
			cv::Mat depth;
			extract_depth(fname, size, depth);

			// Get the 3D points
			for (int y = 0; y < depth.rows; ++y) {
				for (int x = 0; x < depth.cols; ++x) {
					cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth,
						ppx, ppy, focal_input);
					p *= 0.001;
					uvxyz[std::make_pair(x, y)] = p;
				}
			}
		}

	}

	/** @brief It extracts the uvxyz from imgsource

		TODO: Change in template and add function insert
		      due to the code similarity with the lores function
	*/
	static bool extract_xyzuv_imgsource(
		const std::string &path,
		const cv::Size &size,
		int fromID, int bin,
		cv::Mat &rgb,
		std::map<std::pair<int, int>, cv::Point3f> &uvxyz) {

		//// read the intrinsic parameters
		//std::ifstream fin_intrinsic(path + "\\intrinsic.txt");
		//float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0;
		//fin_intrinsic >> width >> height;
		//fin_intrinsic >> fx >> fy >> ppx >> ppy;
		//std::cout << "Intrinsic params: " <<
		//	width << " " << height << " " << fx << " " << fy << " " <<
		//	ppx << " " << ppy << std::endl;
		//float focal_input = (fx + fy) / 2;
		float width = 0, height = 0, fx = 0, fy = 0, ppx = 0, ppy = 0,
			focal_input = 0;
		read_intrinsic(path + "\\intrinsic.txt", width, height, fx, fy,
			ppx, ppy, focal_input);

		std::cout << "Frame: " << fromID << std::endl;
		// copy the image
		//int n_zero = 6;
		//std::string old_string = std::to_string(fromID);
		//std::string new_string =
		//	std::string(n_zero - old_string.length(), '0') + old_string;
		std::string new_string =
			co::text::StringOp::append_front_chars(6, fromID, '0');
		// read image
		std::string fname = path + "\\color\\" + new_string + ".jpg";
		rgb = cv::imread(fname);
		if (rgb.empty()) return false;
		// read depth
		fname = path + "\\depth\\" + new_string + ".png";
		cv::Mat depth = cv::imread(fname, cv::IMREAD_UNCHANGED);
		if (depth.empty()) return false;

		// get the p3d with color
		std::vector<std::pair<cv::Point3f, cv::Scalar>> p3dcolor;
		for (int y = 0; y < rgb.rows; ++y) {
			for (int x = 0; x < rgb.cols; ++x) {
				cv::Point3f p = get_xyz_from_pts(cv::Point2f(x, y), depth,
					ppx, ppy, focal_input);
				p *= 0.001;
				uvxyz[std::make_pair(x, y)] = p;
			}
		}
		return true;
	}


private:

	/** @brief It reads a camera intrinsic parameter.

		It reads a camera intrinsic parameter. The file is structured
		in a naive form:
		width height fx fy ppx ppy focal_input

		@return It returns true in case of success. False otherwise.
	*/
	static bool read_intrinsic(
		const std::string &fname,
		float &width, 
		float &height, 
		float &fx, 
		float &fy, 
		float &ppx, 
		float &ppy,
		float &focal_input) {
		// read the intrinsic parameters
		std::ifstream fin_intrinsic(fname);
		if (fin_intrinsic.is_open()) {
			fin_intrinsic >> width >> height;
			fin_intrinsic >> fx >> fy >> ppx >> ppy;
			//std::cout << "Intrinsic params: " <<
			//	width << " " << height << " " << fx << " " << fy << " " <<
			//	ppx << " " << ppy << std::endl;
			focal_input = (fx + fy) / 2;
			return true;
		}
		return false;
	}

	/** @brief It reads a binary file
	*/
	template<typename _Ty>
	static std::vector<_Ty> readFile(const char* filename)
	{
		// open the file:
		std::ifstream file(filename, std::ios::binary);
		if (!file.is_open()) {
			std::cout << "[-] : " << filename << std::endl;
			return std::vector<_Ty>();
		}

		// Stop eating new lines in binary mode!!!
		file.unsetf(std::ios::skipws);

		// get its size:
		std::streampos fileSize;

		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		// reserve capacity
		std::vector<_Ty> vec;
		vec.reserve(fileSize);

		// read the data:
		vec.insert(vec.begin(),
			std::istream_iterator<_Ty>(file),
			std::istream_iterator<_Ty>());
		return vec;
	}

	static cv::Point3f get_xyz_from_uv(float u, float v, float d, float ppx, float ppy, float focal) {
		float x = 0, y = 0;
		if (focal != 0) {
			x = (u - ppx) / focal * d;
			y = (v - ppy) / focal * d;
		}
		else {
			x = 0;
			y = 0;
		}
		return cv::Point3f(x, y, d);
	}

	static cv::Point2f get_uv_from_xyz(float x, float y, float z, float ppx, float ppy, float focal) {
		float u = 0, v = 0;
		if (z != 0) {
			u = focal * x / z + ppx;
			v = focal * y / z + ppy;
		}
		else {
			u = 0;
			v = 0;
		}
		return cv::Point2f(u, v);
	}

	static cv::Point3f get_xyz_from_pts(cv::Point2f &pts_row, cv::Mat &depth, float ppx, float ppy, float focal) {
		float u = pts_row.x;
		float v = pts_row.y;
		int u0 = int(u);
		int v0 = int(v);
		int height = depth.rows;
		int width = depth.cols;
		// bilinear depth interpolation
		if (u0 > 0 && u0 < width - 1 && v0 > 0 && v0 < height - 1) {
			int up = pts_row.x - u0;
			int vp = pts_row.y - v0;
			float d0 = depth.at<ushort>(v0, u0);
			float d1 = depth.at<ushort>(v0, u0 + 1);
			float d2 = depth.at<ushort>(v0 + 1, u0);
			float d3 = depth.at<ushort>(v0 + 1, u0 + 1);
			float d = (1 - vp) * (d1 * up + d0 * (1 - up)) + vp * (d3 * up + d2 * (1 - up));
			return get_xyz_from_uv(u, v, d, ppx, ppy, focal);
		}
		else {
			return cv::Point3f(0, 0, 0);
		}
	}


	/** @brief Get the valid points information (xyzrgba)

		@param[in] m Source image.
		@param[in] xyz XYZ points coordinate.
		@param[in] uv UV image coordinate (same size and order of XYZ).
		@param[in] kRange Range where the data is valid.
		@param[out] p3d Point cloud with associated color.
	*/
	static void convertTo(
		cv::Mat &m,
		std::vector<cv::Point3f> &xyz,
		std::vector<cv::Point2f> &uv,
		float kRange,
		std::vector<std::pair<cv::Point3f, cv::Scalar>> &p3d) {

		int bin = 1;
		if (bin <= 0) bin = 1;

		size_t num_points = xyz.size();
		std::cout << "# points: " << num_points << std::endl;
		// total number of points
		for (int i = 0; i < num_points; i += bin)
		{
			//std::cout << "XYZ: " << xyz[i] << " " << uv[i] << std::endl;
			if (xyz[i].z > 0) {
				if (uv[i].x >= 0 &&
					uv[i].x <= 1 &&
					uv[i].y >= 0 &&
					uv[i].y <= 1) {

					// inside a valid range
					if (xyz[i].x > -kRange && xyz[i].x < kRange &&
						xyz[i].y > -kRange && xyz[i].y < kRange &&
						xyz[i].z > -kRange && xyz[i].z < kRange) {

						cv::Scalar color(
							m.at<cv::Vec3b>(uv[i].y * (m.rows - 1), uv[i].x * (m.cols - 1))[2],
							m.at<cv::Vec3b>(uv[i].y * (m.rows - 1), uv[i].x * (m.cols - 1))[1],
							m.at<cv::Vec3b>(uv[i].y * (m.rows - 1), uv[i].x * (m.cols - 1))[0]);
						p3d.push_back(std::make_pair(xyz[i], color));
					} // inside a valid range
				} // inside the texture
				else
				{
					// copy the xyzrgba
					if (xyz[i].x > -kRange && xyz[i].x < kRange &&
						xyz[i].y > -kRange && xyz[i].y < kRange &&
						xyz[i].z > -kRange && xyz[i].z < kRange) {
						cv::Scalar color(255, 0, 255);
						p3d.push_back(std::make_pair(xyz[i], color));
					} // inside a valid range
				}

			}
		}
	}
};

} // namespace io

 // It defines the string classes of the enumerator
template<> char const* enumStrings<io::SaveMode>::data[] = { "sm_NotSet", "sm_HiRes", "sm_LoRes","sm_Encoded" };

} // namespace co

#endif // COMMONOBJECTS_IO_RGBDREADER_HPP__