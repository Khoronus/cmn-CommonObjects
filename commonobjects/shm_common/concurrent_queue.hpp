/**
 * @file concurrent_queue.hpp
 * @brief This file contains the class necessary to safe transfer the data from
 * different threads.
 *
 * @section LICENSE
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR/S BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * @bug No known bugs.
 * @version 2.0.0.0
 * 
 */


#ifndef COMMONOBJECTS_SHMCOMMON_CONCURRENTQUEUE_HPP__
#define COMMONOBJECTS_SHMCOMMON_CONCURRENTQUEUE_HPP__

#include <iostream>
#include <deque>
#include <queue>

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace co
{
namespace shm
{

namespace
{
/** @brief The iostreams are not guaranteed to be thread-safe!
	
	The iostreams are not guaranteed to be thread-safe!
*/ 
boost::mutex asio; 
} // anonymous namespace 

/** @brief Cuncurrent queue

	Cuncurrent queue
	
	@link: http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
*/
template<typename Data>
class concurrent_queue
{
public:

	/** @brief 'ctor
	*/
	concurrent_queue(const size_t CUNCURRENT_QUEUE_MAXSIZE) {
		CUNCURRENT_QUEUE_MAXSIZE_ = CUNCURRENT_QUEUE_MAXSIZE;
	}

	/** @brief Add a new element.

		Add a new element.

		@param[in] data Data to add to the queue.
	*/
	bool push(Data const& data)
	{
		bool result = false;
		boost::mutex::scoped_lock lock(the_mutex);
		// Add an element only if there are not too many images memorized
		if (the_queue.size() < CUNCURRENT_QUEUE_MAXSIZE - 1) {
			result = true;
			the_queue.push(data);
		}
		lock.unlock();
		the_condition_variable.notify_one();
		return result;
	}

	/** @brief Check if the queue is empty.

		Check if the queue is empty.

		@return Return TRUE is the queue is empty.
	*/
	bool empty() const
	{
		boost::mutex::scoped_lock lock(the_mutex);
		return the_queue.empty();
	}

	/** @brief Try to pop an element from the queue.

		Try to pop an element from the queue.

		@param[out] popped_value Extracted element if the pop succeeded.
		@return Return TRUE in case of success.
	*/
	bool try_pop(Data& popped_value)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		if(the_queue.empty())
		{
			return false;
		}
        
		popped_value=the_queue.front();
		the_queue.pop();
		return true;
	}

	/** @brief Wait until an element can be popped.

		Pop an element from the queue. If there are no elements it waits.
		If there are too many elements, it removes the remaining elements.

		@param[out] popped_value Extracted element.
	*/
	void wait_and_pop(Data& popped_value)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		while(the_queue.empty())
		{
			the_condition_variable.wait(lock);
		}
        
		popped_value=the_queue.front();
		the_queue.pop();


		// 20130201 Add the condition. If there are too many elements inside 
		// the list, then clear the list.
		if (the_queue.size() > CUNCURRENT_QUEUE_MAXSIZE_)
			while(!the_queue.empty())
				the_queue.pop()
	}

	/** @brief Wait until an element can be popped.

		Pop an element from the queue. If there are no elements it waits.
		If there are too many elements, it removes the remaining elements.

		@param[out] popped_value Extracted element.
		@return Return the number of elements in the queue.
	*/
	int wait_and_pop_size(Data& popped_value)
	{
		boost::mutex::scoped_lock lock(the_mutex);
		while(the_queue.empty())
		{
			the_condition_variable.wait(lock);
		}
        
		popped_value=the_queue.front();
		the_queue.pop();

		// 20130201 Add the condition. If there are too many elements inside 
		// the list, then clear the list.
		if (the_queue.size() > CUNCURRENT_QUEUE_MAXSIZE_)
			while(!the_queue.empty())
				the_queue.pop();

		return the_queue.size();
	}

	int size() {
		return the_queue.size();
	}

private:

	/** @brief Queue with the memorized data.
	*/
	std::queue<Data> the_queue;

	/** @brief Mutex to protect the data access.
	*/
	mutable boost::mutex the_mutex;

	/** @brief Condition variable to protect the thread access.
	*/
	boost::condition_variable the_condition_variable;

	/** Define the maximum number of elements in a queue.
	*/
	size_t CUNCURRENT_QUEUE_MAXSIZE_;

};

} // namespace shm
} // namespace co

#endif // COMMONOBJECTS_SHMCOMMON_CONCURRENTQUEUE_HPP__