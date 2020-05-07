/**
* @file DateTime.hpp
* @brief Header of all the files related to the algorithm
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
* @original author Alessandro Moro
* @bug No known bugs.
* @version 0.1.0.0
*
*/

#ifndef COMMONOBJECTS_TIME_DATETIME_HPP__
#define COMMONOBJECTS_TIME_DATETIME_HPP__

#include <string>
#include <ctime>

namespace co
{
namespace cotime
{

/** @brief Class to manage the date and time
*/
class DateTime
{
public:

	/** @brief Get the current time and date in string form.
	*/
	static std::string get_date_as_string(void){
		time_t now;
		char the_date[1024];
		the_date[0] = '\0';
		now = time(NULL);
		if (now != -1)
		{
			strftime(the_date, 1024, "%Y-%m-%d.%X", gmtime(&now));
		}
		std::string str = std::string(the_date);
		for (int i = 0; i < str.length(); i++)
		{
			if (str[i] == ':') str[i] = '_';
		}
		return str;
	}

	/** @brief Get the time in a string format
	*/
	static std::string time2string() {
		time_t rawtime;
		struct tm * timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%Y_%m_%d_%I_%M_%S", timeinfo);
		std::string str(buffer);
		return str;
	}

};

} // namespace cotime
} // namespace co

#endif // COMMONOBJECTS_TIME_DATETIME_HPP__
