#include <vector>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <fcntl.h>
#include <parser.hpp>

class my_parser: public animation::parser {
	public:
		std::vector<uint16_t> initial_indexes;
		std::vector<animation::stop<animation::basic_rgbw<double>>> stops;
		
		int emit_initial_index(uint16_t value) override {
			initial_indexes.push_back(value);
			return 1;
		}
		int emit_stop(animation::stop<animation::basic_rgbw<double>> &stop) {
			stops.push_back(stop);
			return 1;
		}
};
static inline std::string term_fcolor(int red, int green, int blue)
{
	return std::string("\x1b[38;2;" + std::to_string(red) + ';' + std::to_string(green) + ';' + std::to_string(blue) + 'm');
}
static inline std::string term_bcolor(int red, int green, int blue)
{
	return std::string("\x1b[48;2;" + std::to_string(red) + ';' + std::to_string(green) + ';' + std::to_string(blue) + 'm');
}
static inline std::string term_color(const animation::basic_rgbw<double> &color)
{
	int red   = color.red   * 255;
	int green = color.green * 255;
	int blue  = color.blue  * 255;
	int white = color.white * 255;
	return term_bcolor(white,white,white) + term_fcolor(red,green,blue);
}
int main(int argc, char** argv)
{
	switch (argc) {
		case 2:
			close(0);
			if (open(argv[1],O_RDONLY)) {
				perror(argv[1]);
				return 1;
			}
		case 1:break;
	}
	my_parser parser;
	char buffer[4096];
	while (std::cin.good()) {
		std::cin.read(buffer,sizeof(buffer));
		yajl_status status = parser.parse(buffer,std::cin.gcount());
		if (status != yajl_status_ok) {
			std::cerr << parser.get_error(status,buffer,sizeof(buffer)) << std::endl;
			return 1;
		}
	}
	if (!std::cin.eof()) {
		std::cerr << "Failed to read from input file" << std::endl;
		return 1;
	}
	yajl_status status = parser.complete_parse();
	if (status != yajl_status_ok) {
		std::cerr << parser.get_error(status,buffer,sizeof(buffer)) << std::endl;
		return 1;
	}
	// Sanity checks
	bool sane = true;
	for (auto i = 0; i < parser.initial_indexes.size(); i++)
		if (parser.initial_indexes[i] >= parser.stops.size()) {
			std::cerr << "Item " << i << " of \"initial_indexes\" is greater than the number of \"stops\" (" << parser.initial_indexes[i] << " must be less than " << parser.stops.size() << ")" << std::endl;
			sane = false;
		}
	for (auto i = 0; i < parser.stops.size(); i++)
		if (parser.stops[i].next_index >= parser.stops.size()) {
			std::cerr << "\"next_index\" of the " << i << "th \"stops\" is greater than the number of \"stops\" (" << parser.stops[i].next_index << " must be less than " << parser.stops.size() << ")" << std::endl;
			sane = false;
		}
	if (!sane)
		// Do not go further with such insane animation !
		return 1;
	
	// Print stops
	std::cout << "Color stops:" << std::endl;
	int num = 0;
	for (auto &i: parser.stops) {
		int red   = i.color.red * 255;
		int green = i.color.green * 255;
		int blue  = i.color.blue * 255;
		int white = i.color.white * 255;
		std::cout << "#" << num++ << ' ' << term_color(i.color) << "●\x1b[0m " << "\t \x1b[1;31m" << i.color.red << "\t\x1b[32m" << i.color.green << "\t\x1b[34m" << i.color.blue << "\t\x1b[39m" << i.color.white << "\x1b[0m\tswipe to " << term_color(parser.stops[i.next_index].color) << "#\x1b[0m" << i.next_index << "\tin " << i.duration/(float)10 << 's' << std::endl;
	}
	std::cout << std::endl;
	// Print initial state
	std::cout << "Initial LED states:" << std::endl;
	for (int i = 0; i < parser.initial_indexes.size(); i++) {
		const auto  initial_index = parser.initial_indexes[i % parser.initial_indexes.size()];
		const auto &stop = parser.stops[initial_index];
		std::cout << "#" << i << ' ' << term_color(stop.color) << "●\x1b[0m\tinitial_index:" << initial_index << std::endl;
	}
	return 0;
}
