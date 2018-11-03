/* boost-example.cpp
   Copyright (C) 2014, 2018 Eric Herman <eric@freesa.org>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

	https://www.gnu.org/licenses/lgpl-3.0.txt
	https://www.gnu.org/licenses/gpl-3.0.txt
 */
#include <iostream>
#include <boost/array.hpp>

using namespace std;
int main(void)
{
	boost::array<char, 3> arr = { 'a', 'b', 'c' };
	cout << "boost::array<char, 3> arr = { 'a', 'b', 'c' };\n";
	for (size_t i = 0; i < arr.size(); i++) {
		cout << "\tarr[" << i << "]: " << arr[i] << "\n";
	}
	return 0;
}
