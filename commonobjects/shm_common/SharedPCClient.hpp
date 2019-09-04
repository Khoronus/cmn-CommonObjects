/**
* @file SharedPCClient.hpp
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
* @version 0.2.0.0
*
*/


#ifndef COMMONOBJECTS_SHMCOMMON_SHAREDPCCLIENT_HPP__
#define COMMONOBJECTS_SHMCOMMON_SHAREDPCCLIENT_HPP__

#include <map>

#include "doc_managedmemory_shared_data_v2.hpp"
#include "../string_common/StringOp.hpp"

namespace co
{
namespace shm
{

// Size of each point in bytes
typedef int SizeOfEachPointBytes;
// Total number of points for an object
typedef int NumberOfPoints;

/** @brief Function for callback
*/
//typedef std::function<void(void* data, size_t num_points, pcl::PointCloud<PointType>::Ptr &point_cloud_ptr, bool clear)> registration_callback_function;
typedef std::function<void(SharedMemoryManager &smm)> registration_callback_function;

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
class SharedDataClient
{
public:

	SharedDataClient() {
		memory_to_allocate_bytes_ = 0;
	}

	~SharedDataClient() {
		stop();
	}

	/** @brief It creates the memory which will contain the structured data.

		It creates the memory which will contain the structured data.

		@param[in] name_shm Shared memory name
		@param[in] definition Container wit the size of the points
							  (3float + 3 uchar), and the number of points
	*/
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
		t_process_ = std::thread(&SharedDataClient::process, this);
	}

	/** @brief It stops a running worker
	*/
	void stop() {
		std::cout << "<SharedPCClient::stop>" << std::endl;
		do_continue_ = false;
		if (t_process_.joinable()) {
			t_process_.join();
		}
		std::cout << "</SharedPCClient::stop>" << std::endl;
	}

	/** @brief If started from the function "start()" it will run in a
			   separated thread.
	*/
	void process() {
		try {
			do_continue_ = true;
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

			// Increase the thread priority
			SetThreadPriority(GetCurrentThread(),
				THREAD_MODE_BACKGROUND_BEGIN);
			int speed = 1;
			int num_sources = smm_.num_items();

			// image used to close the program
			while (do_continue_) {
				//std::cout << "Internal : " << num_sources << std::endl;

				// callback here
				if (m_callback_ != nullptr) {
					m_callback_(smm_);
					is_new_data_ = true;
				}

				//// Notify
				//cnd->notify_all();
				////cnd->wait(lock); if use the timeout
				//boost::system_time timeout = boost::get_system_time() + boost::posix_time::milliseconds(1000);
				//noTimeout = cnd->timed_wait(lock, timeout);
				//// timeout
				//if (!noTimeout)
				//{
				//	std::cout << "timeout!" << std::endl;
				//}
			}
			// Notify (multithread)
			//vcnd[i]->notify_all();
			cnd->notify_all();
			std::cout << "</SharedDataServer::process>" << std::endl;
		}
		catch (boost::interprocess::interprocess_exception &ex) {
			std::cout << ex.what() << std::endl;
			return;
		}
	}

	/** @brief It register the callback function
	*/
	void registerCallback(registration_callback_function &&callback) {
		m_callback_ = callback;
	}

	bool is_new_data() { return is_new_data_; }
	void set_is_new_data(bool is_new_data) { is_new_data_ = is_new_data; }

private:

	/** @brief Shared memory
	*/
	SharedMemoryManager smm_;
	std::string name_shm_;

	std::vector<int> v_; // container with the bytes for each image
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

	/** @brief If true is a new data
	*/
	bool is_new_data_;
};

} // namespace shm
} // namespace co

#endif // COMMONOBJECTS_SHMCOMMON_SHAREDPCCLIENT_HPP__