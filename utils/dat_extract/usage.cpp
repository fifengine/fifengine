#include <iostream>
int print_usage(const char* prg_name, int exit_code) {
	std::cout << "USAGE: " << prg_name;
	std::cout << " [-s DAT_source] [-f 1|2] -t target_file" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "This is a very limited DAT extraction utility that can read" << std::endl;
	std::cout << "Fo1 and Fo2 DAT files and extract a single file." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "The DAT source _has to be_ in the same directory, furthermore" << std::endl;
	std::cout << "the program needs the 'content' directory (otherwise the engine" << std::endl;
	std::cout << "will not start)." << std::endl;
	std::cout << "" << std::endl;
	std::cout << "The default DAT file is critter.dat, the default format is" << std::endl;
	std::cout << "FO1; you have to specify '-f 2' if you work on Fo2 DAT files!" << std::endl;
	std::cout << "" << std::endl;
	std::cout << "The target_file has to be the full path inside the DAT file" << std::endl;
	std::cout << "(using / as directory separator); the output file will have" << std::endl;
	std::cout << "the same filename (in the current directory)." << std::endl;
	exit(exit_code);
}
