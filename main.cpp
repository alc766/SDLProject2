#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;
glm::mat4 view2Matrix, model2Matrix, projection2Matrix;
glm::mat4 view3Matrix, model3Matrix, projection3Matrix;

//Right paddle
glm::vec3 player_position = glm::vec3(5,0,0);
glm::vec3 player_movement = glm::vec3(0,0,0);
//Left paddle
glm::vec3 player2_position = glm::vec3(-5,0,0);
glm::vec3 player2_movement = glm::vec3(0,0,0);
//ball
glm::vec3 ball_position = glm::vec3(0,0,0);
glm::vec3 ball_movement = glm::vec3(0,0,0);



float player_speed = 1.5f;

float ball_speed = 1.5f;



//right paddle
GLuint playerTextureID;

//left paddle
GLuint playerTextureID2;

//ball
GLuint playerTextureID3;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n"; assert(false);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    return textureID;
}


void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 640, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 640);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    
    view2Matrix = glm::mat4(1.0f);
    model2Matrix = glm::mat4(1.0f);
    
    view3Matrix = glm::mat4(1.0f);
    model3Matrix = glm::mat4(1.0f);
    
    model3Matrix = glm::scale(model3Matrix, glm::vec3(0.5f,0.5f,1.0f));
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    projection2Matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    projection3Matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    
    program.SetProjectionMatrix(projection2Matrix);
    program.SetViewMatrix(view2Matrix);
    
    program.SetProjectionMatrix(projection3Matrix);
    program.SetViewMatrix(view3Matrix);
    
    glUseProgram(program.programID);
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    
    //Yes they are all the same image of a black box. Originally used different images but it looked weird
    playerTextureID = LoadTexture("paddle.png");
    playerTextureID2 = LoadTexture("paddle.png");
    playerTextureID3 = LoadTexture("paddle.png");
    
}


void ProcessInput() {
    
    player_movement = glm::vec3(0);
    player2_movement = glm::vec3(0);
    ball_movement = glm::vec3(0);
   
    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type){
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                    case SDLK_LEFT:
                      
                        break;
                    case SDLK_RIGHT:
                        //move the player right
                        
                        break;
                    
                    case SDLK_SPACE:
                        //some sort of action
                        break;
                }
                break;
        }
    }
     // W and S move the left paddle up or down
    //Arrow keys move the right paddle
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    if (keys[SDL_SCANCODE_S]) {
        player2_movement.y = -1.5f;
    }
    else if (keys[SDL_SCANCODE_W]) {
        player2_movement.y = 1.5f;    }

    if (keys[SDL_SCANCODE_UP]) {
        player_movement.y = 1.5f;
    }
    else if (keys[SDL_SCANCODE_DOWN]) {
        player_movement.y = -1.5f;    }
    
    //Don't know if necessary to normalize 1d movement
//    if (glm::length(player_movement) > 1.5f) {
//        player_movement = glm::normalize(player_movement);
//    }
//    if (glm::length(player2_movement) > 1.5f) {
//        player2_movement = glm::normalize(player2_movement);
//    }
        
}

float lastTicks = 0.0f;
float ballYDirection = 0.0f;
bool hitLeftRacket = false;
bool hitRightRacket = false;
bool spaceBar = false;
void Update() {
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    //will start the ball when the spacebar is hit
    if (keys[SDL_SCANCODE_SPACE]) {
        spaceBar = true;
    }
    lastTicks = ticks;
    if(spaceBar){
        ball_movement.x = -1.5f; //starts the ball out going left in a straight line
    }
    ball_movement.y = ballYDirection;
   
    player_position += player_movement * player_speed * deltaTime;
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, player_position);
    
    player2_position += player2_movement * player_speed * deltaTime;
    model2Matrix = glm::mat4(1.0f);
    model2Matrix = glm::translate(model2Matrix, player2_position);
    
   
    ball_position.x += ball_movement.x * ball_speed * deltaTime;
    ball_position.y += ball_movement.y * ball_speed * deltaTime;
   
    model3Matrix = glm::mat4(1.0f);
    model3Matrix = glm::translate(model3Matrix, ball_position);
    model3Matrix = glm::scale(model3Matrix, glm::vec3(0.5f,0.5f,1.0f));
    
   
    
    
    //left racket hit
    if(ball_position.x < (player2_position.x+0.75f) && ball_position.x > (player2_position.x) &&ball_position.y < (player2_position.y+0.75f) && ball_position.y > player2_position.y-0.75f){
        hitLeftRacket = true;
        hitRightRacket = false;
        ball_speed = -1.5f;
        ball_movement.x = -fabs(ball_movement.x);
        
        //if it hits the top part of the paddle it will bounce up
        if(ball_position.y < player2_position.y+0.75 && ball_position.y > player2_position.y){
            ballYDirection = -1.5f;
        }
        //if it hits the lower part of the paddle it will bounce down
        if(ball_position.y > player2_position.y-0.75 && ball_position.y < player2_position.y){
            ballYDirection = 1.5f;
        }
    }
    //right racket hit
    if(ball_position.x > (player_position.x-0.75f) && ball_position.x < (player_position.x+1.0f) &&
       ball_position.y < (player_position.y+0.75f) && ball_position.y > player_position.y-0.75f){
        hitRightRacket = true;
        hitLeftRacket = false;
        ball_speed = 1.5f;
        ball_movement.x = fabs(ball_movement.x);
        
        if(ball_position.y < player_position.y+0.75 && ball_position.y > player_position.y){
            ballYDirection = 1.5f;
        }
        if(ball_position.y > player_position.y-0.75 && ball_position.y < player_position.y){
            ballYDirection = -1.5f;
        }
    }
    //If ball hits left wall. resets ball position and stops everything
    if(ball_position.x < -4.75){
        ball_position.x = 0;
        ball_movement.x = 0;
        ball_movement.y = 0;
        ball_position.y = 0;
        player_speed = 0;
        ball_speed = 0;

    }
    //right wall
    if(ball_position.x > 4.75){
        ball_position.x = 0;
        ball_movement.x = 0;
        ball_movement.y = 0;
        ball_position.y = 0;
        player_speed = 0;
        ball_speed = 0;

    }
    //top wall and came from right paddle
    if(ball_position.y > 3.5f && hitRightRacket == true){
          
           ballYDirection = -1.5f;
       }
    //top wall and came from left paddle
       if(ball_position.y > 3.5f && hitLeftRacket == true){
           
              ballYDirection = 1.5f;
          }
    //bottom wall and came from right racket
       if(ball_position.y < -3.5f && hitRightRacket == true){
           
              ballYDirection = 1.5f;
          }
    //bottom wall and came from left racket
       if(ball_position.y < -3.5f && hitLeftRacket == true){
           
              ballYDirection = -1.5f;
          }
    if(glm::length(ball_movement)>1.5f || glm::length(ball_movement) < -1.5f){
        ball_movement = glm::normalize(ball_movement);
    }
}



void Render() {
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetModelMatrix(modelMatrix);
    
    //right racket
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, playerTextureID);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
   
    //left racket
    program.SetModelMatrix(model2Matrix);
    float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords2[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords2);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, playerTextureID2);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    
    //ball
    program.SetModelMatrix(model3Matrix);
    float vertices3[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texCoords3[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices3);
    glEnableVertexAttribArray(program.positionAttribute);
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords3);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, playerTextureID3);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
    
}


void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
       
        ProcessInput();
        Update();
        Render();
       
        
    }
    
    Shutdown();
    return 0;
}

