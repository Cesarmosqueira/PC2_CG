#include <iostream>

#include "maze.hpp"

using namespace std;

int main() {
	vector<vector<int>> maze = mkMaze(10);

	for (auto x : maze) {
		for (auto y : x) {
			cout << (y == 1? "  " : (y == 3? "==" : "██")); // esto -> ██ deben ser bloques
		}
		cout << endl;
	}
	// you should see a beautiful maze

	return 0;
}
