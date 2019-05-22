
#include "Digging3DMazeGenerator.h"
#include <array>

int main() {

	std::array<std::array<std::array<int, 25>, 25>, 25> matrix{ {} };
	Maze::Digging3DMazeGenerator<int>(1, 0).CreateMaze(matrix);

}