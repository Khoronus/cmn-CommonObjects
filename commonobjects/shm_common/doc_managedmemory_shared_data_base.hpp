/**
* @file doc_managedmemory_shared_data_v3.hpp
* @brief Shared memory manager
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
* @version_date 20200413
*
* @known issues:
* > not wake up immediately (windows)
*   https://github.com/boostorg/interprocess/issues/43
*
*
* @previous name COMMONOBJECTS_SHM_COMMON_DOC_MANAGEDMEMORY_SHAREDDATA_BASE_HPP__
*/

#ifndef COMMONOBJECTS_SHM_COMMON_DOC_MANAGEDMEMORY_SHAREDDATA_BASE_HPP__
#define COMMONOBJECTS_SHM_COMMON_DOC_MANAGEDMEMORY_SHAREDDATA_BASE_HPP__

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/container/scoped_allocator.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/thread/thread_time.hpp>

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>

namespace co
{
namespace shm
{

//Typedefs of allocators and containers
typedef boost::interprocess::managed_shared_memory::segment_manager                                          segment_manager_t;
typedef boost::container::scoped_allocator_adaptor<boost::interprocess::allocator<void, segment_manager_t> > void_allocator;

typedef void_allocator::rebind<int>::other                                             int_allocator;
typedef boost::container::vector<int, int_allocator>                                   int_vector;
typedef void_allocator::rebind<double>::other                                          double_allocator;
typedef boost::container::vector<double, double_allocator>                             double_vector;

typedef void_allocator::rebind<char>::other                                            char_allocator;
typedef boost::container::basic_string<char, std::char_traits<char>, char_allocator>   char_string;

//
// @link http://www.boost.org/doc/libs/1_48_0/doc/html/interprocess/quick_guide.html

/** @brief Shared Object set in the shared memory between process.
*/
class SharedObject
{
public:

	//Since void_allocator is convertible to any other allocator<T>, we can simplify
	//the initialization taking just one allocator for all inner containers.
	typedef void_allocator allocator_type;

	SharedObject(const allocator_type &void_alloc) : //ptr_(nullptr),
		object_type_(void_alloc), object_name_(void_alloc),
		char_string_(void_alloc),
		int_vector_(void_alloc), double_vector_(void_alloc) {
		std::cout << "SharedObject()" << std::endl;
	}

	~SharedObject() {
		std::cout << "~SharedObject()" << std::endl;
	}

	void set_ptr(boost::interprocess::offset_ptr<void> &ptr, size_t ptr_size) {
		ptr_ = ptr;
		ptr_size_ = ptr_size;
	}

	/** @brief Access to the pointer to the allocated shared memory
	*/
	void* ptr() const {
		return ptr_.get();
	}

	/** @brief It returns the size of the allocated memory
	*/
	size_t ptr_size() {
		return ptr_size_;
	}

	/** @brief Container with the description of the object type
	*/
	char_string object_type_;
	/** @brief Container with the name of the object
	*/
	char_string object_name_;
	/** @brief Array char data
	*/
	char_string char_string_;
	/** @brief Array int data
	*/
	int_vector int_vector_;
	/** @brief Array double data
	*/
	double_vector double_vector_;

private:

	/** @brief Container with a raw pointer to data
	*/
	boost::interprocess::offset_ptr<void> ptr_;
	/** @brief Allocated raw pointer memory size (in bytes)
	*/
	size_t ptr_size_;
};

typedef void_allocator::rebind<SharedObject>::other    SharedObject_allocator;
typedef boost::container::vector<SharedObject, SharedObject_allocator>   SharedObject_vector;


/** @brief Shared Memory Manager
*/
class SharedMemoryManager
{
public:

	/** @brief 'ctor
	*/
	SharedMemoryManager() : do_destroy_(false), shared_object_(nullptr),
		do_deallocate_(false), num_items_(0) {}

	~SharedMemoryManager() {
		std::cout << "~SharedMemoryManager" << std::endl;
		if (do_deallocate_) deallocate();
		if (do_destroy_) {
			if (!boost::interprocess::shared_memory_object::remove(shared_memory_name_.c_str())) {
				std::cout << "Unable to remove the shared memory" << std::endl;
			}
		}
	}

	/** @brief Create the shared memory

	@param[in] shared_memory_name The name associated to the shared memory.
	@param[in] memory_to_instantiate_size The full instantiated memory.
	*/
	bool create(
		const std::string &shared_memory_name, 
		int memory_to_instantiate_size) {

		shared_memory_name_ = shared_memory_name;

		do_destroy_ = true;

		std::cout << "shm_remove" << std::endl;
		if (boost::interprocess::shared_memory_object::remove(shared_memory_name_.c_str())) {
			std::cout << "Successfully removed the shared memory: " <<
				shared_memory_name_ << std::endl;
		}

		do_deallocate_ = true;

		//Create a managed shared memory
		boost::interprocess::managed_shared_memory managed_shm_tmp(boost::interprocess::create_only,
			shared_memory_name_.c_str(), memory_to_instantiate_size);

		//Check size
		assert(managed_shm_tmp.get_size() == memory_to_instantiate_size);

		std::swap(managed_shm_tmp, managed_shm_);
		return true;
	}

	/** @brief Detect an existing shared memory

		@param[in] shared_memory_name Name of the shared memory
		@param[in] shared_object_name Name of the shared object/s ("SharedObject")
	*/
	bool detect(
		const std::string &shared_memory_name,
		const std::string &shared_object_name) {

		try
		{

			shared_memory_name_ = shared_memory_name;
			//Create a managed shared memory
			std::cout << "Try to detect shared memory: " <<
				shared_memory_name_ << std::endl;

			shared_object_name_ = shared_object_name;
			std::cout << "Try to detect shared object: " <<
				shared_object_name_ << std::endl;

			boost::interprocess::managed_shared_memory managed_shm_tmp(
				boost::interprocess::open_only,
				shared_memory_name_.c_str());

			std::cout << "Swap previous managed shared memory" << std::endl;
			std::swap(managed_shm_tmp, managed_shm_);

			auto tmp = managed_shm_.find<SharedObject>(shared_object_name_.c_str());
			if (tmp.first == nullptr) return false;
			shared_object_ = tmp.first;
			num_items_ = tmp.second;
			assert(shared_object_ != 0);
			do_deallocate_ = false;
			do_destroy_ = false;

			// get the object name to define the key id
			for (size_t i = 0; i < num_items_; ++i) {
				key_id_[std::string(shared_object_[i].object_name_.begin(),
					shared_object_[i].object_name_.end())] = i;
			}

			return true;

		}
		catch (std::exception &ex) {
			std::cout << ex.what() << std::endl;
			return false;
		}
		return false;
	}

	/** @brief Instantiate n new objects.

		The object memory ptr (raw memory) is allocated at this point and it 
		is different from the memory used by the string or vectors 
		(integer/double).

		@param[in] index_size Instantiate n new objects with m objects of k bytes.
	*/
	bool instantiate(
		const std::string &shared_object_name,
		std::vector<size_t> &index_size) {

		shared_object_name_ = shared_object_name;
		std::cout << "Try to instantiate shared object: " <<
			shared_object_name_ << std::endl;

		//An allocator convertible to any allocator<T, segment_manager_t> type
		void_allocator alloc_inst(managed_shm_.get_segment_manager());

		// Construct an array of named objects
		// The total number of objects is instantiated and fix
		shared_object_ = managed_shm_.find_or_construct<SharedObject>(
			shared_object_name_.c_str())[index_size.size()](alloc_inst);

		// It allocates the pointer memory (raw memory) for each object.
		for (size_t i = 0; i < index_size.size(); ++i)
		{
			// Allocate n bytes
			boost::interprocess::offset_ptr<void> o_ptr = managed_shm_.allocate(
				index_size[i], std::nothrow);
			if (o_ptr == nullptr) return false;
			shared_object_[i].set_ptr(o_ptr, index_size[i]);
		}

		// Total number of objects
		num_items_ = index_size.size();

		return true;
	}

	/** @brief Deallocate existing objects
	*/
	bool deallocate() {
		std::cout << "deallocate" << std::endl;
		std::pair<SharedObject*, std::size_t> p =
			managed_shm_.find<SharedObject>(shared_object_name_.c_str());

		for (size_t i = 0; i < p.second; ++i)
		{
			//Deallocate it
			managed_shm_.deallocate(p.first[i].ptr());
			// Destroy the pointer
			managed_shm_.destroy_ptr(p.first);
		}
		return true;
	}

	/** @brief Push new information

		@previous push_info
	*/
	void copyFrom(size_t id, const std::string &msg, std::vector<int> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Fill the new shared string
			shared_object_[id].char_string_ = msg.c_str();
			// Clear and fill the new vector data
			shared_object_[id].int_vector_.clear();
			for (auto it : value)
			{
				shared_object_[id].int_vector_.push_back(it);
			}
		}
	}
	/** @brief It push a new vector data

		@previous push_info
	*/
	void copyFrom_Veci(size_t id, std::vector<int> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Clear and fill the new vector data
			shared_object_[id].int_vector_.clear();
			for (auto it : value)
			{
				shared_object_[id].int_vector_.push_back(it);
			}
		}
	}

	/** @brief Modify the current information (without change the size of the
               object)

		@previous modify_info
	*/
	void modify_Veci(size_t id, std::vector<int> &value) {
		if (id >= 0 && id < num_items_)
		{
			if (shared_object_[id].int_vector_.size() == value.size()) {
				for (size_t i = 0; i < value.size(); ++i) {
					shared_object_[id].int_vector_[i] = value[i];
				}
			}
		}
	}

	/** @brief Push new information
	*/
	void set_object_type(size_t id, const std::string &msg) {
		if (id >= 0 && id < num_items_)
		{
			// Fill the new shared string
			shared_object_[id].object_type_ = msg.c_str();
			// Set the ID of the associated object (fast access)
			key_id_[msg] = id;
		}
	}

	/** @brief Push new information
	*/
	void set_object_name(size_t id, const std::string &msg) {
		if (id >= 0 && id < num_items_)
		{
			// Fill the new shared string
			shared_object_[id].object_name_ = msg.c_str();
			// Set the ID of the associated object (fast access)
			key_id_[msg] = id;
		}
	}

	/** @brief Push new information
	*/
	std::string object_type(size_t id) {
		if (id >= 0 && id < num_items_)
		{
			return std::string(shared_object_[id].object_type_.begin(),
				shared_object_[id].object_type_.end());
		}
		return std::string();
	}

	/** @brief Push new information
	*/
	std::string object_name(size_t id) {
		if (id >= 0 && id < num_items_)
		{
			return std::string(shared_object_[id].object_name_.begin(),
				shared_object_[id].object_name_.end());
		}
		return std::string();
	}


	/** @brief Push new information

		@previous set
	*/
	void set_string(size_t id, const std::string &msg) {
		if (id >= 0 && id < num_items_)
		{
			// Fill the new shared string
			shared_object_[id].char_string_ = msg.c_str();
		}
	}

	/** @brief Push new information

		@previous set
	*/
	void copyFrom_Vecd(size_t id, const std::vector<double> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Clear and fill the new vector data if the size is different
			if (value.size() != shared_object_[id].double_vector_.size()) {
				shared_object_[id].double_vector_.clear();
				for (auto it : value)
				{
					shared_object_[id].double_vector_.push_back(it);
				}
			} else {
				for (size_t i = 0; i < value.size(); ++i) {
					shared_object_[id].double_vector_[i] = value[i];
				}
			}
		}
	}

	/** @brief Push new information

		@previous set
	*/
	void copyFrom_Veci(size_t id, const std::vector<int> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Clear and fill the new vector data if the size is different
			if (value.size() != shared_object_[id].int_vector_.size()) {
				shared_object_[id].int_vector_.clear();
				for (auto it : value)
				{
					shared_object_[id].int_vector_.push_back(it);
				}
			} else {
				for (size_t i = 0; i < value.size(); ++i) {
					shared_object_[id].int_vector_[i] = value[i];
				}
			}
		}
	}


	/** @brief Push new information

		@previous_name push_info
	*/
	void copyFrom(size_t id, const std::string &msg, std::vector<double> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Fill the new shared string
			shared_object_[id].char_string_ = msg.c_str();
			// Clear and fill the new vector data
			shared_object_[id].double_vector_.clear();
			for (auto it : value)
			{
				shared_object_[id].double_vector_.push_back(it);
			}
		}
	}

	/** @brief It push a new vector data.

		It push a new vector data. It modifies the size of the object.
		@Important The internal vector is resized
		@previous_name push_info
	*/
	void copyFrom_Vecd(size_t id, std::vector<double> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Clear and fill the new vector data if the size is different
			if (value.size() != shared_object_[id].double_vector_.size()) {
				// Fill the new shared string
				// Clear and fill the new vector data
				shared_object_[id].double_vector_.clear();
				for (auto it : value)
				{
					shared_object_[id].double_vector_.push_back(it);
				}
			} else {
				for (size_t i = 0; i < value.size(); ++i) {
					shared_object_[id].double_vector_[i] = value[i];
				}
			}

			//// Fill the new shared string
			//// Clear and fill the new vector data
			//shared_object_[id].double_vector_.clear();
			//for (auto it : value)
			//{
			//	shared_object_[id].double_vector_.push_back(it);
			//}
		}
	}

	/** @brief It push a new vector data.

		It push a new vector data. It modifies the size of the object.
		@Important The internal vector is resized
		@previous_name push_info
	*/
	void copyTo_Vecd(size_t id, std::vector<double> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Clear and fill the new vector data if the size is different
			if (value.size() != shared_object_[id].int_vector_.size()) {
				// Fill the new shared string
				// Clear and fill the new vector data
				value.clear();
				for (auto it : shared_object_[id].double_vector_)
				{
					value.push_back(it);
				}
			} else {
				for (size_t i = 0; i < value.size(); ++i) {
					value[i] = shared_object_[id].double_vector_[i];
				}
			}
		}
	}

	/** @brief It push a new vector data.

		It push a new vector data. It modifies the size of the object.
		@Important The internal vector is resized
		@previous_name push_info
	*/
	void copyTo_Veci(size_t id, std::vector<int> &value) {
		if (id >= 0 && id < num_items_)
		{
			// Clear and fill the new vector data if the size is different
			if (value.size() != shared_object_[id].int_vector_.size()) {
				// Fill the new shared string
				// Clear and fill the new vector data
				value.clear();
				for (auto it : shared_object_[id].int_vector_)
				{
					value.push_back(it);
				}
			}
			else {
				for (size_t i = 0; i < value.size(); ++i) {
					value[i] = shared_object_[id].int_vector_[i];
				}
			}
		}
	}



	/** @brief Modify the current information (without change the size of the 
	           object)

		@previous_name modify_info
	*/
	void modify_Vecd(size_t id, std::vector<double> &value) {
		if (id >= 0 && id < num_items_)
		{
			if (shared_object_[id].double_vector_.size() == value.size()) {
				for (size_t i = 0; i < value.size(); ++i) {
					shared_object_[id].double_vector_[i] = value[i];
				}
			}
		}
	}

	/** @brief It sets the pointer data information

		@previous_name set_ptr
	*/
	bool copyFrom(size_t id, void *ptr, size_t bytes) {
		if (id >= 0 && id < num_items_ &&
			bytes < shared_object_[id].ptr_size()) {
			memcpy(shared_object_[id].ptr(), ptr, bytes);
			return true;
		}
		return false;
	}

	/** @brief It sets the pointer data information

		@previous_name set_ptr
	*/
	bool copyTo(size_t id, void *ptr, size_t bytes) {
		if (id >= 0 && id < num_items_ &&
			bytes < shared_object_[id].ptr_size()) {
			memcpy(ptr, shared_object_[id].ptr(), bytes);
		}
	}


	/** @brief Get string content

		@previous get
	*/
	std::string get_string(size_t id) {
		if (id >= 0 && id < num_items_)
		{
			return std::string(shared_object_[id].char_string_.begin(),
				shared_object_[id].char_string_.end());
		}
		return std::string();
	}
	/** @brief Get string content

		@previous get
	*/
	std::string get_string(const std::string &key) {
		int id = key_id_[key];
		if (id >= 0 && id < num_items_)
		{
			return std::string(shared_object_[id].char_string_.begin(),
				shared_object_[id].char_string_.end());
		}
		return std::string();
	}

	/** @brief It gets the pointer to the data information

		@param[in] id Object to have access
		@param[out] size Size of the valid memory data insert
		@return Return a valid pointer if the object exists. nullptr otherwise.
	*/
	void* get_ptr(size_t id, size_t &size) {
		if (id >= 0 && id < num_items_) {
			size = shared_object_[id].ptr_size();
			return shared_object_[id].ptr();
		}
		size = 0;
		return nullptr;
	}

	/** @brief Direct access to a shared object
	*/
	SharedObject* shared_object(size_t id) const {
		if (id >= 0 && id < num_items_) {
			return &shared_object_[id];
		}
		return nullptr;
	}

	size_t num_items() {
		return num_items_;
	}

	/** @brief It find or create a mutex of given name and add to shared memory
	*/
	boost::interprocess::interprocess_mutex* find_or_create_mutex(
		const std::string &name) {
		return managed_shm_.find_or_construct<boost::interprocess::interprocess_mutex>(name.c_str())();
	}

	/** @brief It find or create a condition of given name and add to shared memory
	*/
	boost::interprocess::interprocess_condition* find_or_create_condition(
		const std::string &name) {
		return managed_shm_.find_or_construct<boost::interprocess::interprocess_condition>(name.c_str())();
	}

private:

	/** @brief Name of the shared memory
	*/
	std::string shared_memory_name_;

	/** @brief Name of the shared object
	*/
	std::string shared_object_name_;

	/** @brief It true it destroy the shared memory space. It is set when
	instantiate.
	*/
	bool do_destroy_;
	bool do_deallocate_;

	/** @brief Managed shared memory
	*/
	boost::interprocess::managed_shared_memory managed_shm_;

	/** @brief Shared objects
	*/
	SharedObject *shared_object_;
	/** @brief Number of items
	*/
	size_t num_items_;

	/** @brief Access to the managed shared memory
	*/
	boost::interprocess::managed_shared_memory& managed_shm() {
		return managed_shm_;
	}

	// Container with the key information already assigned
	std::map<std::string, int> key_id_;
};

} // namespace shm
} // namespace co

#endif COMMONOBJECTS_SHM_COMMON_DOC_MANAGEDMEMORY_SHAREDDATA_BASE_HPP__