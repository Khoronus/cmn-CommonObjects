/**
* @file io_calibrationpointsio.cpp
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

#include "../../commonobjects/io/CalibrationPointsIO.hpp"


void main() {
	std::cout << "CalibrationPointsIO" << std::endl;

	std::vector<std::pair<cv::Point3f, cv::Scalar>> v3dpoints;
	std::vector<std::pair<cv::Point2f, cv::Scalar>> v2dpoints;
	std::vector<std::pair<float, float>> vdiameter;

	v3dpoints.push_back(std::make_pair(cv::Point3f(10, 20, 30), cv::Scalar(0, 0, 255)));
	v3dpoints.push_back(std::make_pair(cv::Point3f(10.1, 20.2, 30.3), cv::Scalar(1, 0, 255)));
	v3dpoints.push_back(std::make_pair(cv::Point3f(20.3, 24.6, 37.345), cv::Scalar(2, 0, 255)));

	v2dpoints.push_back(std::make_pair(cv::Point2f(10, 30), cv::Scalar(0, 0, 255)));
	v2dpoints.push_back(std::make_pair(cv::Point2f(10.1, 30.3), cv::Scalar(0, 1, 255)));
	v2dpoints.push_back(std::make_pair(cv::Point2f(20.3, 37.345), cv::Scalar(0, 2, 255)));

	vdiameter.push_back(std::make_pair(4.34, 6.71));
	vdiameter.push_back(std::make_pair(14.34, 63.71));
	vdiameter.push_back(std::make_pair(24.34, 76.71));

	std::vector<std::pair<cv::Point3f, cv::Scalar>> v3dpoints_o;
	std::vector<std::pair<cv::Point2f, cv::Scalar>> v2dpoints_o;
	std::vector<std::pair<float, float>> vdiameter_o;

	// Save full
	co::io::CalibrationPoints<cv::Point2f, cv::Point3f, cv::Scalar, float>::save_points_xyz_xy(
		"sample_savecalib.txt", v3dpoints, v2dpoints, vdiameter);

	std::vector<cv::Point3f> v3dpoints_in;
	v3dpoints_in.push_back(cv::Point3f(10, 20, 30));
	v3dpoints_in.push_back(cv::Point3f(10.1, 20.2, 30.3));
	v3dpoints_in.push_back(cv::Point3f(20.3, 24.6, 37.345));

	co::io::CalibrationPoints<cv::Point2f, cv::Point3f, cv::Scalar, float>::save_points_xyz_xy(
		"sample_savecalib_only3D.txt", v3dpoints_in);

	// Load
	co::io::CalibrationPoints<cv::Point2f, cv::Point3f, cv::Scalar, float>::open_points_xyz_xy(
		"sample_savecalib.txt", v3dpoints_o, v2dpoints_o, vdiameter_o);

	for (int i = 0; i < 3; ++i) {
		std::cout << v3dpoints[i].first << " " << v3dpoints_o[i].first << std::endl;
		std::cout << v3dpoints[i].second << " " << v3dpoints_o[i].second << std::endl;
		std::cout << v2dpoints[i].first << " " << v2dpoints_o[i].first << std::endl;
		std::cout << v2dpoints[i].second << " " << v2dpoints_o[i].second << std::endl;
		std::cout << vdiameter[i].first<< " " << vdiameter_o[i].first << std::endl;
		std::cout << vdiameter[i].second << " " << vdiameter_o[i].second << std::endl;
	}

}