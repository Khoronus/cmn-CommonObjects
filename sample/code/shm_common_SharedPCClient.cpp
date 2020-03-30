/**
* @file shm_common_SharedPCServer.cpp
* @brief Example of the referred class.
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


#include <algorithm>            // std::min, std::max
#include <string>
#include <ctime>
#include <regex>
#include <iostream>
#include <sstream>      // std::stringstream

#include "../../commonobjects/shm_common/SharedPCClient.hpp"


/** @brief Function which transfer data
*/
void my_func(co::shm::SharedMemoryManager &smm) {

	// number of sources
	int num_sources = smm.num_items();
	for (int i = 0; i < num_sources; ++i) {
		co::shm::SharedObject* so = &smm.shared_object()[i];
		std::cout << "id: " << i << " " << so->char_string_ << std::endl;
	}
}


int main() {
	std::cout << "SharedPCClient" << std::endl;

	// shared data
	co::shm::SharedDataClient shared_data_client;
	//std::vector<std::pair<SizeOfEachPointBytes, NumberOfPoints>> definition =
	//{ { sizeof(float) * 5, 1280 * 720 } };

	// bind the function
	shared_data_client.registerCallback(std::bind(&my_func,
		std::placeholders::_1));
	// create and start
	//shared_data_server.create("MySharedMemory", definition);
	std::string name_shm = "MySharedMemory";
	if (!shared_data_client.detect(name_shm)) {
		std::cout << "Unable to detect: " << name_shm << std::endl;
		return 1;
	}
	shared_data_client.start();


	std::cout << "press a key" << std::endl;
	int keypressed = 0;
	std::cin >> keypressed;
}