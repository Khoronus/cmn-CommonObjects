/**
* @file StringOp.hpp
* @brief Elaboration of strings
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

#ifndef COMMONOBJECTS_STRINGCOMMON_STRINGOP_HPP__
#define COMMONOBJECTS_STRINGCOMMON_STRINGOP_HPP__

#include <string>
#include <vector>
#include <regex>

namespace co
{
namespace text
{
/** @brief Class to perform some string operation
*/
class StringOp
{
public:

	/** @brief Split a string

		It splits a string with a single char delimiter
	*/
	static std::vector<std::string> split(const std::string &s, char delim) {
		std::vector<std::string> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	/** @brief It appends a set of chars in front of a number.

		It appends a set of chars in front of a number.
		i.e. 16 becomes 000016
	*/
	static std::string append_front_chars(
		int n_zero, int num_frame, char c) {
		std::string old_string = std::to_string(num_frame);
		std::string new_string = std::string(n_zero - old_string.length(), c) +
			old_string;
		return new_string;
	}

	static std::vector<std::string> split_regex(const std::string& input, const std::string& regex) {
		// passing -1 as the submatch index parameter performs splitting
		std::regex re(regex);
		std::sregex_token_iterator
			first{ input.begin(), input.end(), re, -1 },
			last;
		return{ first, last };
	}


private:

	/** @brief Split a string
	*/
	template<typename Out>
	static void split(const std::string &s, char delim, Out result) {
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			*(result++) = item;
		}
	}

};

} // namespace text
} // namespace co

#endif // COMMONOBJECTS_STRINGCOMMON_STRINGOP_HPP__