#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Kích thước cửa sổ game
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Kích thước xe và chướng ngại vật
const int CAR_WIDTH = 25;
const int CAR_HEIGHT = 50;
const int OBSTACLE_WIDTH = 25;
const int OBSTACLE_HEIGHT = 45;

enum Gamestate{
MENU,
PLAYING,
GAME_OVER,
HOW_TO_PLAY,
HIGH_SCORE
};

int menuOption = 0; // menu 0: Start, 1: High Score, 2: Cách chơi, 3: Exit

int main(int argc, char* args[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Tạo cửa sổ game
    SDL_Window* window = SDL_CreateWindow("Racing Attack", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    return -1;
}

     TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return -1;
    }
    // Màu nền
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);

    // Tọa độ xe của người chơi
    int carX = SCREEN_WIDTH / 2 - CAR_WIDTH / 2;
    int carY = SCREEN_HEIGHT - CAR_HEIGHT - 10;

    // Tọa độ chướng ngại vật
    srand(time(NULL));
    int obstacleX = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
    int obstacleY = -OBSTACLE_HEIGHT;
    int powerX = rand() % (SCREEN_WIDTH - 30);
    int powerY = -200;
    bool powerVisible = true;
    bool isSlowed = false;
    Uint32 slowStartTime = 0;


    // Tốc độ ban đầu của chướng ngại vật
    float obstacleSpeed = 1.0; // Tốc độ khởi đầu
    float speedIncreaseRate = 0.0015; // Tốc độ tăng thêm sau mỗi frame
    int score = 0;
    int highscore = 0;
    FILE* f = fopen("highscore.txt","r");
    if(f){
        fscanf(f, "%d", &highscore);
        fclose(f);
    }
    Gamestate state = MENU;

    // Vòng lặp game
    bool quit = false;
    SDL_Event e;
    SDL_Surface* surface;
    SDL_Texture* texture;
    SDL_Rect rect;
    while (!quit) {
        // Xử lý sự kiện
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (state == MENU && e.type == SDL_KEYDOWN){
                    if(e.key.keysym.sym == SDLK_UP){
                        menuOption = (menuOption -1 +4) %4;
                        continue;
                    }
                    if(e.key.keysym.sym == SDLK_DOWN){
                        menuOption = (menuOption + 1)%4;
                        continue;
                    }
            if (e.key.keysym.sym == SDLK_RETURN){
                    if(menuOption == 0){
                state = PLAYING;
                obstacleY = -OBSTACLE_HEIGHT;
                carX = SCREEN_WIDTH/2 - CAR_WIDTH/2;
                carY = SCREEN_HEIGHT - CAR_HEIGHT -10;
                obstacleSpeed = 0.005;
                score=0;
            }
            else if (menuOption == 1){
                    state = HIGH_SCORE;
            } else if (menuOption == 2){
                state = HOW_TO_PLAY;
            } else if(menuOption == 3){
             quit = true;
             }
             continue;
            }
            }


            if (state == GAME_OVER && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    state = MENU;
                    continue;

                }
            }
            if (state == HOW_TO_PLAY && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                state = MENU;
                continue;
            }
            }
            if (state == HIGH_SCORE && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                state = MENU;
                continue;
    }
}
        }
           if (state == HIGH_SCORE) {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // nền đen
                SDL_RenderClear(renderer);

                SDL_Color white = {255, 255, 255};

               char hsText[64];
            sprintf(hsText, "Highest Score: %d", highscore);
            surface = TTF_RenderText_Solid(font, hsText, white);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            rect = {50, 200, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

    surface = TTF_RenderText_Solid(font, "Press Enter to return", white);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect = {50, 250, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    SDL_RenderPresent(renderer);
    continue;
}

            if (state == MENU) {

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // neenf dden

            SDL_RenderClear(renderer);

            SDL_Color white = {255, 255, 255}; // mauf chuwx tranwsg

            surface = TTF_RenderText_Solid(font, "Racing Attack - PRESS ENTER TO START =)) ", white);

            texture = SDL_CreateTextureFromSurface(renderer, surface);

            rect = {50, 100, surface->w, surface->h};

            SDL_RenderCopy(renderer, texture, NULL, &rect);

            SDL_FreeSurface(surface);

            SDL_DestroyTexture(texture);

            const char* menuItems[] = {"START", "HIGHEST SCORE", "HOW TO PLAY", "Exit"};
     for (int i = 0; i < 4; ++i) {
            SDL_Color color = (i == menuOption) ? SDL_Color{255, 0, 0} : white;  // Mục đang chọn màu đỏ
            surface = TTF_RenderText_Solid(font, menuItems[i], color);

            texture = SDL_CreateTextureFromSurface(renderer, surface);
            rect = {SCREEN_WIDTH / 4, 200 + i * 50, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }

            SDL_RenderPresent(renderer);
        }

        if (state == HOW_TO_PLAY) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Màu nền đen
        SDL_RenderClear(renderer);  // Dọn sạch màn hình

        SDL_Color white = {255, 255, 255};  // Màu chữ trắng

        // Hiển thị cách chơi
        const char* howToPlayLines[] ={
              "Use arrow up, down, left, right to move the car",
              "Try to advoid the obstacles and use the power-ups",
              "to get the highest score",
              "Press Enter to return to Menu."
        } ;
        for (int i = 0; i < 4; ++i) {
          surface = TTF_RenderText_Solid(font, howToPlayLines[i], white);
          texture = SDL_CreateTextureFromSurface(renderer, surface);
          rect = {50, 100 + i * 50, surface->w, surface->h};
          SDL_RenderCopy(renderer, texture, NULL, &rect);
          SDL_FreeSurface(surface);
          SDL_DestroyTexture(texture);
}

        SDL_RenderPresent(renderer);
        continue;
    }
}

        if (state == PLAYING) {
            const Uint8* keystates = SDL_GetKeyboardState(NULL);
            if (keystates[SDL_SCANCODE_LEFT]) carX -= 5;
            if (keystates[SDL_SCANCODE_RIGHT]) carX += 5;
            if (keystates[SDL_SCANCODE_UP]) carY -= 5;
            if (keystates[SDL_SCANCODE_DOWN]) carY += 5;
             //giới hạn xe
            if (carX < 0) carX = 0;
            if (carX > SCREEN_WIDTH - CAR_WIDTH) carX = SCREEN_WIDTH - CAR_WIDTH;
            if (carY < 0) carY = 0;
            if (carY > SCREEN_HEIGHT - CAR_HEIGHT) carY = SCREEN_HEIGHT - CAR_HEIGHT;

           // Cập nhật vị trí CNV và Power-Up
           obstacleY += obstacleSpeed;

            if (powerVisible) {
                powerY += 2 ;

            if (powerY > SCREEN_HEIGHT) {
            powerVisible = false;
    }
}
             else {
    // Cơ hội xuất hiện lại ngẫu nhiên
             if (rand() % 1000 < 5) {
        powerVisible = true;
        powerX = rand() % (SCREEN_WIDTH - 30);
        powerY = -200;
    }
}

// Tăng tốc độ dần theo thời gian
obstacleSpeed += speedIncreaseRate;

// Reset chướng ngại vật nếu đi qua màn hình
if (obstacleY > SCREEN_HEIGHT) {
    obstacleY = -OBSTACLE_HEIGHT;
    obstacleX = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
    score++;
}

// Kiểm tra va chạm với chướng ngại vật
if (carX < obstacleX + OBSTACLE_WIDTH &&
    carX + CAR_WIDTH > obstacleX &&
    carY < obstacleY + OBSTACLE_HEIGHT &&
    carY + CAR_HEIGHT > obstacleY) {
    if (score > highscore) {
        highscore = score;
        FILE* f = fopen("highscore.txt", "w");
        if (f) { fprintf(f, "%d", highscore); fclose(f); }
    }
    state = GAME_OVER;
}

// Va chạm với Power-Up
if (powerVisible &&
    carX < powerX + 30 &&
    carX + CAR_WIDTH > powerX &&
    carY < powerY + 30 &&
    carY + CAR_HEIGHT > powerY) {

    obstacleSpeed *= 0.5; // Giảm tốc độ còn một nửa
    isSlowed = true;
    slowStartTime = SDL_GetTicks();
    powerVisible = false;
}

// Sau 3 giây, khôi phục tốc độ
if (isSlowed && SDL_GetTicks() - slowStartTime >= 3000) {
    obstacleSpeed *= 2;
    isSlowed = false;
}


            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // Màu trắng

            int dashHeight = 20;
            int dashSpacing = 20;
            int dashWidth = 4;
            int laneWidth = SCREEN_WIDTH / 3;

      for (int i = 1; i < 3; ++i) {  // vẽ làn
      int x = i * laneWidth;


      for (int y = 0; y < SCREEN_HEIGHT; y += dashHeight + dashSpacing) {
        SDL_Rect dash = {x - dashWidth / 2, y, dashWidth, dashHeight};
        SDL_RenderFillRect(renderer, &dash);
    }
}

      // xe
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect car = {carX, carY, CAR_WIDTH, CAR_HEIGHT};
            SDL_RenderFillRect(renderer, &car);
      // CNV
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_Rect obstacle = {obstacleX, obstacleY, OBSTACLE_WIDTH, OBSTACLE_HEIGHT};
            SDL_RenderFillRect(renderer, &obstacle);
        if (powerVisible) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // vàng
            SDL_Rect power = {powerX, powerY, 30, 30};
            SDL_RenderFillRect(renderer, &power);
}
            // Hiển thị điểm số
            char scoreText[64];
            sprintf(scoreText, "Score: %d", score);
          SDL_Color white = {255, 255, 255};
          SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, white);
          SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
          SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
          SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
          SDL_FreeSurface(scoreSurface);
          SDL_DestroyTexture(scoreTexture);


            SDL_RenderPresent(renderer);
            SDL_Delay(16);


        }

        if (state == GAME_OVER) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Color red = {255, 0, 0};
            SDL_Surface* surface = TTF_RenderText_Solid(font, "Game Over - Press Enter to return", red);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {50, 200, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_FreeSurface(surface);

            SDL_RenderPresent(renderer);


        }


    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
// 1234567



