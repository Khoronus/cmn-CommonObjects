/**
* @file string_common_StringTypeConversion.cpp
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

#include "../../commonobjects/string_common/StringTypeConversion.hpp"

void main() {
	std::cout << "StringTypeConversion" << std::endl;

	std::vector<std::string> words = { "0", "51", "127.9167" };

	auto res = co::text::StringTypeConversion<float>::convert(words);
	int num = 0;
	for (auto it : res) {
		std::cout << "res[" << num << "]: " << it << std::endl;
		++num;
	}
}