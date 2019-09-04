/**
* @file CalibrationPointsIO.hpp
* @brief Header related to linear algebra structures
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
* @author Alessandro Moro
* @bug No known bugs.
* @version 0.1.0.0
*
*/

#ifndef COMMONOBJECTS_IO_CALIBRATIONPOINTSIO_HPP__
#define COMMONOBJECTS_IO_CALIBRATIONPOINTSIO_HPP__

#include <iostream>
#include <fstream>

namespace co
{
namespace io
{

/** @brief Class to manage the calibration points

	Used with OpenCV
	_Ty2 : cv::Point2f
	_Ty3 : cv::Point3f
	_Scalar : cv::Scalar
	REAL : float
*/
template <typename _Ty2, typename _Ty3, 
	typename _Scalar, typename REAL>
class CalibrationPoints
{
public:

	/** @brief Open a file with the calibration points.

	Open a file with the calibration points.
	@param[in] filename File to open.
	@param[out] v3dpoints Container with points in 3D coordinates and
	associated color.
	@param[out] v2dpoints Container with points in 2D coordinates and
	associated color.
	@param[out] vdiameter Container with the real size of a point as sphere.
	@return Return true in case of success. False otherwise.
	*/
	static bool open_points_xyz_xy(const std::string &filename,
		std::vector<std::pair<_Ty3, _Scalar>> &v3dpoints,
		std::vector<std::pair<_Ty2, _Scalar>> &v2dpoints,
		std::vector<std::pair<REAL, REAL>> &vdiameter)
	{
		std::ifstream f(filename);
		int file_mode = 0;
		if (f.is_open())
		{
			while (!f.eof())
			{
				char buf[512];
				f.getline(buf, 512);
				if (strlen(buf) > 3) {
					if (file_mode == 0 &&
						strcmp(buf, "calibration_points v0.1") == 0) {
						file_mode = 1;
						// It drops the comment line
						f.getline(buf, 512);
						continue;
					}
					if (file_mode == 0 &&
						strcmp(buf, "calibration_points v0.2") == 0) {
						file_mode = 2;
						// It drops the comment line
						f.getline(buf, 512);
						continue;
					}

					// Scan the points
					_Ty2 p2d;
					_Ty3 p3d;
					REAL r, g, b;
					REAL diameter = -1, diameter_expected = -1;
					_Scalar color;
					if (file_mode == 1) {
						sscanf(buf, "%f %f %f %f %f %f %f %f %f",
							&p3d.x, &p3d.y, &p3d.z,
							&p2d.x, &p2d.y, &r, &g, &b, &diameter);
						color = _Scalar(r, g, b);
						v2dpoints.push_back(std::make_pair(p2d, color));
						v3dpoints.push_back(std::make_pair(p3d, color));
						vdiameter.push_back(std::make_pair(diameter, 0));
					}
					else if (file_mode == 2) {
						sscanf(buf, "%f %f %f %f %f %f %f %f %f %f",
							&p3d.x, &p3d.y, &p3d.z,
							&p2d.x, &p2d.y, &r, &g, &b, &diameter,
							&diameter_expected);
						color = _Scalar(r, g, b);
						v2dpoints.push_back(std::make_pair(p2d, color));
						v3dpoints.push_back(std::make_pair(p3d, color));
						vdiameter.push_back(std::make_pair(diameter, diameter_expected));
					}
					else {
						sscanf(buf, "%f %f %f %f %f %f",
							&p3d.x, &p3d.y, &p3d.z,
							&p2d.x, &p2d.y, &r, &g, &b);
						color = _Scalar(r, g, b);
						v3dpoints.push_back(std::make_pair(p3d, color));
					}
					//std::cout << "Add: " << p3d << " " << p2d << " " << 
					//	color << " " << diameter << std::endl;
				}
			}
			f.close();
			f.clear();
			return true;
		}
		return false;
	}


	/** @brief It saves the points for calibra
	*/
	static bool save_points_xyz_xy(const std::string &filename,
		std::vector<std::pair<_Ty3, _Scalar>> &v3dpoints,
		std::vector<std::pair<_Ty2, _Scalar>> &v2dpoints,
		std::vector<std::pair<REAL, REAL>> &vdiameter)
	{
		if (v3dpoints.size() != v2dpoints.size()) return false;
		if (v3dpoints.size() != vdiameter.size()) return false;
		if (v3dpoints.size() == 0) return false;

		std::ofstream f(filename);
		int file_mode = 0;
		if (f.is_open())
		{
			f << "calibration_points v0.2" << std::endl;
			f << "x(mm) y(mm) z(mm) u_x(px) v_y(px) r g b diameter(mm) diameter_ex(px)" << std::endl;
			size_t s = v3dpoints.size();
			for (size_t i = 0; i < s; ++i) {
				f << v3dpoints[i].first.x << " " << v3dpoints[i].first.y <<
					" " << v3dpoints[i].first.z << " " <<
					v2dpoints[i].first.x << " " << v2dpoints[i].first.y <<
					" " <<
					v3dpoints[i].second[0] << " " << v3dpoints[i].second[1] <<
					" " << v3dpoints[i].second[2] << " " <<
					vdiameter[i].first << " " << vdiameter[i].second <<
					std::endl;
			}
			f.close();
			f.clear();
			return true;
		}
		return false;
	}

	/** @brief It saves the points for calibration.
	*/
	static bool save_points_xyz_xy(const std::string &filename,
		std::vector<_Ty3> &v3dpoints)
	{
		if (v3dpoints.size() == 0) return false;

		std::ofstream f(filename);
		int file_mode = 0;
		if (f.is_open())
		{
			f << "calibration_points v0.2" << std::endl;
			f << "x(mm) y(mm) z(mm) u_x(px) v_y(px) r g b diameter(mm) diameter_ex(px)" << std::endl;
			size_t s = v3dpoints.size();
			for (size_t i = 0; i < s; ++i) {
				f << v3dpoints[i].x << " " << v3dpoints[i].y <<
					" " << v3dpoints[i].z << " 0 0 0 255 0 0 0" << std::endl;
			}
			f.close();
			f.clear();
			return true;
		}
		return false;
	}


};

} // namespace io
} // namespace co

#endif // COMMONOBJECTS_IO_CALIBRATIONPOINTSIO_HPP__