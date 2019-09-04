/**
* @file PCLViewerIONaive.hpp
* @brief Header to save and load data for PCL viewer in a naive format
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
* @version 0.2.0.0
*
*/
#ifndef COMMONOBJECTS_IO_PCLVIEWERNAIVE_HPP__
#define COMMONOBJECTS_IO_PCLVIEWERNAIVE_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace co
{
namespace io
{
/** @brief Class to save a set of intersection data for visualization.
*/
template <typename _Ty3>
class PCLViewerIONaive
{
public:

	/** @brief It saves a set of points and observer in a naive form
	*/
	static bool save(const std::string &fname,
		_Ty3 &origin, std::vector<_Ty3> &dir, float scale_dir,
		std::vector<std::pair<size_t, _Ty3> > &intersections) {

		std::ofstream f(fname);
		if (!f.is_open()) return false;

		f << "PCLViewerIONaive v 0.1.0" << std::endl;
		f << "type(0 point, 1 sphere, 2 line) x0 y0 z0 x1 y1 z1 r g b radius" << std::endl;
		f << "1 " << origin.x << " " << origin.y << " " << origin.z << " " <<
			" 0 0 0 255 255 255 0.05" << std::endl;
		for (auto &it : dir) {
			f << "0 " << origin.x + (it.x * scale_dir) << " " <<
				origin.y + (it.y * scale_dir) << " " <<
				origin.z + (it.z * scale_dir) <<
				" 0 0 0 0 255 0 0" << std::endl;
		}
		for (auto &it : intersections) {
			if (it.first != -1) {
				f << "0 " << it.second.x << " " << it.second.y << " " << it.second.z << " 0 0 0 0 0 255 0" << std::endl;
			}
		}
		f.close();
		f.clear();
	}


	/** @brief It saves a set of points and observer in a naive form
	*/
	static bool save(const std::string &fname,
		_Ty3 &origin, std::vector<_Ty3> &dir, float scale_dir,
		std::vector<std::vector<_Ty3> > &v_triangles,
		std::vector<std::pair<size_t, _Ty3> > &intersections) {

		std::ofstream f(fname);
		if (!f.is_open()) return false;

		f << "PCLViewerIONaive v 0.1.0" << std::endl;
		f << "type(0 point, 1 sphere, 2 line) x0 y0 z0 x1 y1 z1 r g b radius" << std::endl;
		f << "1 " << origin.x << " " << origin.y << " " << origin.z << " " <<
			" 0 0 0 255 255 255 0.05" << std::endl;
		for (auto &it : dir) {
			f << "0 " << origin.x + (it.x * scale_dir) << " " <<
				origin.y + (it.y * scale_dir) << " " <<
				origin.z + (it.z * scale_dir) <<
				" 0 0 0 0 255 0 0" << std::endl;
		}
		for (auto &it : intersections) {
			if (it.first != -1) {
				f << "0 " << it.second.x << " " << it.second.y << " " << it.second.z << " 0 0 0 0 0 255 0" << std::endl;
			}
		}
		for (auto &it : v_triangles) {
			for (auto &it2 : it) {
				f << "0 " << it2.x << " " << it2.y << " " << it2.z << " 0 0 0 255 255 0 0" << std::endl;
			}
		}
		f.close();
		f.clear();
	}


	/** @brief It saves a set of points and observer in a naive form
	*/
	static bool save(const std::string &fname,
		const _Ty3 &origin, std::vector<_Ty3> &dir, float scale_dir,
		std::vector<std::vector<_Ty3> > &v_triangles,
		std::vector<std::pair<size_t, _Ty3> > &intersections) {

		std::ofstream f(fname);
		if (!f.is_open()) return false;

		f << "PCLViewerIONaive v 0.1.0" << std::endl;
		f << "type(0 point, 1 sphere, 2 line) x0 y0 z0 x1 y1 z1 r g b radius" << std::endl;
		f << "1 " << origin.x << " " << origin.y << " " << origin.z << " " <<
			" 0 0 0 255 255 255 0.05" << std::endl;
		for (auto &it : dir) {
			f << "0 " << origin.x + (it.x * scale_dir) << " " <<
				origin.y + (it.y * scale_dir) << " " <<
				origin.z + (it.z * scale_dir) <<
				" 0 0 0 0 255 0 0" << std::endl;
		}
		for (auto &it : intersections) {
			if (it.first != -1) {
				f << "0 " << it.second.x << " " << it.second.y << " " << it.second.z << " 0 0 0 0 0 255 0" << std::endl;
			}
		}
		for (auto &it : v_triangles) {
			for (auto &it2 : it) {
				f << "0 " << it2.x << " " << it2.y << " " << it2.z << " 0 0 0 255 255 0 0" << std::endl;
			}
		}
		f.close();
		f.clear();
	}

	/** @brief It saves the current scene for debug
	*/
	template <typename _Scalar>
	static bool save_naive(const std::string &fname,
		const _Ty3 &p, const _Ty3 &o, std::vector<std::pair<_Ty3, _Scalar>> &pts) {

		std::ofstream f(fname);
		if (!f.is_open()) return false;

		f << "PCLViewerIONaive v 0.1.0" << std::endl;
		f << "type(0 point, 1 sphere, 2 line) x0 y0 z0 x1 y1 z1 r g b radius" << std::endl;
		f << "1 " << p.x << " " << p.y << " " << p.z <<
			" 0 0 0 255 255 255 0.001" << std::endl;
		f << "2 " << p.x << " " << p.y << " " << p.z <<
			" " << o.x << " " << o.y << " " << o.z << " 255 255 255 0.001" << std::endl;
		for (auto &it : pts) {
			f << "1 " << it.first.x << " " << it.first.y << " " << it.first.z <<
				" 0 0 0 " << it.second[0] << " " << it.second[1] << " " <<
				it.second[2] << " 0.01" << std::endl;
		}
	}

	/** @brief It saves the current scene for debug
	*/
	template <typename _Scalar>
	static bool save_naive_pointcloud(const std::string &fname,
		const _Ty3 &p, const _Ty3 &o, std::vector<std::pair<_Ty3, _Scalar>> &pts) {

		std::ofstream f(fname);
		if (!f.is_open()) return false;

		f << "PCLViewerIONaive v 0.1.0" << std::endl;
		f << "type(0 point, 1 sphere, 2 line) x0 y0 z0 x1 y1 z1 r g b radius" << std::endl;
		f << "1 " << p.x << " " << p.y << " " << p.z <<
			" 0 0 0 255 255 255 0.001" << std::endl;
		f << "2 " << p.x << " " << p.y << " " << p.z <<
			" " << o.x << " " << o.y << " " << o.z << " 255 255 255 0.001" << std::endl;
		for (auto &it : pts) {
			f << "0 " << it.first.x << " " << it.first.y << " " << it.first.z <<
				" 0 0 0 " << it.second[0] << " " << it.second[1] << " " <<
				it.second[2] << " 0.01" << std::endl;
		}
	}

	/** @brief It saves the current scene for debug
	*/
	template <typename _Scalar>
	static bool save_naive_spheres(const std::string &fname,
		const _Ty3 &p, const _Ty3 &o, std::vector<std::pair<_Ty3, _Scalar>> &pts) {

		std::ofstream f(fname);
		if (!f.is_open()) return false;

		f << "PCLViewerIONaive v 0.1.0" << std::endl;
		f << "type(0 point, 1 sphere, 2 line) x0 y0 z0 x1 y1 z1 r g b radius" << std::endl;
		f << "1 " << p.x << " " << p.y << " " << p.z <<
			" 0 0 0 255 255 255 0.001" << std::endl;
		f << "2 " << p.x << " " << p.y << " " << p.z <<
			" " << o.x << " " << o.y << " " << o.z << " 255 255 255 0.001" << std::endl;
		for (auto &it : pts) {
			f << "1 " << it.first.x << " " << it.first.y << " " << it.first.z <<
				" 0 0 0 " << it.second[0] << " " << it.second[1] << " " <<
				it.second[2] << " 0.01" << std::endl;
		}
	}

	template <typename _Scalar>
	static bool save_naive(const std::string &fname,
		const _Ty3 &p, const _Ty3 &o, std::vector<_Ty3> &pts) {

		std::vector<std::pair<_Ty3, _Scalar>> pts_in;
		for (auto &it : pts) pts_in.push_back(std::make_pair(it, 
			_Scalar(255, 255, 0)));
		return save_naive(fname, p, o, pts_in);
	}

	static bool load_naive(const std::string &fname,
		std::map<int, std::vector< std::pair< std::pair< _Ty3, _Ty3>, std::pair<_Ty3, float> > > > &items)
	{
		std::ifstream myfile(fname);
		if (!myfile.is_open()) return false;
		char buf[512];
		myfile.getline(buf, 512);
		myfile.getline(buf, 512);
		// Scan the data type. Detect the light points
		while (!myfile.eof())
		{
			int type = 0;
			float x = 0, y = 0, z = 0, r = 0, g = 0, b = 0, radius = 0;
			float x2 = 0, y2 = 0, z2 = 0;
			myfile.getline(buf, 512);
			if (strlen(buf) > 3)
			{
				//std::cout << "Line: " << buf << std::endl;
				sscanf(buf, "%i %f %f %f %f %f %f %f %f %f %f", &type, &x, &y, &z, &x2, &y2, &z2, &r, &g, &b, &radius);
				//std::cout << "type: " << type << std::endl;
				items[type].push_back(std::make_pair(std::make_pair(_Ty3(x, y, z), _Ty3(x2, y2, z2)),
					std::make_pair(_Ty3(b, g, r), radius)));
			}
		}
		myfile.close();
		myfile.clear();

		return true;
	}

};

} // namespace io
} // namespace co

#endif // COMMONOBJECTS_IO_PCLVIEWERNAIVE_HPP__