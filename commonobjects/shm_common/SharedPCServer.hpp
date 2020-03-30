/**
* @file SharedPCServer.hpp
* @brief Class to allocate the memory and structure for the shared memory.
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
* @version 0.3.0.0
*
*/


#ifndef COMMONOBJECTS_SHMCOMMON_SHAREDPCSERVER_HPP__
#define COMMONOBJECTS_SHMCOMMON_SHAREDPCSERVER_HPP__

#include <map>
#include <thread>
#include <mutex>
#include <memory>

#include "doc_managedmemory_shared_data_v2.hpp"
#include "../string_common/StringOp.hpp"

namespace co
{
namespace shm
{

const int kSharedNoError = 0;
const int kSharedKeyExist = 1;
const int kSharedUnableToCreateSharedMemory = 2;

// Invalid ID when a key does not exist.
const int kInvalidKeyID = -1;

// Size of each point in bytes
typedef int SizeOfEachPointBytes;
// Total number of points for an object
typedef int NumberOfPoints;

/** @brief Function for callback
*/
typedef std::function<void(int id, void* data, size_t size)> registration_callback_function;

/** @brief Structure with an observed frame data
*/
struct ObservedObject
{
	std::string serial;
	std::vector<float> points_data;
	int num_points;
};


/** @brief Class to manage a shared data

The shared objects contains the information of 3D cloud points
that are shared among applications.
The maximum number of points and the number of valid points is contained
in the int_vector as follow
[0]: maximum number of points
[1]: valid points
The points are defined as follow
xyz (float) rgb (unsigned char)
*/
class SharedDataServer
{
public:

	SharedDataServer() : c_is_ready_(false), memory_to_allocate_bytes_(0) {}

	~SharedDataServer() {
		stop();
	}

	/**

	expected
	type0,params0|type1,params1|...
	the index of the object is in the order of the construct
	*/
	int parse(
		const std::string &name_shm,
		const std::string &msg,
		bool do_allocate = true) {

		int err = kSharedNoError;

		// copy the name of the shared memory
		name_shm_ = name_shm;

		// total amount of memory to allocate
		memory_to_allocate_bytes_ = 0;
		int memory_buffer = 4096;

		// current object id instantiated
		int object_id = 0;

		// parse the request
		std::vector<std::string> words = co::text::StringOp::split(msg, '|');
		for (auto &it : words) {
			std::vector<std::string> words2 = co::text::StringOp::split(it, ',');
			if (words2.size() >= 1) {
				std::string type = words2[0];

				// It checks if the key does exist. A warning message is 
				// generated if 
				if (key_id_.find(type) != key_id_.end()) {
					err = kSharedKeyExist;
				}

				// function for each type
				if (type == "pcl") {
					int byte4point = std::stoi(words2[1]);
					int num_points = std::stoi(words2[2]);
					// size byte
					int size_byte = num_points * byte4point;
					// total bytes
					memory_to_allocate_bytes_ += size_byte;
					v_.push_back(size_byte);
					if (!do_allocate) {
						// Set the key id
						key_id_.insert(std::make_pair(type, object_id));
						// set the maximum number of points and other information
						smm_.set(object_id, std::vector<int>({ num_points, 0 }));
						// set object name
						smm_.set(object_id, type);
						++object_id;
					}
				} else if (type == "pose_kSize") {
					int byte4point = std::stoi(words2[1]);
					int num_skeletons = std::stoi(words2[2]);
					int num_points_skeleton = std::stoi(words2[3]);
					// size byte
					int size_byte = num_skeletons * num_points_skeleton * byte4point;
					// total bytes
					memory_to_allocate_bytes_ += size_byte;
					v_.push_back(size_byte);
					if (!do_allocate) {
						// Set the key id
						key_id_.insert(std::make_pair(type, object_id));
						// set the maximum number of points and other information
						smm_.set(object_id, std::vector<int>({ 
							num_skeletons * num_points_skeleton, 0, 
							num_points_skeleton }));
						// set object name
						smm_.set(object_id, type);
						++object_id;
					}
				} else if (type == "image") {
					int width = std::stoi(words2[1]);
					int height = std::stoi(words2[2]);
					int channels = std::stoi(words2[3]);
					// Set the camera image
					v_.push_back(width * height * channels); // set the image size
					memory_to_allocate_bytes_ += width * height * channels;
					if (!do_allocate) {
						// Set the key id
						key_id_.insert(std::make_pair(type, object_id));
						// Set the information to share with another process
						std::vector<int> ready_status;
						std::vector<double> value;
						value.clear();
						value.push_back(width);
						value.push_back(height);
						value.push_back(channels);
						value.push_back(0); // timestamp
						value.push_back(0); // frame_id
						ready_status.clear();
						ready_status.push_back(1);
						smm_.push_info(object_id, type, value);
						smm_.push_info(object_id, type, ready_status);
						++object_id;
					}
				} else if (type == "yolo") {
					// yolo object has the xywh in the double_vector and the label
					// in the ptr
					// the labels size is in the int_vector
					int byte_labels = 4096;
					v_.push_back(byte_labels); // set the image size
					memory_to_allocate_bytes_ += byte_labels;
					if (!do_allocate) {
						// Set the key id
						key_id_.insert(std::make_pair(type, object_id));
						// set object name
						smm_.set(object_id, type);
						++object_id;
					}
				}
			}
		}

		// Create the memory space
		if (do_allocate) {
			if (!smm_.create(name_shm_, memory_to_allocate_bytes_ +
				memory_buffer)) {
				std::cout << "Unable to create: " << name_shm_ << std::endl;
				// error
				return kSharedUnableToCreateSharedMemory;
			}
			// Instantiate the objects
			smm_.instantiate(v_);
			// parse the string again to instantiate the parameters
			parse(name_shm, msg, false);
		} else {
			// it creates the mutex and condition variable
			for (size_t i = 0; i < smm_.num_items(); ++i) {
				std::string name = "mtx" + std::to_string(i);
				vmtx_.push_back(
					smm_.find_or_create_mutex(name.c_str())
				);
			}
			for (size_t i = 0; i < smm_.num_items(); ++i) {
				std::string name = "cnd" + std::to_string(i);
				vcnd_.push_back(
					smm_.find_or_create_condition(name.c_str())
				);
			}
		}
		return err;
	}



	/** @brief It creates the memory which will contain the structured data.

	It creates the memory which will contain the structured data.

	@param[in] name_shm Shared memory name
	@param[in] definition Container wit the size of the points
	(3float + 3 uchar), and the number of points
	*/
	[[deprecated("This function is no longer used. Use parse instead")]]
	void create(
		const std::string &name_shm,
		const std::vector<std::pair<SizeOfEachPointBytes, NumberOfPoints>> &definition) {

		// copy the name of the shared memory
		name_shm_ = name_shm;

		// Allocate the container for the points
		size_t kNumSources = definition.size();
		for (auto & it : definition) {
			// xyz rgb
			int size_point = it.first;
			// number of the points
			int num_points = it.second;
			// size byte
			int size_byte = num_points * size_point;
			// total bytes
			memory_to_allocate_bytes_ += size_byte;
			v_.push_back(size_byte);
			information_size_.push_back(std::vector<int>({ num_points, 0 }));
		}
		// Create the memory space
		int memory_buffer = 4096;
		if (!smm_.create(name_shm_, memory_to_allocate_bytes_ +
			memory_buffer)) {
			std::cout << "Unable to create: " << name_shm_ << std::endl;
			return;
		}
		// Instantiate the objects
		smm_.instantiate(v_);
		// Set the information for each shared object
		for (int i = 0; i < smm_.num_items(); ++i) {
			// set the maximum number of points and other information
			smm_.set(i, information_size_[i]);
			// set object name
			smm_.set(i, "pcl");
		}
	}

	/** @brief It detects the memory which will contain the structured data.

	It detects the memory which will contain the structured data.

	@param[in] name_shm Shared memory name
	*/
	bool detect(const std::string &name_shm) {

		// copy the name of the shared memory
		name_shm_ = name_shm;

		if (!smm_.detect(name_shm_)) {
			std::cout << "Unable to detect: " << name_shm << std::endl;
			return false;
		}

		return true;
	}

	/** @brief It starts the worker
	*/
	void start() {
		t_process_ = std::thread(&SharedDataServer::process, this);
	}

	/** @brief It stops a running worker
	*/
	void stop() {
		do_continue_ = false;
		if (t_process_.joinable()) t_process_.join();
	}

	/** @brief If started from the function "start()" it will run in a
	separated thread.
	*/
	void process() {
		// <INFO> if the solution is multithread, this make sense
		//std::vector<interprocess_mutex*> vmtx;
		//for (size_t i = 0; i < smm.num_items(); ++i) {
		//	std::string name = "mtx" + std::to_string(i);
		//	vmtx.push_back(smm.find_or_create_mutex(name));
		//}
		//std::vector<interprocess_condition*> vcnd;
		//for (size_t i = 0; i < smm.num_items(); ++i) {
		//	std::string name = "cnd" + std::to_string(i);
		//	vcnd.push_back(smm.find_or_create_condition(name));
		//}
		//std::vector<scoped_lock<interprocess_mutex>> vlock;
		//for (auto &it : vmtx) {
		//	vlock.push_back(scoped_lock<interprocess_mutex>(*it));
		//}
		// <INFO> for a single thread, this solution is better
		boost::interprocess::interprocess_mutex *mtx =
			smm_.find_or_create_mutex("mtx");
		boost::interprocess::interprocess_condition *cnd =
			smm_.find_or_create_condition("cnd");
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock{ *mtx };
		bool noTimeout = true;

		//Print messages until the other process marks the end
		int num_sources = smm_.num_items();

		// image used to close the program
		do {
			//double start = cv::getTickCount();
			// In normal condition it should be locked
			cnd->notify_all();
			// wait?
			boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(20000);
			noTimeout = cnd->timed_wait(lock, timeout);
			// timeout
			if (!noTimeout)
			{
				std::cout << "timeout!" << std::endl;
				break;
			}

			// Wait that some valid data is given
			std::unique_lock<std::mutex> lk(c_mutex_);
			bool valid_data = true;
			while (!c_is_ready_)
			{
				//c_cv_.wait(lk);
				c_cv_.wait_for(lk, std::chrono::milliseconds(1000));
				if (!c_is_ready_) {
					std::cout << "Spurious wake up!\n";
					valid_data = false;
					break;
				}
			}
			if (!valid_data) continue;
			// it stops the next time that the condition variable is detected
			c_is_ready_ = false;

			//std::cout << "<> notify to server" << std::endl;

			//double stop = cv::getTickCount();
			//double computation_time = (stop - start) / cv::getTickFrequency();
			//std::cout << "ct(ms): " << computation_time << std::endl;
		} while (do_continue_);

		// notify
		cnd->notify_all();
		std::cout << "</SharedDataClient::process>" << std::endl;
	}

	/** @brief It register the callback function
	*/
	void registerCallback(registration_callback_function &&callback) {
		m_callback_ = callback;
	}

	/** @brief It push new data
	*/
	void push_data(int id_obj, void *data, int size, int num_points) {
		std::unique_lock<std::mutex> lk(c_mutex_);
		// copy the data (if any)
		if (data != nullptr) {
			memcpy(smm_.shared_object()[id_obj].ptr(), data, size);
			smm_.shared_object()[id_obj].int_vector_[1] = num_points;
		}
		c_is_ready_ = true;
		c_cv_.notify_one();
	}

	/** brief It push new data
	*/
	void push_data(std::map<int, ObservedObject> &data) {
		//std::unique_lock<std::mutex> lk(c_mutex_);

		std::clock_t c_start = std::clock();
		// your_algorithm
		// transfer the information
		int num_points = 0;
		for (auto &it : data) {
			int id_obj = it.first; // object with the PCL
			memcpy(smm_.shared_object()[id_obj].ptr(), 
				&it.second.points_data[0], 
				sizeof(float) * it.second.points_data.size());
			smm_.shared_object()[id_obj].int_vector_[1] = it.second.num_points;
			// sum points
			num_points += it.second.num_points;
		}
		std::clock_t c_end = std::clock();
		long double time_elapsed_ms = 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC;
		//std::cout << "CPU time used: " << time_elapsed_ms << " ms #" << num_points << "\n";

		//// notify
		//c_is_ready_ = true;
		//c_cv_.notify_one();
	}

	/** @brief It push a source image in the shared memory
	*/
	void push_image(int id_obj, 
		unsigned char *data, size_t data_size,
		int current_time, int num_frame) {
		const int source_id = 0;
		memcpy(smm_.shared_object()[id_obj].ptr(), data, 
			data_size);
		smm_.shared_object()[id_obj].double_vector_[3] = current_time;
		smm_.shared_object()[id_obj].double_vector_[4] = num_frame;
	}

	/** @brief It notifies that the data has been pushed
	*/
	void notify() {
		for (auto &it : vcnd_) it->notify_all();
	}

	/** @brief It returns the index of an associated object key
	*/
	int get_key_id(const std::string &key) {
		if (key_id_.find(key) != key_id_.end()) {
			return key_id_[key];
		} 
		return kInvalidKeyID;
	}

private:

	/** @brief Shared memory
	*/
	SharedMemoryManager smm_;
	std::string name_shm_;

	std::vector<int> v_; // container with the bytes for each object
	std::vector<std::vector<int>> information_size_;
	/** @brief Total allocated memory
	*/
	int memory_to_allocate_bytes_;

	/** @brief If true it continue to process the internal thread
	*/
	bool do_continue_;

	/** @brief Thread of the process
	*/
	std::thread t_process_;

	/** @brief Container with the callback functions
	*/
	registration_callback_function m_callback_;

	/** @brief It guarantee that the passed data is valid
	*/
	std::mutex c_mutex_;
	/** @brief Condition variable
	*/
	std::condition_variable c_cv_;
	/** @brief To avoid spurious wakeup
	*/
	bool c_is_ready_;

	/** @brief Container with the index of the 
	*/
	std::map<std::string, int> key_id_;

	/** @brief Multiple sources mtx and condition variable
	*/
	std::vector<boost::interprocess::interprocess_mutex*> vmtx_;
	std::vector<boost::interprocess::interprocess_condition*> vcnd_;
};

} // namespace shm
} // namespace co

#endif // COMMONOBJECTS_SHMCOMMON_SHAREDPCSERVER_HPP__