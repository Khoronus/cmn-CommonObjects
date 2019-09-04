/**
* @file RGBDRecorder.hpp
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
* @version 0.2.0.0
*
* IMPORTANT: StoreData record MUST be included and linked
*/
#ifndef COMMONOBJECTS_IO_RGBDRECORDER_HPP__
#define COMMONOBJECTS_IO_RGBDRECORDER_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "../string_common/StringOp.hpp"

#include "record/record_headers.hpp"
#include "RGBDData.hpp"

namespace co
{
namespace io
{

/** @brief Class to record the RealSense data
*/
class RGBDRecorder
{
public:

	RGBDRecorder() {
		save_mode_ = sm_NotSet;
	}

	~RGBDRecorder() {
		flush();
	}

	/** @brief It gets the size of the buffers
	*/
	void size() {
		std::cout << "#record_container_: " << record_container_.size() << std::endl;
		for (auto &it : record_container_) {
			std::cout << "Size[" << it.first << "]: " <<
				it.second.size_about() << " " <<
				it.second.size_about_micro() << std::endl;
		}
	}

	/** @brief It saves the intrinsic parameters
	*/
	void save_intrinsic(
		const std::string &path,
		std::vector<std::string> &intrinsic_lines) {

		std::ofstream fout_intrinsic(path + "\\intrinsic.txt");
		if (fout_intrinsic.is_open()) {
			for (auto &it : intrinsic_lines) {
				fout_intrinsic << it << std::endl;
			}
			fout_intrinsic.close();
			fout_intrinsic.clear();
		}
	}

	/** @brief It tries to add a new frame to the set

		@param[in] path Where to save
		@param[in] num_frame_save Frame index identifier
		@param[in] color unsigned char* data 
		@param[in] color_size color size in bytes
		@param[in] vertices float* data
		@param[in] vertices_size vertices size in bytes
		@param[in] tex_coords float* data
		@param[in] tex_coords_size vertices size in bytes
	*/
	void add(
		const std::string &path,
		unsigned int num_frame_save,
		const void* color,
		size_t color_size,
		const void* vertices,
		size_t vertices_size,
		const void* tex_coords,
		size_t tex_coords_size) {

		// set the hires
		set_save_mode(sm_HiRes);

		// Add the record to save on a separate thread
		RecordContainerData rcd0, rcd1, rcd2;
		// copy the image
		std::string fname = path + "\\frame_" +
			std::to_string(num_frame_save) + ".data";
		rcd0.copyFrom(color, color_size);
		record_container_[0].push(fname, rcd0);
		// copy the xyz
		fname = path + "\\xyz_" +
			std::to_string(num_frame_save) + ".data";
		rcd1.copyFrom(vertices, vertices_size);
		record_container_[1].push(fname, rcd1);
		// copy the uv
		fname = path + "\\uv_" +
			std::to_string(num_frame_save) + ".data";
		rcd2.copyFrom(tex_coords, tex_coords_size);
		record_container_[2].push(fname, rcd2);
	}

	/** @brief It tries to add a new frame to the set

		@param[in] path Where to save
		@param[in] num_frame_save Frame index identifier
		@param[in] color unsigned char* data
		@param[in] color_size color size in bytes
		@param[in] depth short* data
		@param[in] depth_size vertices size in bytes
	*/
	void add(
		const std::string &path,
		unsigned int num_frame_save,
		const void* color,
		size_t color_size,
		const void* depth,
		size_t depth_size) {

		// set the hires
		set_save_mode(sm_LoRes);

		// Add the record to save on a separate thread
		RecordContainerData rcd0, rcd1;
		// copy the image
		//int n_zero = 6;
		//std::string old_string = std::to_string(num_frame_save);
		//std::string new_string = std::string(n_zero - old_string.length(), '0') + old_string;
		std::string new_string =
			co::text::StringOp::append_front_chars(6, num_frame_save, '0');
		std::string fname = path + "\\color\\" + new_string + ".data";
		rcd0.copyFrom(color, color_size);
		record_container_[0].push(fname, rcd0);
		fname = path + "\\depth\\" + new_string + ".data";
		rcd1.copyFrom(depth, depth_size);
		record_container_[1].push(fname, rcd1);
	}

	/** @brief It tries to add a new frame to the set

		The data is encoded as image. JPG for the image, PNG for the depth
		Color image and depth image are expected to have the same size.

		@param[in] path Where to save
		@param[in] num_frame_save Frame index identifier
		@param[in] color unsigned char* data
		@param[in] depth short* data
		@param[in] width image width
		@param[in] height image height
	*/
	void add(
		const std::string &path,
		unsigned int num_frame_save,
		const void* color,
		const void* depth,
		int width, 
		int height) {

		// set the hires
		set_save_mode(sm_Encoded);

		// Add the record to save on a separate thread
		RecordContainerData rcd0, rcd1;
		// copy the image
		//int n_zero = 6;
		//std::string old_string = std::to_string(num_frame_save);
		//std::string new_string = std::string(n_zero - old_string.length(), '0') + old_string;
		std::string new_string = 
			co::text::StringOp::append_front_chars(6, num_frame_save, '0');
		std::string fname = path + "\\color\\" + new_string + ".jpg";
		// encode the rgb source
		std::vector<uchar> buf;
		cv::imencode(".jpg", cv::Mat(height,
			width, CV_8UC3, const_cast<void*>(color)),
			buf);
		rcd0.copyFrom(&buf[0], buf.size());
		record_container_[0].push(fname, rcd0);
		fname = path + "\\depth\\" + new_string + ".png";
		cv::imencode(".png", cv::Mat(height,
			width, CV_16UC1, const_cast<void*>(depth)),
			buf);
		rcd1.copyFrom(&buf[0], buf.size());
		record_container_[1].push(fname, rcd1);
	}

	void add_raw(
		const std::string &path,
		unsigned int num_frame_save,
		const void* color,
		const void* depth,
		int width,
		int height) {

		// set the hires
		set_save_mode(sm_Encoded);

		// Add the record to save on a separate thread
		RecordContainerData rcd0, rcd1;
		// copy the image
		std::string new_string =
			co::text::StringOp::append_front_chars(6, num_frame_save, '0');
		std::string fname = path + "\\color\\" + new_string + ".rgb_raw";

		rcd0.copyFrom(const_cast<void*>(color), width * height * 3 * sizeof(unsigned char));
		record_container_[0].push(fname, rcd0);
		fname = path + "\\depth\\" + new_string + ".depth_raw";
		rcd1.copyFrom(const_cast<void*>(depth), width * height * sizeof(unsigned short));
		record_container_[1].push(fname, rcd1);
	}

	/** @brief It force the flush of the buffers
	*/
	void force_flush() {
		// check if the buffer contains data
		bool is_empty = true;
		for (auto &it : record_container_)
		{
			if (it.second.size_about() > 0) is_empty = false;
		}
		// if there is data, empty it
		if (!is_empty) flush();
	}

private:

	/** @brief Container with the data to record
	*/
	std::map<int, RecordContainer> record_container_;

	// If true, it saves the data at highest quality possible
	// It requires a larger amount of data
	SaveMode save_mode_ = sm_Encoded;

	/** @brief It sets the total number of recorders used
	*/
	void set_total_recorder(int num_recorder) {
		// stop the current running recorders
		for (auto &it : record_container_) {
			it.second.stop();
		}
		// clear the container
		record_container_.clear();
		// set new containers
		for (int i = 0; i < num_recorder; ++i) {
			std::cout << "Start recorder: " << i << std::endl;
			// start to record
			record_container_[i].start();
		}
	}

	/** @brief It returns the mode the data is saved
	*/
	SaveMode save_mode() {
		return save_mode_;
	}

	/** @brief It sets the save mode.

		It sets the save mode.
		It also set the total running recorders.
	*/
	void set_save_mode(const SaveMode &save_mode) {
		if (save_mode_ == save_mode) return;
		// flush the previous if different
		flush();
		// set the new mode
		save_mode_ = save_mode;
		switch (save_mode_)
		{
		case sm_HiRes:
			set_total_recorder(3);
			break;
		case sm_LoRes:
			set_total_recorder(2);
			break;
		case sm_Encoded:
			set_total_recorder(2);
			break;
		default:
			break;
		}
	}

	/** @brief It flush the container
	*/
	void flush() {
		// Flush the buffer
		for (auto &it : record_container_)
		{
			it.second.wait_until_buffer_is_empty(10000, 5);
			it.second.stop();
		}
	}

};

} // namespace io
} // namespace co

#endif // COMMONOBJECTS_IO_RGBDRECORDER_HPP__