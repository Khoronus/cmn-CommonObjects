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

#include "../../commonobjects/shm_common/SharedPCServer.hpp"

void main() {
	std::cout << "SharedPCServer" << std::endl;

	co::shm::SharedDataServer shared_data_server;

	const int width = 640;
	const int height = 480;
	const int num_skeletons = 10;
	const int num_points_skeletons = 18;

	// Set the shared data
	std::string name_shm = "MySharedMemory";
	// which objects to allocate
	std::string cmd = "image," + std::to_string(width) +
		"," + std::to_string(height) + ",3|pcl,20," +
		std::to_string(width * height) + "|pose_kSize,12," +
		std::to_string(num_skeletons) + "," +
		std::to_string(num_points_skeletons);
	std::cout << "[shm::cmd]>>" << cmd << std::endl;
	// parse the command and allocate the memory
	int err = shared_data_server.parse(name_shm, cmd);
	if (err != co::shm::kSharedNoError) {
		std::cout << "shared_data error: " << err << std::endl;
	}


	while (true) {
		// share data
		if (true)
		{
			// set the maximum distance
			float max_distance_depth = 10.0f;
			float kRange = 10.0f;
			// set the shared data
			std::map<int, co::shm::ObservedObject> data;
			// create a new data object for the point cloud
			int shared_object_id = shared_data_server.get_key_id("pcl");
			data[shared_object_id] =
				(co::shm::ObservedObject() = { "serial", std::vector<float>() });

			// share the data
			shared_data_server.push_data(data);
			// create a new data object for the pose
			shared_object_id = shared_data_server.get_key_id("pose_kSize");
			data[shared_object_id] =
				(co::shm::ObservedObject() = { "serial", std::vector<float>() });
			//std::cout << "###" << std::endl;
			for (int i = 0; i < num_points_skeletons * 2; ++i) {
				//std::cout << "> " << it << std::endl;
				data[shared_object_id].points_data.push_back((float)rand() / RAND_MAX);
				data[shared_object_id].points_data.push_back((float)rand() / RAND_MAX);
				data[shared_object_id].points_data.push_back((float)rand() / RAND_MAX);
			}
			data[shared_object_id].num_points = data[shared_object_id].points_data.size() / num_points_skeletons;
			// share the data
			shared_data_server.push_data(data);
		}

	}



}