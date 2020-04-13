/**
* @file SharedDataBase.hpp
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
* @author Alessandro Moro (alessandromoro.italy@gmail.com)
* @bug No known bugs.
* @version 0.4.0.0
*
*/

#ifndef COMMONOBJECTS_SHMCOMMON_SHAREDDATABASE_HPP__
#define COMMONOBJECTS_SHMCOMMON_SHAREDDATABASE_HPP__

#include <mutex>
#include <condition_variable>
#include <map>

#include "doc_managedmemory_shared_data_base.hpp"
#include "../string_common/StringOp.hpp"

namespace co
{
namespace shm
{

// Set the thread priority mode
const int kThreadPriorityNone = 0;
const int kThreadPriorityLowest = 1;
const int kThreadPriorityBelowNormal = 2;
const int kThreadPriorityNormal = 3;
const int kThreadPriorityHighest = 4;
const int kThreadPriorityAboveNormal = 5;
const int kThreadPriorityTimeCritical = 6;
const int kThreadPriorityIdle = 7;
const int kThreadPriorityBackgroundBegin = 8;
const int kThreadPriorityBackgroundEnd = 9;

const int kSharedNoError = 0;
const int kSharedKeyExist = 1;
const int kSharedUnableToCreateSharedMemory = 2;

// Invalid ID when a key does not exist.
const size_t kInvalidKeyID = -1;

/** @brief Function for callback
*/
typedef std::function<void(int id, SharedMemoryManager &smm)> registration_callback_function_shared;

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
class SharedDataBase
{
public:

	SharedDataBase() : /*c_is_ready_(false), */memory_to_allocate_bytes_(0) {}

	~SharedDataBase() {
		stop();
	}

	/** expected
	    type0,params0|type1,params1|...
	    the index of the object is in the order of the construct
	*/
	virtual int parse(
		const std::string &name_shm,
		const std::string &name_object_shm,
		const std::string &msg,
		bool do_allocate = true) = 0;

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
		mtx_ =
			smm_.find_or_create_mutex("mtx");
		cnd_ =
			smm_.find_or_create_condition("cnd");

		// It creates the mutex and condition variable for the existing
		// objects
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

		return true;
	}

	/** @brief It starts the worker
	*/
	void start(int thread_priority) {
		t_process_ = std::thread(&SharedDataBase::process, this, thread_priority);
	}

	/** @brief It starts a set of workers to listen for a specific process
	*/
	void start(std::vector<size_t> &which, int thread_priority) {
		for (auto &it : which) {
			container_t_process_.push_back(
				std::thread(&SharedDataBase::process_id, this,
					thread_priority, it));
		}
	}

	/** @brief It stops a running worker
	*/
	void stop() {
		do_continue_ = false;
		if (t_process_.joinable()) t_process_.join();
		for (auto &it : container_t_process_) {
			if (it.joinable()) it.join();
		}
	}

	/** @brief If started from the function "start()" it will run in a
		separated thread. 
	
		It will run in a separated process for a generic operation.
		An implementation may call the callback without specific ID.
	*/
	virtual void process(int thread_priority) = 0;

	/** @brief If started from the function "start()" it will run in a
	separated thread.

		It launch a thread for a specific object
	*/
	virtual void process_id(int thread_priority, size_t object_id) = 0;

	/** @brief It register the callback function
	*/
	void registerCallback(registration_callback_function_shared &&callback) {
		m_callback_ = callback;
	}

	/** @brief It returns the index of an associated object key
	*/
	size_t get_key_id(const std::string &key) {
		for (size_t i = 0; i < smm_.num_items(); ++i) {
			if (smm_.shared_object(i) != nullptr) {
				std::string s(smm_.shared_object(i)->object_name_.begin(),
					smm_.shared_object(i)->object_name_.end());
				if (s == key) {
					return i;
				}
			}
		}
		//if (key_id_.find(key) != key_id_.end()) {
		//	return key_id_[key];
		//} 
		return kInvalidKeyID;
	}

	/** @brief It returns the pointer to the object associated
	*/
	void* get_object_ptr(size_t id_obj, size_t &size) {
		auto ptr = smm_.get_ptr(id_obj, size);
		if (ptr != nullptr) {
			return ptr;
		}
		size = 0;
		return nullptr;
	}

	/** @brief It notifies that the data has been pushed (process)
	*/
	void notify() {
		cnd_->notify_all();
	//	std::unique_lock<std::mutex> lk(c_mutex_);
	//	c_is_ready_ = true;
	//	c_cv_.notify_one();
	}

	/** @brief Notify for the objects allocated
	*/
	void notify_objects() {
		for (auto &it : vcnd_) it->notify_all();
	}

	/** @brief Notify for a single object allocated
	*/
	void notify_object(size_t id_obj) {
		if (id_obj >= 0 && id_obj < vcnd_.size()) {
			vcnd_[id_obj]->notify_all();
		}
	}

	/** @brief It push a source image in the shared memory

		@previous_name get_object_values
	*/
	bool get_object_Vecd(
		const std::string &name,
		std::vector<double> &values) {
		size_t key_id = get_key_id(name);
		if (key_id != kInvalidKeyID) {
			for (auto &it : smm_.shared_object(key_id)->double_vector_) {
				values.push_back(it);
			}
			return true;
		}
		return false;
	}


protected:

	/** @brief Shared memory
	*/
	SharedMemoryManager smm_;
	std::string name_shm_;

	/** @brief container with the bytes to instantiate for each object
	*/
	std::vector<size_t> v_;
	std::vector<std::vector<int>> information_size_;
	/** @brief Total allocated memory
	*/
	size_t memory_to_allocate_bytes_;

	/** @brief If true it continue to process the internal thread
	*/
	bool do_continue_;

	/** @brief Thread of the process
	*/
	std::thread t_process_;
	/** @brief Thread of the process
	*/
	std::vector<std::thread> container_t_process_;


	/** @brief Container with the callback functions
	*/
	registration_callback_function_shared m_callback_;

	///** @brief It guarantee that the passed data is valid
	//*/
	//std::mutex c_mutex_;
	///** @brief Condition variable
	//*/
	//std::condition_variable c_cv_;
	///** @brief To avoid spurious wakeup
	//*/
	//bool c_is_ready_;
	boost::interprocess::interprocess_mutex *mtx_;
	boost::interprocess::interprocess_condition *cnd_;


	/** @brief Container with the index of the object

		It is valid only for the object that allocated it
	*/
	//std::map<std::string, int> key_id_;

	/** @brief Multiple sources mtx and condition variable
	*/
	std::vector<boost::interprocess::interprocess_mutex*> vmtx_;
	std::vector<boost::interprocess::interprocess_condition*> vcnd_;
};

} // namespace shm
} // namespace co

#endif // COMMONOBJECTS_SHMCOMMON_SHAREDDATABASE_HPP__