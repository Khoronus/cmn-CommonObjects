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
#include "commonobjects/shm_common/SharedDataDerivedSyncSample.hpp"
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
		parse(smm.get_string("req0"));
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
	if (!shared_data_client.detect(name_shm, "SharedObject")) {
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


/** @brief Class to elaborate the received callbacks
*/
class CallbackElaborationSync
{
public:

	CallbackElaborationSync() : do_record_(false) { num_frame = 0; }

	void set_is_ready_to_write(bool is_ready_to_write) {
		is_ready_to_write_ = is_ready_to_write;
	}
	bool is_ready_to_write() {
		return is_ready_to_write_;
	}

	/** @brief Callback functions
	*/
	void my_func(int object_id, co::shm::SharedMemoryManager &smm) {
		std::cout << "client_side: received:" << object_id << std::endl;
		std::string msg;
		msg = smm.object_name(object_id);
		std::cout << "client_side: smm object name:" << msg << std::endl;
		msg = smm.get_string(object_id);
		std::cout << "msg: " << msg << std::endl;
		//smm.set_string(key_cmd0, "data_is_ready|" + std::to_string(num_frame));

		is_ready_to_write_ = true;
		//shared_data_client->push_data_byid(key_cmd0, "data_is_ready|" + std::to_string(num_frame));
		++num_frame;
		//c_cv_->notify_all();
		//*is_valid = true;
		// parse the command
		//parse(smm.get_string("req0"));
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

	co::shm::SharedDataDerivedSample *shared_data_client;
	size_t key_cmd0;

	//std::condition_variable *c_cv_;
	bool *is_valid;

	int num_frame = 0;

private:

	bool is_ready_to_write_;

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
			}
			else if (words[0] == "path" && words.size() > 1) {
				path_where_to_save_ = words[1];
			}
		}
	}
};


/** @brief Test the client
*/
int test_client_sycn() {
	co::shm::SharedDataDerivedSample shared_data_client;
	std::string name_shm = "MySharedMemory";
	std::string name_shared_object = "SharedObject";
	if (!shared_data_client.detect(name_shm, name_shared_object)) {
		std::cout << "Unable to detect: " << name_shm << std::endl;
		return 1;
	}
	CallbackElaborationSync callback_elaboration;
	// bind the function
	shared_data_client.registerCallback(std::bind(&CallbackElaborationSync::my_func,
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
	size_t key_cmd0 = shared_data_client.get_key_id("cmd0");
	if (key_cmd0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_cmd0" << std::endl;
		return 1;
	}

	// start the listening process
	shared_data_client.start(std::vector<size_t>{key_req0},
		co::shm::kThreadPriorityBackgroundBegin);
	//shared_data_client.start(co::shm::kThreadPriorityBackgroundBegin);

	try {

		///** @brief It guarantee that the passed data is valid
		//*/
		//std::mutex c_mutex_;
		///** @brief Condition variable
		//*/
		//std::condition_variable c_cv_;
		///** @brief To avoid spurious wakeup
		//*/
		//bool c_is_ready_ = true;
		//bool is_valid = true;


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
		int kSharedDataProcessTimeout = 1000;

		//callback_elaboration.c_cv_ = &c_cv_;
		//callback_elaboration.is_valid = &is_valid;
		callback_elaboration.shared_data_client = &shared_data_client;
		callback_elaboration.key_cmd0 = key_cmd0;

		//shared_data_client.push_data_byid(key_cmd0, "data_is_ready|-1");
		std::cout << "input a key to quit" << std::endl;
		//int val = 0;
		//std::cin >> val;

		callback_elaboration.set_is_ready_to_write(true);


		bool end_loop = false;
		int num_frame = 0;
		do {
			//// Wait that some valid data is given
			//std::unique_lock<std::mutex> lk(c_mutex_);
			//while (!c_is_ready_)
			//{
			//	c_cv_.wait_for(lk, std::chrono::milliseconds(kSharedDataProcessTimeout));
			//	if (!c_is_ready_) {
			//		std::cout << "Spurious wake up!\n";
			//		break;
			//	}
			//}
			//// it stops the next time that the condition variable is detected
			//c_is_ready_ = false;
			//std::cout << "here: " << is_valid << std::endl;

			cv::Mat tmp;
			vc >> tmp;
			cv::imshow("client", tmp);
			if (cv::waitKey(10) == 27) end_loop = true;

			if (callback_elaboration.is_ready_to_write()) {

				// Read the result of the detection
				std::vector<int> vint;
				std::vector<double> vdouble;
				shared_data_client.get_object_Veci("objdet", vint);
				shared_data_client.get_object_Vecd("objdet", vdouble);
				std::cout << "int>> ";
				for (auto &it : vint) {
					std::cout << it << " ";
				}
				std::cout << std::endl;
				std::cout << "double>> ";
				for (auto &it : vdouble) {
					std::cout << it << " ";
				}
				std::cout << std::endl;


				memcpy(m.data, tmp.data, m.cols * m.rows * m.channels());
				callback_elaboration.set_is_ready_to_write(false);
				++num_frame;
			}
			shared_data_client.push_data_byid(key_cmd0, "data_is_ready|" + std::to_string(num_frame));
		} while (!end_loop);
	}
	catch (boost::interprocess::interprocess_exception &ex) {
		std::cout << ex.what() << std::endl;
		return 1;
	}

	shared_data_client.stop();
	return 0;
}



/** @brief Class to elaborate the received callbacks
*/
class CallbackElaborationSyncGlob
{
public:

	CallbackElaborationSyncGlob() : do_record_(false) { num_frame = 0; }

	void set_is_ready_to_write(bool is_ready_to_write) {
		is_ready_to_write_ = is_ready_to_write;
	}
	bool is_ready_to_write() {
		return is_ready_to_write_;
	}

	/** @brief Callback functions
	*/
	void my_func(int object_id, co::shm::SharedMemoryManager &smm) {
		std::cout << "client_side: received:" << object_id << std::endl;
		std::string msg;
		msg = smm.object_name(object_id);
		std::cout << "client_side: smm object name:" << msg << std::endl;
		msg = smm.get_string(object_id);
		std::cout << "msg: " << msg << std::endl;
		//smm.set_string(key_cmd0, "data_is_ready|" + std::to_string(num_frame));

		is_ready_to_write_ = true;
		//shared_data_client->push_data_byid(key_cmd0, "data_is_ready|" + std::to_string(num_frame));
		++num_frame;
		//c_cv_->notify_all();
		//*is_valid = true;
		// parse the command
		//parse(smm.get_string("req0"));
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

	co::shm::SharedDataDerivedSyncSample *shared_data_client;
	size_t key_cmd0;

	//std::condition_variable *c_cv_;
	bool *is_valid;

	int num_frame = 0;

private:

	bool is_ready_to_write_;

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
			}
			else if (words[0] == "path" && words.size() > 1) {
				path_where_to_save_ = words[1];
			}
		}
	}
};


/** @brief Test the client
*/
int test_client_sycn_glob() {
	co::shm::SharedDataDerivedSyncSample shared_data_client;
	std::string name_shm = "MySharedMemory";
	std::string name_shared_object = "SharedObject";
	if (!shared_data_client.detect(name_shm, name_shared_object)) {
		std::cout << "Unable to detect: " << name_shm << std::endl;
		return 1;
	}
	CallbackElaborationSyncGlob callback_elaboration;
	// bind the function
	shared_data_client.registerCallback(std::bind(&CallbackElaborationSyncGlob::my_func,
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
	size_t key_cmd0 = shared_data_client.get_key_id("cmd0");
	if (key_cmd0 == co::shm::kInvalidKeyID) {
		std::cout << "[e] Shared memory has memory for the key_cmd0" << std::endl;
		return 1;
	}

	// start the listening process
	shared_data_client.start(std::vector<size_t>{key_req0},
		co::shm::kThreadPriorityBackgroundBegin);
	//shared_data_client.start(co::shm::kThreadPriorityBackgroundBegin);

	try {

		///** @brief It guarantee that the passed data is valid
		//*/
		//std::mutex c_mutex_;
		///** @brief Condition variable
		//*/
		//std::condition_variable c_cv_;
		///** @brief To avoid spurious wakeup
		//*/
		//bool c_is_ready_ = true;
		//bool is_valid = true;


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
		int kSharedDataProcessTimeout = 1000;

		//callback_elaboration.c_cv_ = &c_cv_;
		//callback_elaboration.is_valid = &is_valid;
		callback_elaboration.shared_data_client = &shared_data_client;
		callback_elaboration.key_cmd0 = key_cmd0;

		//shared_data_client.push_data_byid(key_cmd0, "data_is_ready|-1");
		std::cout << "input a key to quit" << std::endl;
		//int val = 0;
		//std::cin >> val;

		callback_elaboration.set_is_ready_to_write(true);

		// Find all the mutex and condition variable
		shared_data_client.initialize_wait("glob_mtx", "glob_cnd_srv", 100);
		shared_data_client.initialize_wait("glob_mtx", "glob_cnd_cln", 100);

		// notify
		shared_data_client.notify_all("glob_cnd_srv");

		bool end_loop = false;
		int num_frame = 0;

		long long duration_tot = 0;
		int duration_entries = 0;
		do {

			auto t1 = std::chrono::high_resolution_clock::now();

			//// Wait that some valid data is given
			//std::unique_lock<std::mutex> lk(c_mutex_);
			//while (!c_is_ready_)
			//{
			//	c_cv_.wait_for(lk, std::chrono::milliseconds(kSharedDataProcessTimeout));
			//	if (!c_is_ready_) {
			//		std::cout << "Spurious wake up!\n";
			//		break;
			//	}
			//}
			//// it stops the next time that the condition variable is detected
			//c_is_ready_ = false;
			//std::cout << "here: " << is_valid << std::endl;

			cv::Mat tmp;
			vc >> tmp;
			cv::imshow("client", tmp);
			if (cv::waitKey(1) == 27) end_loop = true;

			// wait for the connection
			if (!shared_data_client.wait("glob_mtx", "glob_cnd_cln")) {
				std::cout << "Timeout" << std::endl;
			} else {
				//std::cout << "num_frame: " << num_frame << std::endl;
			}

			++num_frame;
			// notify
			shared_data_client.notify_all("glob_cnd_srv");


			//if (callback_elaboration.is_ready_to_write()) {

			//	// Read the result of the detection
			//	std::vector<int> vint;
			//	std::vector<double> vdouble;
			//	shared_data_client.get_object_Veci("objdet", vint);
			//	shared_data_client.get_object_Vecd("objdet", vdouble);
			//	std::cout << "int>> ";
			//	for (auto &it : vint) {
			//		std::cout << it << " ";
			//	}
			//	std::cout << std::endl;
			//	std::cout << "double>> ";
			//	for (auto &it : vdouble) {
			//		std::cout << it << " ";
			//	}
			//	std::cout << std::endl;


			//	memcpy(m.data, tmp.data, m.cols * m.rows * m.channels());
			//	callback_elaboration.set_is_ready_to_write(false);
			//	++num_frame;
			//}
			//shared_data_client.push_data_byid(key_cmd0, "data_is_ready|" + std::to_string(num_frame));


			auto t2 = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
			duration_tot += duration;
			duration_entries += 1;
			if (duration_entries % 30 == 0) {
				std::cout << duration_tot / duration_entries << std::endl;

				duration_tot = 0;
				duration_entries = 0;

				duration_tot += duration;
				duration_entries += 1;
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
	//test_client_sycn();

	test_client_sycn_glob();
}