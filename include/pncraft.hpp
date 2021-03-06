#include "renderer/Konstants.hpp"
#include "renderer/IndexBuffer.hpp"
#include "resources/Chunk.hpp"
#define LIGHT_MIN 0.4f
#define LIGHT_MAX 0.9f
#define LIGHT_OFF 0.02f


class World {
public:
    bool wireframe;
private:
    Shader* block_shader;   
    Shader* sun_shader;

    ui32 view_distance;
    ui32 DirtTexture, WaterTexture; 
    ui32 SandTexture, SunTexture;

    ui32 GrassTopTexture, GrassSideTexture;
    ui32 VAO, VBO; 
    i32 BUFFER_W, BUFFER_H;

    /*TODO: Make this IBO's set fancier*/
    IndexBuffer* UP;
    IndexBuffer* NORTH;
    IndexBuffer* EAST;
    IndexBuffer* WEST;

    IndexBuffer* SOUTH;
    IndexBuffer* DOWN;

    glm::mat4 projection;
    glm::vec3 lightColor;

    Chunk* chunk;

    f32 max_h;
    ui32 current_frame = 0;
    f32 X = 0, Z = 0;
    std::vector<std::vector<i32>> maze;
public:
    World() : 
        block_shader(new Shader("shaders/default/")), projection(glm::mat4(1.0f)),
        sun_shader(new Shader("shaders/coord/"))

    {

        max_h = CHUNK_HEIGHT - CHUNK_HEIGHT/4.0f;
        maze = mkMaze(CHUNK_SIDE);
        chunk = new Chunk(0, 0, maze, max_h);
        
        view_distance = 48;
        this->wireframe = false;
        this->mem_init();

        /* TODO: Make this a tilemap */
        DirtTexture = block_shader->loadTexture("blocks/dirt.jpg");

        GrassTopTexture = block_shader->loadTexture("blocks/grassTop.jpg");

        GrassSideTexture = block_shader->loadTexture("blocks/grassSide.jpg");

        WaterTexture = block_shader->loadTexture("blocks/water.jpg");

        SandTexture = block_shader->loadTexture("blocks/sand.jpg");

        SunTexture = block_shader->loadTexture("sun.jpg");

        lightColor = {0.9f, 0.1f, 0.05f};

        glBindVertexArray(VAO);
    }

    ~World(){
        /* clean object */
        std::cout << "About to destroy chunks\n";
        delete chunk;
        if(UP) delete UP;
        if(NORTH) delete NORTH;
        if(EAST) delete EAST;
        if(WEST) delete WEST;
        if(EAST) delete EAST;
        if(SOUTH) delete SOUTH;
        if(DOWN) delete DOWN;
        if(block_shader) delete block_shader;
    }

    static Chunk* find_chunk(const std::vector<Chunk*>& vec, const i32& x, const i32& z){
        for(Chunk* c : vec){
            if (c->X() == x && c->Z() == z){
                return c;
            }   
        }
        return nullptr;
    }
    void undo_move(const glm::vec3& pp, const glm::vec3& dir){
        std::cout << "UNDPOING UNDOING\n";
        std::cout << "UNDPOING UNDOING\n";
        std::cout << "UNDPOING UNDOING\n";
    }
    void on_update(const glm::vec3& pp, Sun*& sun, const glm::mat4& camView, glm::vec3 dir) {
        if (pp[0] != X || pp[2] != Z) {
            //update xChunk and zChunk
            //
            X = pp[0];
            Z = pp[2];
        }
        if(round(X) == 1 && round(Z) == 1 || int(X) % 4 == 0 && int(Z) % 6 == 0){
            lightColor = {0.9f, 0.1f, 0.05f};
        }

        current_frame =  (current_frame + 1) % 6000;
        block_shader->useProgram();
        if(pp[1] > CHUNK_SIDE-5){
            lightColor = {1.0f, 0.0f, 0.0f};
        }

        sun->X() = 2.0f * (cos(current_frame) + sin(current_frame));
		sun->Z() = 2.0f * (cos(current_frame) - sin(current_frame));
        block_shader->setVec3("xyz", sun->X(), sun->Y(), sun->Z());

        block_shader->setVec3("xyzColor", lightColor);

        block_shader->setMat4("xyzView", camView);
        
        if(lightColor[0] < LIGHT_MAX) lightColor[0] += LIGHT_OFF;
        else if(lightColor[0] > LIGHT_MIN) lightColor[0] -= LIGHT_OFF;

        if(lightColor[1] < LIGHT_MAX) lightColor[1] += LIGHT_OFF;
        else if(lightColor[1] > LIGHT_MIN) lightColor[1] -= LIGHT_OFF;


        glPolygonMode( GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
        chunk_draw_call(chunk->Data());
        on_sun_update(sun);
    }


    void update_width_height(const i32& w, const i32& h) {
        if (w != this->BUFFER_W && h != this->BUFFER_H) {
            std::cout << "Resizing\n";
            this->BUFFER_W = w;
            this->BUFFER_H = h;
            projection = glm::perspective(glm::radians(45.0f), /* size btw [1, 180] */
                (float)w/h, 0.1f, 360.0f);

            block_shader->useProgram();
            block_shader->setMat4("proj", projection);
            sun_shader->useProgram();
            sun_shader->setMat4("proj", projection);
        }
    }

    void send_view_mat(const glm::mat4& view) {
        block_shader->useProgram();
        block_shader->setMat4("view",view); 
        sun_shader->useProgram();
        sun_shader->setMat4("view",view); 
    }

    void toggle_wireframe() { this->wireframe = !this->wireframe;};

private:
    void chunk_draw_call(Block*** data) { 
        for(i32 x = 0; x < CHUNK_SIDE; x++){
            for(i32 z = 0; z < CHUNK_SIDE; z++){
                for(i32 y = 0; y < CHUNK_HEIGHT; y++){
                    block_draw_call(data, x, y,z);
                }
            }
        }
    }

    void block_draw_call(Block*** data, const i32& x, const i32&y, const i32& z) {
        if ( data[y][x][z].is_solid() ) {
            ui8 code = data[y][x][z].get_TexCode();
            if(code == 'S'){
                block_shader->setFloat("u_modifier", 0.2f); 
            } else {
                block_shader->setFloat("u_modifier", 0.6f); 
            }
            if(code != 'G'){
                glBindTexture(GL_TEXTURE_2D, code_to_tex(code));

                face_draw_call(data,    UP, x,y,z , 'U');
                face_draw_call(data, NORTH, x,y,z , 'N');
                face_draw_call(data,  EAST, x,y,z , 'E');
                face_draw_call(data,  WEST, x,y,z , 'W');
                face_draw_call(data, SOUTH, x,y,z , 'S');
                if(y > 1) face_draw_call(data,  DOWN, x,y,z , 'D');
            } else {

                glBindTexture(GL_TEXTURE_2D, GrassTopTexture);
                face_draw_call(data,    UP, x,y,z , 'U');
                glBindTexture(GL_TEXTURE_2D, GrassSideTexture);
                face_draw_call(data, NORTH, x,y,z , 'N');
                face_draw_call(data,  EAST, x,y,z , 'E');
                face_draw_call(data,  WEST, x,y,z , 'W');
                face_draw_call(data, SOUTH, x,y,z , 'S');
                if(y > 1){
                    glBindTexture(GL_TEXTURE_2D, DirtTexture);
                    face_draw_call(data,  DOWN, x,y,z , 'D');
                }
        
            }
        }
    }
    void on_sun_update(Sun*& sun) {
        sun_shader->useProgram();
        sun->rotation();
        sun_shader->setMat4("model", sun->model);
        glBindTexture(GL_TEXTURE_2D, SunTexture);
        NORTH->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        EAST->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        UP->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        DOWN->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        WEST->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SOUTH->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    ui32 code_to_tex(const ui8& code){
        switch(code){
        case 'W': return WaterTexture;
        case 'D': return DirtTexture;
        case 'S': return SandTexture;
        }
        std::cout << "Undefined texture code\n";
        return 1;
    }

    void face_draw_call(Block***& data,const IndexBuffer* face, 
           const i32& x, const i32&y, const i32& z, const ui8& code) { 
        
        if(true){
            switch(code){
            case 'N': block_shader->setVec3("normal", { 1, 0, 0} ); break;
            case 'E': block_shader->setVec3("normal", { 0, 0,-1} ); break;
            case 'U': block_shader->setVec3("normal", { 0, 1, 0} ); break;
            case 'D': block_shader->setVec3("normal", { 0,-1, 0} ); break;
            case 'W': block_shader->setVec3("normal", { 0, 0, 1} ); break;
            case 'S': block_shader->setVec3("normal", {-1, 0, 0} ); break;
            }
            block_shader->setMat4("model", data[y][x][z].model);
            face->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        } 
    }

    bool not_visible(Block***& blocks, const ui32& x, const ui32& y, const ui32& z, 
            const ui32& dir){ 
        if(dir == 'D') { //if in the bottom of the chunk
            if ( 0 < y ) {
                return blocks[y-1][x][z].is_solid();
            }
            else return false;
        }
        /* MUST SUPPORT BLOCKS FACING OTHER CHUNKS */
        if(dir == 'N') { 
            /* - Validate if its on the border of the current chunk - */
            if ( x < CHUNK_SIDE - 1 ) {
                return blocks[y][x + 1][z].is_solid();
            }
            else { //its on the border
                //validate with the adjacent chunk to (x+1)  
                return false;
            }
        }
        if(dir == 'W') { 
            if ( z < CHUNK_SIDE - 1 ) {
                return blocks[y][x][z+1].is_solid();
            }
            else {
                return false;
            }
        }
        if(dir == 'E') { 
            if ( 0 < z ) {
                return blocks[y][x][z-1].is_solid();
            }
            else { 
                return false;
            }
        }
        if(dir == 'S') { 
            if ( 0 < x ) {
                return blocks[y][x-1][z].is_solid();
            }
            else {
                return false;
            }
        }
        /* --------------------------------------- */
        if(dir == 'U'){ //if its on top of the chunk
            if ( y < CHUNK_HEIGHT-1 ) { 
                if (! blocks[y+1][x][z].is_solid() ) {
                    glBindTexture(GL_TEXTURE, GrassTopTexture);
                    return false;

                }
                else return true;
            }
            else { 
                return false;
            }
        }
        std::cerr << "Unknown block face\n";
        return false;
    }

    void mem_init(){
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(K::vertices), K::vertices, GL_STATIC_DRAW);

        /*Index buffers for each face of the cube*/
        UP = new IndexBuffer(K::UP, 6);
        NORTH = new IndexBuffer(K::NORTH, 6);
        EAST = new IndexBuffer(K::EAST, 6);
        WEST = new IndexBuffer(K::WEST, 6);
        SOUTH = new IndexBuffer(K::SOUTH, 6);
        DOWN = new IndexBuffer(K::DOWN, 6);

        // posiciones
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // shadows
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);
        //normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};
