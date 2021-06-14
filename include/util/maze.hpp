#ifndef __MAZE_HPP__
#define __MAZE_HPP__

#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <time.h>

// union-find galore for modified kruskal, don't mind it,
// yes yes it's disjointset stuff, ignore it I said!
int Find(std::vector<int>& ds, int a) {
	if (ds[a] < 0) {
		return a;
	} else {
		int daddy = Find(ds, ds[a]);
		ds[a] = daddy;
		return daddy;
	}
}
void Union(std::vector<int>& ds, int a, int b) {
	int A = Find(ds, a);
	int B = Find(ds, b);
	if (A == B) return;
	if (-ds[A] > -ds[B]) {
		ds[A] += ds[B];
		ds[B] = A;
	} else {
		ds[B] += ds[A];
		ds[A] = B;
	}
}

/* THIS IS THE IMPORTANT ONE!
 * Se genera un laberinto de n*2 + 1  por n*2 + 1 celdas
 * 0 son paredes 1 es camino libre 3 determina inicio y fin
 **/
std::vector<std::vector<int>> mkMaze(int n) {
	std::vector<std::vector<int>> maze(n*2+1, std::vector<int>(n*2+1, 0));

	std::vector<std::pair<int,int>> walls;

	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			maze[i*2+1][j*2+1] = 1;
		}
	}
	maze[1][0] = 3;
	maze[n*2-1][n*2] = 3;
	for (int i = 0; i < n - 1; ++i) {
		walls.push_back({i*n+n-1, (i+1)*n+n-1});
		walls.push_back({(n-1)*n+i, (n-1)*n+i+1});
		for (int j = 0; j < n - 1; ++j) {
			walls.push_back({i*n+j, i*n+j+1});
			walls.push_back({i*n+j, (i+1)*n+j});
		}
	}
	srand(time(0));
	for (int i = walls.size() - 1; i > 0; --i) {
		swap(walls[i], walls[rand() % i]);
	}
	std::vector<int> ds(walls.size(), -1);
	for (auto e : walls) {
		int r1 = e.first / n;
		int c1 = e.first % n;
		int r2 = e.second / n;
		int c2 = e.second % n;
		if (Find(ds, e.first) != Find(ds, e.second)) {
			if (r1 == r2 && c1 < c2) {
				maze[r1*2+1][c1*2+2] = 1;
			} else if (r1 == r2) {
				maze[r1*2+1][c2*2+2] = 1;
			} else if (c1 == c2 && r1 < r2) {
				maze[r1*2+2][c1*2+1] = 1;
			} else {
				maze[r2*2+2][c1*2+1] = 1;
			}
			Union(ds, e.first, e.second);
		}
	}

	return maze; // no se complique, corra mazeTest.cpp para vea como funciona ;)
}

#endif
