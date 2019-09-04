/**
* @file enum_helper.hpp
* @brief Header of the relative class
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
* @original  
* @bug No known bugs.
* @version 0.1.0.0
*
* link: https://codereview.stackexchange.com/questions/14309/conversion-between-enum-and-string-in-c-class-header
*       https://github.com/quicknir/wise_enum
*/

#ifndef COMMONOBJECTS_ENUM_HELPER_HPP__
#define COMMONOBJECTS_ENUM_HELPER_HPP__

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

namespace co
{

// This is the type that will hold all the strings.
// Each enumeration type will declare its own specialization.
// Any enum that does not have a specialization will generate a compiler error
// indicating that there is no definition of this variable (as there should be
// be no definition of a generic version).
template<typename T>
struct enumStrings
{
	static char const* data[];
};

// This is a utility type.
// Created automatically. Should not be used directly.
template<typename T>
struct enumRefHolder
{
	T& enumVal;
	enumRefHolder(T& enumVal) : enumVal(enumVal) {}
};
template<typename T>
struct enumConstRefHolder
{
	T const& enumVal;
	enumConstRefHolder(T const& enumVal) : enumVal(enumVal) {}
};

// The next two functions do the actual work of reading/writing an
// enum as a string.
template<typename T>
std::ostream& operator<<(std::ostream& str, enumConstRefHolder<T> const& data)
{
	return str << enumStrings<T>::data[data.enumVal];
}

template<typename T>
std::istream& operator>>(std::istream& str, enumRefHolder<T> const& data)
{
	std::string value;
	str >> value;

	// These two can be made easier to read in C++11
	// using std::begin() and std::end()
	//  
	static auto begin = std::begin(enumStrings<T>::data);
	static auto end = std::end(enumStrings<T>::data);

	auto find = std::find(begin, end, value);
	if (find != end)
	{
		data.enumVal = static_cast<T>(std::distance(begin, find));
	}
	return str;
}


// This is the public interface:
// use the ability of function to deduce their template type without
// being explicitly told to create the correct type of enumRefHolder<T>
template<typename T>
enumConstRefHolder<T>  enumToString(T const& e) { return enumConstRefHolder<T>(e); }

template<typename T>
enumRefHolder<T>       enumFromString(T& e) { return enumRefHolder<T>(e); }

} // namespace co

#endif // COMMONOBJECTS_ENUM_HELPER_HPP__