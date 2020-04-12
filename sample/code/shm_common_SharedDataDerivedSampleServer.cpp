/**
* @file sample_sharedmemory_server.cpp
* @brief Example of point cloud server
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
* @version 0.2.0.0
*
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <chrono>
#include <thread>

//#include "doc_managedmemory_shared_data_v2.hpp"

#include <opencv2/opencv.hpp>

//#include <Windows.h>

#include "commonobjects/shm_common/SharedDataDerivedSample.hpp"
#include "commonobjects/string_common/StringOp.hpp"

namespace
{


/** @brief The current version of the program
*/
const std::string version_ = "0.1.0.0";

/** @brief Callback functions

	It should be thread safe since is called back from a mutex protected function
*/
void my_func(int object_id, co::shm::SharedMemoryManager &smm) {
	std::cout << "server_side: received" << std::endl;
	std::cout << "msg: " << smm.get("cmd1") << std::endl;

	//std::cout << "Received[" << id << "]: " << size << " bytes" << std::endl;
	//cv::Mat m(480, 640, CV_8UC3, data);
	//cv::imshow("mserver" + std::to_string(id), m);
	//cv::waitKey(1);
}

/** @brief Example of shared data
*/
void share_data_test() {

	// shared data
	co::shm::SharedDataDerivedSample shared_data_server;

	// Set the shared data
	std::string name_shm = "MySharedMemory";
	int width = 640, height = 480;
	int num_skeletons = 10;
	int num_points_skeletons = 18;
	// which objects to allocate
	// change to type, params(name, etc...)
	// add to the shared object type and name
	std::string cmd = 
		"image,rgb," + std::to_string(width) + "," + std::to_string(height) + ",3" +
		"|image,depth," + std::to_string(width) + "," + std::to_string(height) + ",2" +
		"|image,rgbskel," + std::to_string(width) + "," + std::to_string(height) + ",3" +
		"|pose_kSize,skeleton,12," + std::to_string(num_skeletons) + "," + std::to_string(num_points_skeletons) +
		"|instruction,cmd0,4096" +
	    "|instruction,cmd1,4096" +
		"|instruction,req0,4096" +
		"|instruction,req1,4096";

	std::cout << "[shm::cmd]>>" << cmd << std::endl;
	// parse the command and allocate the memory
	int err = shared_data_server.parse(name_shm, cmd);
	if (err != co::shm::kSharedNoError) {
		std::cout << "shared_data error: " << err << std::endl;
	}
	// bind the function
	shared_data_server.registerCallback(std::bind(&my_func,
		std::placeholders::_1, std::placeholders::_2));

	// Object ID
	size_t key_req0 = shared_data_server.get_key_id("req0");
	if (key_req0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_req0" << std::endl;
		return;
	}
	size_t key_req1 = shared_data_server.get_key_id("req1");
	if (key_req1 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_req1" << std::endl;
		return;
	}
	size_t key_cmd0 = shared_data_server.get_key_id("cmd0");
	if (key_cmd0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_cmd0" << std::endl;
		return;
	}
	size_t key_cmd1 = shared_data_server.get_key_id("cmd1");
	if (key_cmd1 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_cmd1" << std::endl;
		return;
	}

	size_t key_image = shared_data_server.get_key_id("rgb");
	if (key_image == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the image" << std::endl;
		return;
	}
	size_t key_image2 = shared_data_server.get_key_id("rgbskel");
	if (key_image2 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the image" << std::endl;
		return;
	}

	// create and start
	//shared_data_server.start(kThreadPriorityBackgroundBegin);
	shared_data_server.start(std::vector<size_t>{key_cmd1},
		co::shm::kThreadPriorityBackgroundBegin);

	// test words
	std::vector<std::string> words = { "hello", "0123456", "qwe", "$#!#$%&()", "987643" };

	// main
	try {
		// Get the image information
		int cols = 0, rows = 0, channels = 0;
		shared_data_server.get_image_size("rgb", cols, rows, channels);
		std::cout << "Expected image [" << cols << "," << rows << "," <<
			channels << "]" << std::endl;

		// It allocates the memory for the shared images
		size_t size = 0;
		cv::Mat rgb(rows, cols, CV_8UC3,
			shared_data_server.get_object_ptr(key_image, size));
		cv::Mat skel(rows, cols, CV_8UC3,
			shared_data_server.get_object_ptr(key_image2, size));

		// image used to close the program
		cv::Mat m(10, 10, CV_8UC3, cv::Scalar(0, 255));
		bool do_continue = true;
		int num_frame = 0;
		std::cout << "All the instructions are for demonstration only" << 
			std::endl;
		std::cout << "q: quit" << std::endl;
		std::cout << "r: start record" << std::endl;
		std::cout << "s: stop record" << std::endl;
		std::cout << "p: set path" << std::endl;
		std::cout << "n: is runtime on" << std::endl;
		std::cout << "m: is runtime off" << std::endl;
		while (do_continue) {

			cv::imshow("rgb", rgb);
			cv::imshow("skel", skel);

			cv::imshow("server", m);
			char c = cv::waitKey(30);
			//shared_data_server.push_data(key_instruction, 
			//	words[num_frame % words.size()]);
			//shared_data_server.notify();
			switch (c) {
			case 'q':
			case 27:
				do_continue = false;
				break;
			case 'r':
				std::cout << "pushed r" << std::endl;
				shared_data_server.push_data(key_req0, "record|1");
				break;
			case 's':
				std::cout << "pushed s" << std::endl;
				shared_data_server.push_data(key_req0, "record|0");
				break;
			case 'p':
				std::cout << "pushed p" << std::endl;
				shared_data_server.push_data(key_req0, "path|d:\\workspace\\testrec");
				shared_data_server.push_data(key_req1, "path|d:\\workspace\\testrec");
				break;
			case 'n':
				std::cout << "pushed n" << std::endl;
				shared_data_server.push_data(key_req1, "is_runtime|1|0|0|0|0");
				break;
			case 'm':
				std::cout << "pushed m" << std::endl;
				shared_data_server.push_data(key_req1, "is_runtime|0|0|100|0|0");
				break;
			}
			++num_frame;
		}
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << ex.what() << std::endl;
		return;
	}
	shared_data_server.stop();
}



/** @brief Example of shared data
*/
void share_data_sync() {

	// shared data
	co::shm::SharedDataDerivedSample shared_data_server;

	// Set the shared data
	std::string name_shm = "MySharedMemory";
	int width = 640, height = 480;
	// which objects to allocate
	// change to type, params(name, etc...)
	// add to the shared object type and name
	// cmd instruction from clients
	// req requests from server
	std::string cmd =
		"image,rgb," + std::to_string(width) + "," + std::to_string(height) + ",3" +
		"|instruction,cmd0,4096" +
		"|instruction,cmd1,4096" +
		"|instruction,req0,4096" +
		"|instruction,req1,4096";

	std::cout << "[shm::cmd]>>" << cmd << std::endl;
	// parse the command and allocate the memory
	int err = shared_data_server.parse(name_shm, cmd);
	if (err != co::shm::kSharedNoError) {
		std::cout << "shared_data error: " << err << std::endl;
	}
	// bind the function
	shared_data_server.registerCallback(std::bind(&my_func,
		std::placeholders::_1, std::placeholders::_2));

	// Object ID
	size_t key_req0 = shared_data_server.get_key_id("req0");
	if (key_req0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_req0" << std::endl;
		return;
	}
	size_t key_req1 = shared_data_server.get_key_id("req1");
	if (key_req1 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_req1" << std::endl;
		return;
	}
	size_t key_cmd0 = shared_data_server.get_key_id("cmd0");
	if (key_cmd0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_cmd0" << std::endl;
		return;
	}
	size_t key_cmd1 = shared_data_server.get_key_id("cmd1");
	if (key_cmd1 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_cmd1" << std::endl;
		return;
	}

	size_t key_image = shared_data_server.get_key_id("rgb");
	if (key_image == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the image" << std::endl;
		return;
	}

	// create and start the listening process to a specific instruction
	// key
	//shared_data_server.start(kThreadPriorityBackgroundBegin);
	shared_data_server.start(std::vector<size_t>{key_cmd1},
		co::shm::kThreadPriorityBackgroundBegin);

	// test words
	std::vector<std::string> words = { "hello", "0123456", "qwe", "$#!#$%&()", "987643" };

	// main
	try {
		// Get the image information
		int cols = 0, rows = 0, channels = 0;
		shared_data_server.get_image_size("rgb", cols, rows, channels);
		std::cout << "Expected image [" << cols << "," << rows << "," <<
			channels << "]" << std::endl;

		// It allocates the memory for the shared images
		size_t size = 0;
		cv::Mat rgb(rows, cols, CV_8UC3,
			shared_data_server.get_object_ptr(key_image, size));

		// image used to close the program
		cv::Mat m(10, 10, CV_8UC3, cv::Scalar(0, 255));
		bool do_continue = true;
		int num_frame = 0;
		std::cout << "All the instructions are for demonstration only" <<
			std::endl;
		std::cout << "q: quit" << std::endl;
		std::cout << "r: start record" << std::endl;
		std::cout << "s: stop record" << std::endl;
		std::cout << "p: set path" << std::endl;
		std::cout << "n: is runtime on" << std::endl;
		std::cout << "m: is runtime off" << std::endl;
		while (do_continue) {

			cv::imshow("rgb", rgb);
			cv::imshow("server", m);
			char c = cv::waitKey(30);
			//shared_data_server.push_data(key_instruction, 
			//	words[num_frame % words.size()]);
			//shared_data_server.notify();
			switch (c) {
			case 'q':
			case 27:
				do_continue = false;
				break;
			case 'r':
				std::cout << "pushed r" << std::endl;
				shared_data_server.push_data(key_req0, "record|1");
				break;
			case 's':
				std::cout << "pushed s" << std::endl;
				shared_data_server.push_data(key_req0, "record|0");
				break;
			case 'p':
				std::cout << "pushed p" << std::endl;
				shared_data_server.push_data(key_req0, "path|d:\\workspace\\testrec");
				shared_data_server.push_data(key_req1, "path|d:\\workspace\\testrec");
				break;
			case 'n':
				std::cout << "pushed n" << std::endl;
				shared_data_server.push_data(key_req1, "is_runtime|1|0|0|0|0");
				break;
			case 'm':
				std::cout << "pushed m" << std::endl;
				shared_data_server.push_data(key_req1, "is_runtime|0|0|100|0|0");
				break;
			}
			++num_frame;
		}
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << ex.what() << std::endl;
		return;
	}
	shared_data_server.stop();
}



} // namespace anonymous


//-----------------------------------------------------------------------------
void main(int argc, char* argv[])
{
	std::cout << "shm_common_SharedDataDerivedSampleServer" << std::endl;
	//share_data_test();
	share_data_sync();

	return;
}