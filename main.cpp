#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL_mixer.h>

// Kích thước cửa sổ game
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Kích thước xe và chướng ngại vật
const int CAR_WIDTH = 25;
const int CAR_HEIGHT = 50;
const int OBSTACLE_WIDTH = 35;
const int OBSTACLE_HEIGHT = 55;
Mix_Music* bgMusic = NULL;       // Nhạc nền
Mix_Chunk* crashSound = NULL;    // Âm thanh va chạm
Mix_Chunk* powerupSound = NULL;  // Âm thanh ăn item

enum Gamestate{
MENU,
PLAYING,
GAME_OVER,
HOW_TO_PLAY,
HIGH_SCORE
};

int menuOption = 0; // menu 0: Start, 1: High Score, 2: Cách chơi, 3: Reset highscore, 4: Exit

int main(int argc, char* args[]) {
    // Khởi tạo SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    printf("SDL could not initialize! Error: %s\n", SDL_GetError());
    return -1;
    }
// Khởi tạo SDL_mixer (44100Hz, stereo, 2048 sample buffer)
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    printf("SDL_mixer could not initialize! Error: %s\n", Mix_GetError());
    SDL_Quit();
    return -1;
}
  bgMusic = Mix_LoadMUS("music/Megalovania.mp3");  // Nhạc nền (nếu có)
if (!bgMusic) {
    printf("Failed to load music: %s\n", Mix_GetError());
} else{
    Mix_PlayMusic(bgMusic, -1);  // -1 = lặp vô hạn
    Mix_VolumeMusic(30);         // Âm lượng (0-128)
}
crashSound = Mix_LoadWAV("sounds/crash.wav");    // Âm thanh va chạm
powerupSound = Mix_LoadWAV("sounds/powerup.wav");// Âm thanh ăn item

    // Tạo cửa sổ game
    SDL_Window* window = SDL_CreateWindow("Racing Attack", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        Mix_CloseAudio();
        SDL_Quit();
        return -1;
    }

    // Tạo renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        SDL_Quit();
        return -1;
    }
    //tạo SDL_ttf
    if (TTF_Init() == -1) {
    printf("TTF_Init Error: %s\n", TTF_GetError());
    SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        SDL_Quit();
    return -1;
}

     TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_CloseAudio();
        SDL_Quit();
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
    float obstacleSpeed = 0.025; // Tốc độ khởi đầu
    const float speedIncreaseRate = 0.0005; // Tốc độ tăng thêm sau mỗi frame
    const float maxObstacleSpeed = 5.0; //max speed
    int score = 0;
    int highscore = 0;
    FILE* f = fopen("highscore.txt","r");
    if(f != NULL ){
        fscanf(f, "%d", &highscore);
        fclose(f);
    }
    enum Gamestate state = MENU;

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
            switch (state){
                case MENU:
            if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_UP) {
                        menuOption = (menuOption - 1 + 5) % 5;
                    }
                    if (e.key.keysym.sym == SDLK_DOWN) {
                        menuOption = (menuOption + 1) % 5;
                    }
            if (e.key.keysym.sym == SDLK_RETURN){
                    switch(menuOption) {
            case 0: {// START
                state = PLAYING;
                obstacleY = -OBSTACLE_HEIGHT;
                carX = SCREEN_WIDTH/2 - CAR_WIDTH/2;
                carY = SCREEN_HEIGHT - CAR_HEIGHT -10;
                obstacleSpeed = 0.025;
                score = 0;
                powerVisible = true;
                powerY = -200;
                break;
            }
            case 1: {// HIGH SCORE
                state = HIGH_SCORE;
                break;
            }
            case 2:{ // HOW TO PLAY
                state = HOW_TO_PLAY;
                break;
            }
            case 3:{ // RESET HIGH SCORE
                highscore = 0;
                f = fopen("highscore.txt", "w");
                if (f) {
                    fprintf(f, "%d", highscore);
                    fclose(f);
                }
                break;
            }
            case 4: {// EXIT
                quit = true;
                break;
        }
    }
}
      break;
      case GAME_OVER:
      case HOW_TO_PLAY:
      case HIGH_SCORE:
         if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                    state = MENU;
                }
                break;
      case PLAYING:
        break;
            }
        }
            if (state == GAME_OVER && e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_RETURN) {
                    state = MENU;
                }
            }
            if (state == HOW_TO_PLAY && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                state = MENU;
            }
            if (state == HIGH_SCORE && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                state = MENU;
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
            const char* menuItems[] = {"START", "HIGHEST SCORE", "HOW TO PLAY", "RESET HIGH SCORE", "Exit"};
     for (int i = 0; i < 5; ++i) {
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
         else if (state == HIGH_SCORE) {
            SDL_SetRenderDrawColor(renderer, 0,0,0,255);
            SDL_RenderClear(renderer);
            SDL_Color white = {255,255,255,255};

            char hsText[64];
            snprintf(hsText, sizeof(hsText), "Highest Score: %d", highscore);
            SDL_Surface* surface = TTF_RenderText_Solid(font, hsText, white);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = { (SCREEN_WIDTH - surface->w)/2, SCREEN_HEIGHT/2 - surface->h, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            const char* prompt = "Press Enter to return";
            surface = TTF_RenderText_Solid(font, prompt, white);
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            rect.x = (SCREEN_WIDTH - surface->w)/2;
            rect.y = SCREEN_HEIGHT/2 + 20;
            rect.w = surface->w;
            rect.h = surface->h;
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            SDL_RenderPresent(renderer);
        }
        else if (state == HOW_TO_PLAY) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Màu nền đen
        SDL_RenderClear(renderer);  // Dọn sạch màn hình
        SDL_Color white = {255, 255, 255};  // Màu chữ trắng
        // Hiển thị cách chơi
        const char* howToPlayLines[] ={
              "Use arrow up, down, left, right to move the car",
              "Try to avoid the obstacles and use the power-ups",
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

    }
        else if (state == PLAYING) {
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
            if (obstacleY > SCREEN_HEIGHT) {
                obstacleY = -OBSTACLE_HEIGHT;
                obstacleX = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
                score++;
                // Tăng speed nhưng không vượt quá max
                obstacleSpeed += speedIncreaseRate;
                if (obstacleSpeed > maxObstacleSpeed) obstacleSpeed = maxObstacleSpeed;
            }
            if (powerVisible) {
                powerY += 2;
                if (powerY > SCREEN_HEIGHT) {
                    powerVisible = false;
                }
            }
             else {
    //xuất hiện lại ngẫu nhiên
             if (rand() % 1000 < 5) {
        powerVisible = true;
        powerX = rand() % (SCREEN_WIDTH - 30);
        powerY = -200;
    }
             }

// Kiểm tra va chạm với chướng ngại vật
if (carX < obstacleX + OBSTACLE_WIDTH &&
    carX + CAR_WIDTH > obstacleX &&
    carY < obstacleY + OBSTACLE_HEIGHT &&
    carY + CAR_HEIGHT > obstacleY) {
    if (crashSound) {
        Mix_PlayChannel(-1, crashSound, 0);  // Phát âm thanh va chạm
    }
    if (score > highscore) {
        highscore = score;
        f = fopen("highscore.txt", "w");
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
    if (powerupSound) {
        Mix_PlayChannel(-1, powerupSound, 0);  // Phát âm thanh ăn item
    }

    obstacleSpeed *= 0.005; // Giảm tốc độ còn một nửa
    isSlowed = true;
    slowStartTime = SDL_GetTicks();
    powerVisible = false;
}

// Sau 3.5 giây, khôi phục tốc độ
if (isSlowed && SDL_GetTicks() - slowStartTime >= 3500) {
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

          char hsText[64];
            snprintf(hsText, sizeof(hsText), "High Score: %d", highscore);
            SDL_Surface* hsSurface = TTF_RenderText_Solid(font, hsText, white);
            SDL_Texture* hsTexture = SDL_CreateTextureFromSurface(renderer, hsSurface);
            SDL_Rect hsRect = { SCREEN_WIDTH - hsSurface->w - 10, 10, hsSurface->w, hsSurface->h };
            SDL_RenderCopy(renderer, hsTexture, NULL, &hsRect);
            SDL_FreeSurface(hsSurface);
            SDL_DestroyTexture(hsTexture);

            SDL_RenderPresent(renderer);
        }

        else if (state == GAME_OVER) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_Color red = {255, 0, 0};
            SDL_Surface* surface = TTF_RenderText_Solid(font, "Game Over - Press Enter to return", red);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect rect = {50, 200, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);

            SDL_RenderPresent(renderer);


        }
            SDL_Delay(10);

    }
    }
    TTF_CloseFont(font);
    TTF_Quit();
    if (bgMusic) Mix_FreeMusic(bgMusic);
    if (crashSound) Mix_FreeChunk(crashSound);
    if (powerupSound) Mix_FreeChunk(powerupSound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

// 1234567



