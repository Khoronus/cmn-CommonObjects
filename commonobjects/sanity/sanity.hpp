/**
* @file Const.hpp
* @brief Constant values
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
* @original  Alessandro Moro
* @bug No known bugs.
* @version 0.1.0.0
*
*/
#ifndef COMMONOBJECTS_SANITY_SANITY_HPP__
#define COMMONOBJECTS_SANITY_SANITY_HPP__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <boost/filesystem.hpp>

namespace co
{
namespace sanity
{

/** @brief Class used to check the sanity of the folder tree.

	It is used to prevent that some data cannot be saved on Hard Drive.
*/
class FolderManager
{
public:

	/** @brief Check the sanity of the folders and files
	*/
	static void sanity_check(const std::string &fname_out) {

		// Sanity check
		boost::filesystem::path p(fname_out);
		boost::filesystem::path dir = p.parent_path();

		if (boost::filesystem::create_directories(dir)) {
			std::cout << "[+] Root::sanity_check: create " << dir << std::endl;
		} else {
			std::cout << "[-] Root::sanity_check: create " << dir << std::endl;
		}
	}

	/** @brief Check the sanity of the folders and files
	*/
	static void folder_sanity_check(const std::string &fname_out) {

		// Sanity check
		boost::filesystem::path dir(fname_out);

		if (boost::filesystem::create_directories(dir)) {
			std::cout << "[+] Root::sanity_check: create " << dir << std::endl;
		}
		else {
			std::cout << "[-] Root::sanity_check: create " << dir << std::endl;
		}
	}
};

} // namespace sanity
} // namespace co



#endif // COMMONOBJECTS_SANITY_SANITY_HPP__