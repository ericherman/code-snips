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
