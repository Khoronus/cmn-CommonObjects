/**
* @file shm_common_SharedDataDerivedSampleClient.cpp
* @brief Example of point cloud client
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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <opencv2/opencv.hpp>

#include "commonobjects/shm_common/SharedDataDerivedSample.hpp"
#include "commonobjects/string_common/StringOp.hpp"


/** @brief Class to elaborate the received callbacks
*/
class CallbackElaboration
{
public:

	CallbackElaboration() : do_record_(false) {}

	/** @brief Callback functions
	*/
	void my_func(int object_id, co::shm::SharedMemoryManager &smm) {
		// parse the command
		parse(smm.get("req0"));
		//std::cout << "clientside: received" << std::endl;

		//std::cout << "msg: " << smm.get("cmd0") << std::endl;
		////std::cout << "Received[" << id << "]: " << size << " bytes" << std::endl;
		////cv::Mat m(480, 640, CV_8UC3, data);
		////cv::imshow("mserver" + std::to_string(id), m);
		////cv::waitKey(1);
		//if (smm.get("cmd0") == "start_record") {
		//	do_record_ = true;
		//} else if (smm.get("cmd0") == "stop_record") {
		//	do_record_ = false;
		//}
	}

	/** @brief Do record?
	*/
	bool do_record() {
		return do_record_;
	}

	std::string path_where_to_save() {
		return path_where_to_save_;
	}

private:

	/** @brief Set by the callback, it informs if it is necessary to record.
	*/
	bool do_record_;

	std::string path_where_to_save_;

	/** @brief It parses the command received
	*/
	void parse(const std::string &msg) {
		std::vector<std::string> words = co::text::StringOp::split(msg, '|');
		if (words.size() > 0) {
			if (words[0] == "record" && words.size() > 1) {
				do_record_ = std::stoi(words[1]);
			} else if (words[0] == "path" && words.size() > 1) {
				path_where_to_save_ = words[1];
			}
		}
	}
};

/** @brief Test the client
*/
int test_client() {
	co::shm::SharedDataDerivedSample shared_data_client;
	std::string name_shm = "MySharedMemory";
	if (!shared_data_client.detect(name_shm)) {
		std::cout << "Unable to detect: " << name_shm << std::endl;
		return 1;
	}
	CallbackElaboration callback_elaboration;
	// bind the function
	shared_data_client.registerCallback(std::bind(&CallbackElaboration::my_func,
		std::ref(callback_elaboration), std::placeholders::_1, std::placeholders::_2));

	// Get the object IDs
	size_t key_image = shared_data_client.get_key_id("rgb");
	if (key_image == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the image" << std::endl;
		return 1;
	}
	size_t key_req0 = shared_data_client.get_key_id("req0");
	if (key_req0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_instruction" << std::endl;
		return 1;
	}

	// start the listening process
	shared_data_client.start(std::vector<size_t>{key_req0},
		co::shm::kThreadPriorityBackgroundBegin);

	try {
		cv::VideoCapture vc(0);
		if (!vc.isOpened()) return 1;

		// Get the image information
		int cols = 0, rows = 0, channels = 0;
		shared_data_client.get_image_size("rgb", cols, rows, channels);
		std::cout << "Expected image [" << cols << "," << rows << "," <<
			channels << "]" << std::endl;

		// image used to close the program
		size_t size = 0;
		cv::Mat m(rows, cols, CV_8UC3,
			shared_data_client.get_object_ptr(key_image, size));
		std::cout << "size: " << size << std::endl;
		bool end_loop = false;
		do {
			vc >> m;
			cv::imshow("client", m);
			if (cv::waitKey(10) == 27) end_loop = true;

			if (callback_elaboration.do_record()) {
				std::cout << "record_data" << std::endl;
				std::cout << "path: " << callback_elaboration.path_where_to_save() << std::endl;;
			}
		} while (!end_loop);
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << ex.what() << std::endl;
		return 1;
	}

	shared_data_client.stop();
	return 0;
}


/** @brief Test the client
*/
int test_client_sycn() {
	co::shm::SharedDataDerivedSample shared_data_client;
	std::string name_shm = "MySharedMemory";
	if (!shared_data_client.detect(name_shm)) {
		std::cout << "Unable to detect: " << name_shm << std::endl;
		return 1;
	}
	CallbackElaboration callback_elaboration;
	// bind the function
	shared_data_client.registerCallback(std::bind(&CallbackElaboration::my_func,
		std::ref(callback_elaboration), std::placeholders::_1, std::placeholders::_2));

	// Get the object IDs
	size_t key_image = shared_data_client.get_key_id("rgb");
	if (key_image == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the image" << std::endl;
		return 1;
	}
	size_t key_req0 = shared_data_client.get_key_id("req0");
	if (key_req0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_instruction" << std::endl;
		return 1;
	}

	// start the listening process
	shared_data_client.start(std::vector<size_t>{key_req0},
		co::shm::kThreadPriorityBackgroundBegin);

	try {
		cv::VideoCapture vc(0);
		if (!vc.isOpened()) return 1;

		// Get the image information
		int cols = 0, rows = 0, channels = 0;
		shared_data_client.get_image_size("rgb", cols, rows, channels);
		std::cout << "Expected image [" << cols << "," << rows << "," <<
			channels << "]" << std::endl;

		// image used to close the program
		size_t size = 0;
		cv::Mat m(rows, cols, CV_8UC3,
			shared_data_client.get_object_ptr(key_image, size));
		std::cout << "size: " << size << std::endl;
		bool end_loop = false;
		do {
			vc >> m;
			cv::imshow("client", m);
			if (cv::waitKey(10) == 27) end_loop = true;

			if (callback_elaboration.do_record()) {
				std::cout << "record_data" << std::endl;
				std::cout << "path: " << callback_elaboration.path_where_to_save() << std::endl;;
			}
		} while (!end_loop);
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << ex.what() << std::endl;
		return 1;
	}

	shared_data_client.stop();
	return 0;
}

//-----------------------------------------------------------------------------
int main()
{
	//test_client();
	test_client_sycn();
}