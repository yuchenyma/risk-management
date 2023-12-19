#include <iostream>
#include <iomanip>
#include <cstdint>
#include "Streamer.h"

//using namespace std;
using namespace minirisk;

//void out_char_as_hex(int c)
//{
//    cout << hex << setw(2) << setfill('0') << c;
//}

int main()
{
    union { double d; uint64_t u; } tmp;
    double x = -0.15625;
    tmp.d = x;
    std::cout << std::hex << tmp.u << std::endl;


	/*Q3 Demo below:
	when writing: given a double -> reinterpret as a 64 bits integer -> save in hex format as string
	when reading: read the string in hex format -> convert to 64 bits integer -> reinterpret as double
	*/
	{
		double y = -0.15625;
		my_ofstream outfile("../../data/DemoHex.txt");
		outfile << y;
		outfile.endl();
		outfile.close();

		my_ifstream infile("../../data/DemoHex.txt");
		double i;
		infile.read_line();
		infile >> i;
		std::cout << i << std::endl;
	}

    return 0;
}
