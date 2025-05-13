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
Mix_Music* bgMusic = NULL;        // Nhạc nền
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

// Hàm để tạo chướng ngại vật
SDL_Rect createObstacle(int x, int y) {
    SDL_Rect obstacle;
    obstacle.x = x;
    obstacle.y = y;
    obstacle.w = OBSTACLE_WIDTH;
    obstacle.h = OBSTACLE_HEIGHT;
    return obstacle;
}

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
    bgMusic = Mix_LoadMUS("C:/=))/DemoSDL/music/background.wav");  // Nhạc nền (nếu có)
    if (!bgMusic) {
        printf("Failed to load music: %s\n", Mix_GetError());
    } else{
        Mix_PlayMusic(bgMusic, -1);    // -1 = lặp vô hạn
        Mix_VolumeMusic(30);          // Âm lượng (0-128)
    }
    crashSound = Mix_LoadWAV("C:/=))/DemoSDL/sound/crash.wav");    // Âm thanh va chạm
    powerupSound = Mix_LoadWAV("C:/=))/DemoSDL/sound/item.wav");// Âm thanh ăn item

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
        printf("TTF_Init Error: %s\n", SDL_GetError());
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
    // Tạo mảng chướng ngại vật
    const int MAX_OBSTACLES = 5; // Số lượng chướng ngại vật tối đa
    SDL_Rect obstacles[MAX_OBSTACLES];
    int obstacleX[MAX_OBSTACLES];
    int obstacleY[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacleX[i] = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
        obstacleY[i] = -OBSTACLE_HEIGHT - rand() % 200; // Tạo độ cao ngẫu nhiên
        obstacles[i] = createObstacle(obstacleX[i], obstacleY[i]);
    }

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

    Uint32 lastTime = SDL_GetTicks();
    float deltaTime = 0;
    while (!quit) {
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        // Xử lý sự kiện
        Uint32 currentTime = SDL_GetTicks();
        deltaTime = (currentTime - lastTime) / 1000.0f; // Chuyển sang giây
        lastTime = currentTime;

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        switch (state) {
            case MENU: {
                // Xử lý sự kiện bàn phím cho MENU trực tiếp trong vòng lặp chính
                static bool upKeyPressed = false;
                static bool downKeyPressed = false;
                static bool returnKeyPressed = false;
                SDL_Color white = {255, 255, 255}; // mauf chuwx tranwsg
                const char* menuItems[] = {"START", "HIGHEST SCORE", "HOW TO PLAY", "RESET HIGH SCORE", "Exit"};
                if (keystates[SDL_SCANCODE_UP]) {
                    // Kiểm tra xem có phím UP đang được giữ không, tránh thay đổi quá nhanh

                    if (!upKeyPressed) {
                        menuOption = (menuOption - 1 + 5) % 5;
                        printf("Menu option: %d\n", menuOption);
                        upKeyPressed = true;
                    }
                } else {
                    upKeyPressed = false;
                }

                if (keystates[SDL_SCANCODE_DOWN]) {
                    // Tương tự cho phím DOWN

                    if (!downKeyPressed) {
                        menuOption = (menuOption + 1) % 5;
                        printf("Menu option: %d\n", menuOption);
                        downKeyPressed = true;
                    }
                } else {
                    downKeyPressed = false;
                }

                if (keystates[SDL_SCANCODE_RETURN]) {
                    // Xử lý phím ENTER

                    if (!returnKeyPressed) {
                        printf("Enter pressed on option: %d\n", menuOption);
                        switch (menuOption) {
                            case 0:
                                state = PLAYING;
                                 // Reset game state
                                for (int i = 0; i < MAX_OBSTACLES; i++) {
                                    obstacleY[i] = -OBSTACLE_HEIGHT - rand() % 200; // Khởi tạo vị trí chướng ngại vật
                                    obstacleX[i] = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
                                    obstacles[i] = createObstacle(obstacleX[i], obstacleY[i]);
                                }
                                carX = SCREEN_WIDTH/2 - CAR_WIDTH/2;
                                carY = SCREEN_HEIGHT - CAR_HEIGHT - 10;
                                obstacleSpeed = 0.025;
                                score = 0;
                                powerVisible = true;
                                powerY = -200;
                                break;
                            case 1:
                                state = HIGH_SCORE;
                                break;
                            case 2:
                                state = HOW_TO_PLAY;
                                break;
                            case 3:
                                 highscore = 0;
                                 f = fopen("highscore.txt", "w");
                                 if (f) {
                                     fprintf(f, "%d", highscore);
                                     fclose(f);
                                 }
                                break;
                            case 4:
                                quit = true;
                                break;
                        }
                        returnKeyPressed = true;
                    }
                } else {
                    returnKeyPressed = false;
                }
                // Vẽ menu
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // neenf dden
                SDL_RenderClear(renderer);

                surface = TTF_RenderText_Solid(font, "Racing Attack - PRESS ENTER TO START =)) ", white);

                texture = SDL_CreateTextureFromSurface(renderer, surface);

                rect = {50, 100, surface->w, surface->h};

                SDL_RenderCopy(renderer, texture, NULL, &rect);

                SDL_FreeSurface(surface);

                SDL_DestroyTexture(texture);

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
                break;
            }
            case GAME_OVER: {
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
                SDL_Color red = {255, 0, 0};
                surface = TTF_RenderText_Solid(font, "Game Over - Press Enter to return", red);
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                rect = {50, 200, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, NULL, &rect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);

                SDL_RenderPresent(renderer);
                if (keystates[SDL_SCANCODE_RETURN])
                    state = MENU;
                break;
            }
            case HOW_TO_PLAY: {
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
                if (keystates[SDL_SCANCODE_RETURN])
                    state = MENU;
                break;
            }
            case HIGH_SCORE: {
                SDL_SetRenderDrawColor(renderer, 0,0,0,255);
                SDL_RenderClear(renderer);
                SDL_Color white = {255,255,255,255};
                char hsText[64];
                snprintf(hsText, sizeof(hsText), "Highest Score: %d", highscore);
                surface = TTF_RenderText_Solid(font, hsText, white);
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                rect = { (SCREEN_WIDTH - surface->w)/2, SCREEN_HEIGHT/2 - surface->h, surface->w, surface->h };
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
                if (keystates[SDL_SCANCODE_RETURN])
                    state = MENU;
                break;
            }
            case PLAYING:
                const float playerSpeed = 300.0f; // pixels/second
                if (keystates[SDL_SCANCODE_LEFT]) carX -= playerSpeed * deltaTime;
                if (keystates[SDL_SCANCODE_RIGHT]) carX += playerSpeed * deltaTime;
                if (keystates[SDL_SCANCODE_UP]) carY -= playerSpeed * deltaTime;
                if (keystates[SDL_SCANCODE_DOWN]) carY += playerSpeed * deltaTime;
                 //giới hạn xe
                if (carX < 0) carX = 0;
                if (carX > SCREEN_WIDTH - CAR_WIDTH) carX = SCREEN_WIDTH - CAR_WIDTH;
                if (carY < 0) carY = 0;
                if (carY > SCREEN_HEIGHT - CAR_HEIGHT) carY = SCREEN_HEIGHT - CAR_HEIGHT;

                // Cập nhật vị trí CNV và Power-Up
                float baseObstacleSpeed = 100.0f;  // Tốc độ cơ bản (pixel/giây)

                for (int i = 0; i < MAX_OBSTACLES; i++)
                {
                    obstacleY[i] += baseObstacleSpeed * deltaTime * (isSlowed ? 0.5f : 1.0f);
                    obstacles[i].y = obstacleY[i]; // Cập nhật vị trí của chướng ngại vật

                    if (obstacleY[i] > SCREEN_HEIGHT) {
                        obstacleY[i] = -OBSTACLE_HEIGHT - rand() % 200;
                        obstacleX[i] = rand() % (SCREEN_WIDTH - OBSTACLE_WIDTH);
                        obstacles[i].x = obstacleX[i];
                        obstacles[i].y = obstacleY[i];
                        score++;
                         // Tăng speed nhưng không vượt quá max
                        const float speedIncreasePerSecond = 10.0f;  // Tăng 10 pixel/giây mỗi giây
                        if (!isSlowed) {
                            obstacleSpeed += speedIncreasePerSecond * deltaTime; // Tăng tốc độ chướng ngại vật
                            if (obstacleSpeed > maxObstacleSpeed) obstacleSpeed = maxObstacleSpeed;
                        }
                    }
                }
                const float powerUpSpeed = 150.0f;  // Tốc độ rơi (pixel/giây)
                if (powerVisible) {
                    powerY += powerUpSpeed * deltaTime;
                }
                else {
             //xuất hiện lại ngẫu nhiên
                    if (rand() % 500 < 5) { // Thay đổi tần suất xuất hiện, càng nhỏ càng nhiều
                        powerVisible = true;
                        powerX = rand() % (SCREEN_WIDTH - 30);
                        powerY = -200;
                    }
                }

// Kiểm tra va chạm với chướng ngại vật
for (int i = 0; i < MAX_OBSTACLES; i++) {
    if (carX < obstacles[i].x + OBSTACLE_WIDTH &&
        carX + CAR_WIDTH > obstacles[i].x &&
        carY < obstacles[i].y + OBSTACLE_HEIGHT &&
        carY + CAR_HEIGHT > obstacles[i].y) {
        if (crashSound) {
            Mix_PlayChannel(-1, crashSound, 0);  // Phát âm thanh va chạm
        }
        if (score > highscore) {
            highscore = score;
            f = fopen("highscore.txt", "w");
            if (f) { fprintf(f, "%d", highscore); fclose(f); }
        }
        state = GAME_OVER;
        break; // Dừng kiểm tra va chạm sau va chạm đầu tiên
    }
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
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);   // Màu trắng

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
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    SDL_RenderFillRect(renderer, &obstacles[i]);
                }
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
                break;
        } // End switch(state)
        SDL_Delay(10);
    } // End while (!quit)
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

