/**
* @file SharedDataDerivedSyncSample.hpp
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


#ifndef COMMONOBJECTS_SHMCOMMON_SHAREDDATADERIVEDSYNCSAMPLE_HPP__
#define COMMONOBJECTS_SHMCOMMON_SHAREDDATADERIVEDSYNCSAMPLE_HPP__

#include "SharedDataBase.hpp"

#include <windows.h>

namespace co
{
namespace shm
{

///** @brief Amount of ms to timeout a condition variable
//*/
//const int kSharedDataProcessTimeout = 1000;
//
//// Size of each point in bytes
//typedef int SizeOfEachPointBytes;
//// Total number of points for an object
//typedef int NumberOfPoints;
//
///** @brief Function for callback
//*/
////typedef std::function<void(SharedMemoryManager &smm)> registration_callback_function;
//
///** @brief Structure with an observed frame data
//*/
//struct ObservedObject
//{
//	std::string serial;
//	std::vector<float> points_data;
//	int num_points;
//};



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
class SharedDataDerivedSyncSample : public SharedDataBase
{
public:

	SharedDataDerivedSyncSample() {}

	~SharedDataDerivedSyncSample() {
		stop();
	}

	/** expected
		type0,name0,params0|type1,name1,params1|...
		the index of the object is in the order of the construct

		The function allocates OR initialize a set of objects.
		The standard use of the function is to call without do_allocate 
		parameter (use default true).
		The function estimates the memory to allocate, and a recoursive
		call set the values.
	*/
	int parse(
		const std::string &name_shm,
		const std::string &name_object_shm,
		const std::string &msg,
		bool do_allocate = true) {

		int err = kSharedNoError;

		// copy the name of the shared memory
		name_shm_ = name_shm;

		// total amount of memory to allocate
		memory_to_allocate_bytes_ = 0;
		size_t memory_buffer = 4096;

		// current object id instantiated
		int object_id = 0;

		// Container with the key information already assigned
		std::map<std::string, int> key_id;

		// parse the request
		std::vector<std::string> words = co::text::StringOp::split(msg, '|');
		for (auto &it : words) {
			std::vector<std::string> words2 = co::text::StringOp::split(it, ',');
			if (words2.size() >= 2) {
				std::string type = words2[0];
				std::string name = words2[1];

				// It checks if the key does exist. A warning message is 
				// generated if 
				if (key_id.find(name) != key_id.end()) {
					err = kSharedKeyExist;
				}

				// function for each type
				if (type == "pcl") {
					// i.e. pcl,20,2000
					int byte4point = std::stoi(words2[2]);
					int num_points = std::stoi(words2[3]);
					// size byte
					int size_byte = num_points * byte4point;
					// total bytes
					memory_to_allocate_bytes_ += size_byte;
					v_.push_back(size_byte);
					if (!do_allocate) {
						// Set the key id
						key_id.insert(std::make_pair(type, object_id));
						// set the maximum number of points and other information
						smm_.copyFrom_Veci(object_id, std::vector<int>({ num_points, 0 }));
						// set object type
						smm_.set_object_type(object_id, type);
						// set object name
						smm_.set_object_name(object_id, name);
						++object_id;
					}
				} else if (type == "pose_kSize") {
					// i.e. pose_kSize,20,3,18
					int byte4point = std::stoi(words2[2]);
					int num_skeletons = std::stoi(words2[3]);
					int num_points_skeleton = std::stoi(words2[4]);
					// size byte
					int size_byte = num_skeletons * num_points_skeleton * byte4point;
					// total bytes
					memory_to_allocate_bytes_ += size_byte;
					v_.push_back(size_byte);
					if (!do_allocate) {
						// Set the key id
						key_id.insert(std::make_pair(type, object_id));
						// set the maximum number of points and other information
						smm_.copyFrom_Veci(object_id, std::vector<int>({
							num_skeletons * num_points_skeleton, 0,
							num_points_skeleton }));
						// set object type
						smm_.set_object_type(object_id, type);
						// set object name
						smm_.set_object_name(object_id, name);
						++object_id;
					}
				} else if (type == "image") {
					// i.e. image,640,480,3
					int width = std::stoi(words2[2]);
					int height = std::stoi(words2[3]);
					int channels = std::stoi(words2[4]);
					// Set the camera image
					v_.push_back(width * height * channels); // set the image size
					memory_to_allocate_bytes_ += width * height * channels;
					if (!do_allocate) {
						// Set the key id
						key_id.insert(std::make_pair(type, object_id));
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
						smm_.copyFrom(object_id, "image_info", value);
						smm_.copyFrom(object_id, "ready_status", ready_status);
						// set object type
						smm_.set_object_type(object_id, type);
						// set object name
						smm_.set_object_name(object_id, name);
						++object_id;
					}
				} else if (type == "yolo") {
					// i.e. yolo
					// yolo object has the xywh in the double_vector and the label
					// in the ptr
					// the labels size is in the int_vector
					int byte_labels = 4096;
					v_.push_back(byte_labels); // set the message size
					memory_to_allocate_bytes_ += byte_labels;
					if (!do_allocate) {
						// Set the key id
						key_id.insert(std::make_pair(type, object_id));
						// set object type
						smm_.set_object_type(object_id, type);
						// set object name
						smm_.set_object_name(object_id, name);
						++object_id;
					}
				}
				else if (type == "instruction") {
					// i.e. instruction,2048
					// the instruction size
					int bytes = std::stoi(words2[2]);
					v_.push_back(0); // set the image size (no use raw memory, but need the object instantiation)
					memory_to_allocate_bytes_ += bytes;
					if (!do_allocate) {
						// Set the key id
						key_id.insert(std::make_pair(type, object_id));
						// set object type
						smm_.set_object_type(object_id, type);
						// set object name
						smm_.set_object_name(object_id, name);
						++object_id;
					}
				}
			}
		}

		// Create the memory space
		if (do_allocate) {
			// create the memory (allocate the necessary space)
			if (!smm_.create(name_shm_, memory_to_allocate_bytes_ +
				memory_buffer)) {
				std::cout << "Unable to create: " << name_shm_ << std::endl;
				// error
				return kSharedUnableToCreateSharedMemory;
			}
			// Instantiate the objects
			smm_.instantiate(name_object_shm, v_);
			// parse the string again to instantiate the parameters
			parse(name_shm, name_object_shm, msg, false);
		} else {
			// It creates the mutex and condition variable for whole process
			global_mtx_ =
				smm_.find_or_create_mutex("global_mtx");
			global_cnd_ =
				smm_.find_or_create_condition("global_cnd");

			// it creates the mutex and condition variable
			for (size_t i = 0; i < smm_.num_items(); ++i) {
				std::string name = "obj_mtx" + std::to_string(i);
				v_obj_mtx_.push_back(
					smm_.find_or_create_mutex(name.c_str())
				);
			}
			for (size_t i = 0; i < smm_.num_items(); ++i) {
				std::string name = "obj_cnd" + std::to_string(i);
				v_obj_cnd_.push_back(
					smm_.find_or_create_condition(name.c_str())
				);
			}
		}
		return err;
	}


	/** @brief It detects the memory which will contain the structured data.

	It detects the memory which will contain the structured data.

	@param[in] name_shm Shared memory name
	*/
	bool detect(
		const std::string &name_shm,
		const std::string &name_object_shm) {

		// copy the name of the shared memory
		name_shm_ = name_shm;

		if (!smm_.detect(name_shm_, name_object_shm)) {
			std::cout << "Unable to detect: " << name_shm << std::endl;
			return false;
		}

		// It creates the mutex and condition variable for whole process
		global_mtx_ =
			smm_.find_or_create_mutex("global_mtx");
		global_cnd_ =
			smm_.find_or_create_condition("global_cnd");

		// It creates the mutex and condition variable for the existing
		// objects
		for (size_t i = 0; i < smm_.num_items(); ++i) {
			std::string name = "obj_mtx" + std::to_string(i);
			v_obj_mtx_.push_back(
				smm_.find_or_create_mutex(name.c_str())
			);
		}
		for (size_t i = 0; i < smm_.num_items(); ++i) {
			std::string name = "obj_cnd" + std::to_string(i);
			v_obj_cnd_.push_back(
				smm_.find_or_create_condition(name.c_str())
			);
		}

		return true;
	}


	/** @brief If started from the function "start()" it will run in a
			   separated thread.
	*/
	void process(int thread_priority) {
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock{ *global_mtx_ };
		bool noTimeout = true;

		//Print messages until the other process marks the end
		size_t num_sources = smm_.num_items();

		switch (thread_priority) {
		case kThreadPriorityNone:
			break;
		case kThreadPriorityBackgroundBegin:
			// Increase the thread priority
			SetThreadPriority(GetCurrentThread(),
				THREAD_MODE_BACKGROUND_BEGIN);
			break;
		}

		// image used to close the program
		bool valid_data = false;
		do_continue_ = true;
		do {
			//double start = cv::getTickCount();
			// In normal condition it should be locked
			global_cnd_->notify_all();

			//std::cout << "<> notify to server" << std::endl;
			// callback here
			if (f_callback_ != nullptr && valid_data) {
				f_callback_(kInvalidKeyID, smm_);
			}
			// invalidate the data
			valid_data = false;

			// wait?
			boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(kSharedDataProcessTimeout);
			noTimeout = global_cnd_->timed_wait(lock, timeout);
			// timeout
			if (!noTimeout)
			{
				//std::cout << "timeout!" << std::endl;
				valid_data = false;
				//break;
			} else {
				valid_data = true;
			}

			//// Wait that some valid data is given
			//std::unique_lock<std::mutex> lk(c_mutex_);
			//while (!c_is_ready_)
			//{
			//	//c_cv_.wait(lk);
			//	c_cv_.wait_for(lk, std::chrono::milliseconds(kSharedDataProcessTimeout));
			//	if (!c_is_ready_) {
			//		std::cout << "Spurious wake up!\n";
			//		break;
			//	}
			//}
			//// it stops the next time that the condition variable is detected
			//c_is_ready_ = false;

			//if (!valid_data) continue;

			//double stop = cv::getTickCount();
			//double computation_time = (stop - start) / cv::getTickFrequency();
			//std::cout << "ct(ms): " << computation_time << std::endl;			std::cout << "THERE" << std::endl;
		} while (do_continue_);

		// notify
		global_cnd_->notify_all();
		std::cout << "</SharedDataClient::process>" << std::endl;
	}



	/** @brief It process a specific object as separate thread and callback 
	           when event occurrs.
	*/
	void process_id(int thread_priority, size_t object_id) {
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock{ *v_obj_mtx_[object_id] };
		bool noTimeout = true;

		//Print messages until the other process marks the end
		size_t num_sources = smm_.num_items();

		switch (thread_priority) {
		case kThreadPriorityNone:
			break;
		case kThreadPriorityBackgroundBegin:
			// Increase the thread priority
			SetThreadPriority(GetCurrentThread(),
				THREAD_MODE_BACKGROUND_BEGIN);
			break;
		}

		// image used to close the program
		bool valid_data = false;
		do_continue_ = true;
		do {
			//double start = cv::getTickCount();
			// In normal condition it should be locked
			v_obj_cnd_[object_id]->notify_all();

			// wait?
			boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(kSharedDataProcessTimeout);
			noTimeout = v_obj_cnd_[object_id]->timed_wait(lock, timeout);
			// timeout
			if (!noTimeout)
			{
				//std::cout << "timeout!" << std::endl;
				valid_data = false;
				//break;
			}
			else {
				valid_data = true;
			}

			//std::cout << "<> notify to server" << std::endl;
			// callback here
			if (f_callback_ != nullptr && valid_data) {
				f_callback_(object_id, smm_);
			}
			// invalidate the data
			valid_data = false;

			//// Wait that some valid data is given
			//std::unique_lock<std::mutex> lk(c_mutex_);
			//while (!c_is_ready_)
			//{
			//	//c_cv_.wait(lk);
			//	c_cv_.wait_for(lk, std::chrono::milliseconds(kSharedDataProcessTimeout));
			//	if (!c_is_ready_) {
			//		std::cout << "Spurious wake up!\n";
			//		break;
			//	}
			//}
			//// it stops the next time that the condition variable is detected
			//c_is_ready_ = false;

			//if (!valid_data) continue;

			//double stop = cv::getTickCount();
			//double computation_time = (stop - start) / cv::getTickFrequency();
			//std::cout << "ct(ms): " << computation_time << std::endl;
		} while (do_continue_);

		// notify
		v_obj_cnd_[object_id]->notify_all();
		std::cout << "</SharedDataClient::process>" << std::endl;
	}


	/** @brief It push new data

		It pushes a new data.
		@param[in] obj_name The name of the object.
		@param[in] msg The value to set.
	*/
	void push_data_byname(const std::string &obj_name, const std::string &msg) {
		size_t id_obj = get_key_id(obj_name);
		if (id_obj != kInvalidKeyID &&
			id_obj >= 0 && id_obj < smm_.num_items()) {
			// update
			smm_.set_string(id_obj, msg);
			// notify
			v_obj_cnd_[id_obj]->notify_all();
			//std::unique_lock<std::mutex> lk(c_mutex_);
			//smm_.set(id_obj, msg);
			//c_is_ready_ = true;
			//c_cv_.notify_one();
		}
	}


	/** @brief It push new data
	*/
	void push_data_byid(int id_obj, const std::string &msg) {
		if (id_obj >= 0 && id_obj < smm_.num_items()) {
			// update
			smm_.set_string(id_obj, msg);
			// notify
			v_obj_cnd_[id_obj]->notify_all();
			//std::unique_lock<std::mutex> lk(c_mutex_);
			//smm_.set(id_obj, msg);
			//c_is_ready_ = true;
			//c_cv_.notify_one();
		}
	}

	/** @brief It push new data
	*/
	void push_data(std::map<int, ObservedObject> &data) {
		//int num_points = 0;
		//std::cout << "#data: " << data.size() << std::endl;
		for (auto &it : data) {
			int id_obj = it.first; // object with the PCL
			size_t max_size = 0;
			auto ptr = smm_.get_ptr(id_obj, max_size);
			//std::cout << "itdata2: " << it.first << " ptr: " << 
			//	ptr << " " << max_size << std::endl;

			// the object does exist, thrtr id data to transfer, and it is not
			// too large.
			if (ptr != nullptr &&
				it.second.points_data.size() > 0 &&
				sizeof(float) * it.second.points_data.size() < max_size) {
				
				memcpy(ptr,&it.second.points_data[0],
					sizeof(float) * it.second.points_data.size());
				auto obj = smm_.shared_object(id_obj);
				//std::cout << "obj: " << obj << std::endl;
				if (obj != nullptr) {
					obj->int_vector_[1] = it.second.num_points;
					// sum points
					//num_points += it.second.num_points;
					//std::cout << "Obj: " << it.first << " " << id_obj << " " << obj->int_vector_[1] << " " << it.second.num_points << std::endl;
				}
			}
		}
	}


	/** @brief It gets the size of an associated image
	*/
	bool get_image_size(const std::string &object_name, 
		int &cols, int &rows, int &channels) {
		std::vector<double> values;
		if (get_object_Vecd(object_name, values)) {
			cols = static_cast<int>(values[0]);
			rows = static_cast<int>(values[1]);
			channels = static_cast<int>(values[2]);
			return true;
		}
		return false;
	}


	/** @brief It initializes the mutex and condition variable
	*/
	void initialize_wait(const std::string &who_mtx, const std::string &who_cnd, int timeout_ms) {

		// It creates the mutex and condition variable for whole process
		if (sync_mtx_.find(who_mtx) == sync_mtx_.end()) {
			sync_mtx_[who_mtx] = smm_.find_or_create_mutex(who_mtx);
			sync_timeout_ms_[who_mtx] = timeout_ms;
			if (timeout_ms > 0) {
				sync_lock_[who_mtx] = boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>{ *(sync_mtx_[who_mtx]) };
			}
		}

		if (sync_cnd_.find(who_cnd) == sync_cnd_.end()) {
			sync_cnd_[who_cnd] = smm_.find_or_create_condition(who_cnd);
		}
	}

	/** @return It return true in case of no timeout (notify received). False
	            otherwise.
	*/
	bool wait_timeout(const std::string &who_mtx, const std::string &who_cnd, int timeout_ms) {
		// wait?
		boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(timeout_ms);
		return sync_cnd_[who_cnd]->timed_wait(sync_lock_[who_mtx], timeout);
	}

	bool wait_no_timeout(const std::string &who_mtx, const std::string &who_cnd) {
		boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(*sync_mtx_[who_mtx]);
		sync_cnd_[who_cnd]->wait(lock);

		return true;
	}


	bool wait(const std::string &who_mtx, const std::string &who_cnd) {

		if (sync_lock_.find(who_mtx) != sync_lock_.end() &&
			sync_timeout_ms_[who_mtx] > 0) {
			return wait_timeout(who_mtx, who_cnd, sync_timeout_ms_[who_mtx]);
		} else {
			return wait_no_timeout(who_mtx, who_cnd);
		}

		return false;

	}

	void notify_all(const std::string &who) {
		if (sync_cnd_.find(who) != sync_cnd_.end()) {
			sync_cnd_[who]->notify_all();
		}
	}

	void notify_one(const std::string &who) {
		if (sync_cnd_.find(who) != sync_cnd_.end()) {
			sync_cnd_[who]->notify_one();
		}
	}


private:

	std::map<std::string, boost::interprocess::interprocess_mutex*> sync_mtx_;
	std::map<std::string, boost::interprocess::interprocess_condition*> sync_cnd_;
	std::map<std::string, int> sync_timeout_ms_;
	std::map<std::string, boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex>> sync_lock_;
};

} // namespace shm
} // namespace co

#endif // COMMONOBJECTS_SHMCOMMON_SHAREDDATADERIVEDSYNCSAMPLE_HPP__