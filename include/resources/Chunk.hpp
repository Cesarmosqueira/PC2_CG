#include "resources/Sun.hpp"
#include "util/maze.hpp"
#define CHUNK_HEIGHT 20
#define CHUNK_SIDE 40
#define WATER_LEVEL 16
bool vis;
char tex;

class Chunk { 
public: 
    Chunk(const i32& _x, const i32& _z, const std::vector<std::vector<i32>>& maze, const f32& max_h) : XPOS(_x), ZPOS(_z) { 
        XOFF = XPOS * CHUNK_SIDE;
        ZOFF = ZPOS * CHUNK_SIDE;

        blocks = new Block**[CHUNK_HEIGHT];
        for(i32 y = 0; y < CHUNK_HEIGHT; y++) { 

            blocks[y] = new Block*[CHUNK_SIDE];
            for(i32 x = 0; x < CHUNK_SIDE; x++){

                blocks[y][x] = new Block[CHUNK_SIDE];

                for(i32 z = 0; z < CHUNK_SIDE; z++){
                    if(y == 0 || y == CHUNK_HEIGHT-1 || 
                       x == 0 || x == CHUNK_SIDE-1 || 
                       z == 0 || z == CHUNK_SIDE-1){
                        vis = true;
                        tex = 'S';
                        if(y == 0 && (float)rand()/(float)RAND_MAX < 0.1f)
                            tex = 'G';
                    }
                    else {
                        if(y < CHUNK_HEIGHT * 0.3) {
                            vis = maze[x][z] == 0;
                            tex = 'W';
                        }
                        else {
                            vis = false;
                        }
                    }
                    
                    blocks[y][x][z].init(y, x + XOFF, z + ZOFF, vis, tex);
                }
            }
        }

    } 
    ~Chunk() { 
        std::cout << "About to destroy some chunk\n";
        for(ui32 y = 0; y < CHUNK_HEIGHT; y++) { 

            for(ui32 x = 0; x < CHUNK_SIDE; x++){
                delete[] blocks[y][x];
            }
            delete[] blocks[y];
        }
        delete[] blocks;
    }
    Block*** Data() {
        return blocks;
    }
    i32 X() { 
        return XPOS;
    }
    i32 Z() { 
        return ZPOS;
    }
private:
    i32 XPOS, ZPOS;
    Block ***blocks;
public:
    i32 heightmap[CHUNK_SIDE][CHUNK_SIDE];
    i32 XOFF, ZOFF;
};
