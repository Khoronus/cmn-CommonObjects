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
* @author Alessandro Moro <alessandromoro.italy@gmail.com>
* @bug No known bugs.
* @version 0.1.0.0
*
*/

#ifndef COMMONOBJECTS_STRINGCOMMON_STRINGTYPECONVERSION_HPP__
#define COMMONOBJECTS_STRINGCOMMON_STRINGTYPECONVERSION_HPP__

#include <string>
#include <vector>

namespace co
{
namespace text
{

template <typename T, typename... Ts> struct get_index;

template <typename T, typename... Ts>
struct get_index<T, T, Ts...> : std::integral_constant<std::size_t, 0> {};

template <typename T, typename Tail, typename... Ts>
struct get_index<T, Tail, Ts...> :
	std::integral_constant<std::size_t, 1 + get_index<T, Ts...>::value> {};

template <typename T, typename Tuple> struct get_index_in_tuple;

template <typename T, typename ... Ts>
struct get_index_in_tuple<T, std::tuple<Ts...>> : get_index<T, Ts...> {};


/** @brief It converts container of strings in a container of other format
*/
template <typename _Ty>
class StringTypeConversion
{
public:

	/** @brief It converts a vector of strings in the template type.

		It converts a vector of strings in the template type.
	*/
	static std::vector<_Ty> convert(const std::vector<std::string> &words) {

		using types = std::tuple<int, float, double, long double>;
		auto funcs = std::make_tuple(
			&StringTypeConversion::to_int, 
			&StringTypeConversion::to_float,
			&StringTypeConversion::to_double,
			&StringTypeConversion::to_longdouble);

		std::vector<_Ty> vals;
		for (auto &it : words) {
			try
			{
				if (it.size() > 0) {
					float v = (std::get<get_index_in_tuple<_Ty, types>::value>(funcs))(it);
					vals.push_back(v);
				}
			}
			catch (std::exception ex) {

			}
		}
		return vals;
	}

private:

	/** @brief It converts a container of strings in a container of floats
	*/
	static float to_float(const std::string &word) {
		try
		{
			return std::stof(word);
		}
		catch (std::exception ex) {

		}
		return 0;
	}

	/** @brief It converts a container of strings in a container of floats
	*/
	static double to_double(const std::string &word) {
		try
		{
			return std::stod(word);
		}
		catch (std::exception ex) {

		}
		return 0;
	}

	/** @brief It converts a container of strings in a container of floats
	*/
	static long double to_longdouble(const std::string &word) {
		try
		{
			return std::stold(word);
		}
		catch (std::exception ex) {

		}
		return 0;
	}

	/** @brief It converts a container of strings in a container of floats
	*/
	static int to_int(const std::string &word) {
		try
		{
			return std::stoi(word);
		}
		catch (std::exception ex) {

		}
		return 0;
	}
};


} // namespace text
} // namespace co

#endif // COMMONOBJECTS_STRINGCOMMON_STRINGTYPECONVERSION_HPP__