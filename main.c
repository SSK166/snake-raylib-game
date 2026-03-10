#include <raylib.h>
#include <accounts.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MAX_FRUITS 5
#define MAX_BACKGROUNDS 6
Texture2D backgroundTextures[MAX_BACKGROUNDS];
int selectedBackground = 0;



#define WIDTH 50
#define HEIGHT 40
#define CELL_SIZE 22
#define LEADERBOARD_SIZE 10

//leaderboard variables
char leaderboardUsernames[LEADERBOARD_SIZE][50];
int leaderboardScores[LEADERBOARD_SIZE];
int leaderboardCount = 0;


int screenWidth = WIDTH * CELL_SIZE;
int screenHeight = HEIGHT * CELL_SIZE + 100;



//  Snake variables
int x, y, fruitX, fruitY, score, highscore;
int tailX[100], tailY[100];
int nTail;
int dir;

// Game variables
float cur_time;
float init_time;
float lvlt = 60.0f;
float mul = 1.0f;
float gameSpeed = 1.0f;
int FRAME_RATE = 60;
char cl = '1';
int gameOver = 0;
bool exit_game = false;

// Skin variables
Color headColor = DARKGRAY;
Color bodyColor = LIGHTGRAY;
Color bgColor = RAYWHITE;
Color borderColor = BLACK;
char bodyShape = 'o';

// Sound variables
Sound eatSound;
Sound gameOverSound;
Music bgMusic;
Texture2D backgroundTexture;



// UI states
typedef enum
{
    LOGIN_SCREEN,
    CREATE_ACCOUNT,
    ACCOUNT_MENU,
    MAIN_MENU,
    SELECT_FRUIT_SCREEN,
    SELECT_BACKGROUND_SCREEN,
    SELECT_LEADERBOARD,
    GAMEPLAY
} GameState;

GameState currentScreen = ACCOUNT_MENU;

// UI Colors
Color buttonColor = LIGHTGRAY;
Color buttonHoverColor = GRAY;
Color buttonTextColor = BLACK;
Color highlightColor = BLUE;
Color errorColor = RED;

// Menu buttons
typedef struct Button
{
    Rectangle rect;
    char text[50];
    bool isHovered;
    Color color;

} Button;

// Account menu buttons
Button loginButton = {0};
Button createAccountButton = {0};
Button exitButton = {0};

// Main menu buttons
Button startGameButton = {0};
Button changeLevelButton = {0};
Button selectFruitButton = {0};
Button changeBackgroundButton={0};
Button showLeaderboard = {0};
Button logoutButton = {0};

// Login/Create Account fields
Rectangle usernameField = {0};
Rectangle passwordField = {0};
bool isUsernameFocused = false;
bool isPasswordFocused = false;
char username[50] = {0};
char password[50] = {0};
int usernameLetterCount = 0;
int passwordLetterCount = 0;
char passwordMask[50] = {0};
bool showErrorMessage = false;
char errorMessage[100] = {0};

// Select fruits
Texture2D fruitTextures[MAX_FRUITS];
int selectedFruit = 0;

// Function Declarations
void InitializeUI();
void GenerateFruit();
void Setup();
void UpdateGameState();
void SetLevel();
void ResetGame();
void DrawAccountMenu();
void DrawLoginScreen();
void DrawCreateAccountScreen();
void DrawMainMenu();
void DrawSelectBackgroundScreen();
void ShowLeaderBoard();
void DrawTextBox(Rectangle rec, char *text, int letterCount, bool isMasked, bool isFocused);
void ProcessTextInput(char *text, int *letterCount, int maxLetters);
bool CheckCollisionPointRec(Vector2 point, Rectangle rec);

// ---------------- Functions ----------------

void InitializeUI()
{
    // Account menu buttons
    loginButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 - 60, 300, 50},
        "Login",
        false,
        buttonColor};

    createAccountButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2, 300, 50},
        "Create Account",
        false,
        buttonColor};

    exitButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 + 60, 300, 50},
        "Exit",
        false,
        buttonColor};

    // Main menu buttons
    startGameButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 - 60, 300, 50},
        "Start Game",
        false,
        buttonColor};
    
        

    changeLevelButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2, 300, 50},
        "Change Level (Current: 1)",
        false,
        buttonColor};

    logoutButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 + 60, 300, 50},
        "Logout",
        false,
        buttonColor};

    selectFruitButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 + 120, 300, 50},
        "Select Fruit",
        false,
        buttonColor};
    
    changeBackgroundButton = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 + 180, 300, 50},
        "Change Background",
        false,
        buttonColor
    };
   showLeaderboard = (Button){
        (Rectangle){screenWidth / 2 - 150, screenHeight / 2 + 240, 300, 50},
        "View the Leaderboard",
        false,
        buttonColor
    };

    // Text input fields
    usernameField = (Rectangle){screenWidth / 2 - 150, screenHeight / 2 - 60, 300, 40};
    passwordField = (Rectangle){screenWidth / 2 - 150, screenHeight / 2, 300, 40};
}

void GenerateFruit()
{
    fruitX = GetRandomValue(0, WIDTH - 1);
    fruitY = GetRandomValue(0, HEIGHT - 1);

    do
    {
        fruitX = GetRandomValue(0, WIDTH - 1);
        fruitY = GetRandomValue(0, HEIGHT - 1);
    } while ((fruitX == x && fruitY == y));

    int i = 0;
    do
    {
        fruitX = GetRandomValue(0, WIDTH - 1);
        fruitY = GetRandomValue(0, HEIGHT - 1);
        i++;
    } while (tailX[i] == fruitX && tailY[i] == fruitY);
}

void Setup()
{
    x = WIDTH / 2;
    y = HEIGHT / 2;
    score = 0;
    dir = 2;
    nTail = 0;
    GenerateFruit();
    init_time = GetTime();
    highscore = Scorer();
    gameOver = 0;
}

void UpdateGameState()
{
    int prevX = x;
    int prevY = y;
    int prev2X, prev2Y;
    for (int i = 0; i < nTail; i++)
    {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    if (dir == 1)
        x--;
    else if (dir == 2)
        x++;
    else if (dir == 3)
        y--;
    else if (dir == 4)
        y++;

    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
    {
        gameOver = 1;
        PlaySound(gameOverSound);
    }

    for (int i = 0; i < nTail; i++)
    {
        if (tailX[i] == x && tailY[i] == y)
        {
            gameOver = 1;
            PlaySound(gameOverSound);
        }
    }

    if (x == fruitX && y == fruitY)
    {
        score += 10;
        GenerateFruit();
        tailX[nTail] = x;
        tailY[nTail] = y;
        nTail++;
        highscore = Scorer();
        PlaySound(eatSound);
    }
}

void SetLevel()
{
    switch (cl)
    {
    case '1':
        gameSpeed = 0.5;
        lvlt = 60.0f;
        // FRAME_RATE = 60;
        mul = 1.0f;
        break;
    case '2':
        gameSpeed = 0.75;
        lvlt = 45.0f;
        // FRAME_RATE = 60;
        mul = 1.2f;
        break;
    case '3':
        gameSpeed = 1.0;
        lvlt = 30.0f;
        // FRAME_RATE = 60;
        mul = 1.5f;
        break;
    case '4':
        gameSpeed = 1.25;
        lvlt = 9999.0f;
        // FRAME_RATE = 60;
        mul = 0.8f;
        break;
    default:
        break;
    }

    // Update level button text
    sprintf(changeLevelButton.text, "Change Level (Current: %c)", cl);
}

void ResetGame()
{
    Setup();
}

void DrawSelectBackgroundScreen()
{
    DrawTextureEx(backgroundTextures[selectedBackground], (Vector2){0, 0}, 0.0f, 
                  fmax((float)screenWidth / backgroundTextures[selectedBackground].width, 
                       (float)screenHeight / backgroundTextures[selectedBackground].height), WHITE);

    DrawText("Select Background", screenWidth / 2 - 120, 40, 30, DARKBLUE);

    Vector2 mousePoint = GetMousePosition();
    int startX = screenWidth / 2 - (MAX_BACKGROUNDS)*70 / 2;
    int y = screenHeight / 2 - 40;

    for (int i = 1; i < MAX_BACKGROUNDS-1; i++)
    {
        Rectangle bgRec = {startX + i * 70, y, 60, 60};
        if (CheckCollisionPointRec(mousePoint, bgRec))
        {
            DrawRectangleLinesEx(bgRec, 3, highlightColor);
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                selectedBackground = i;
            }
        }
        DrawTextureEx(backgroundTextures[i], (Vector2){bgRec.x, bgRec.y}, 0.0f, 60.0f / backgroundTextures[i].width, WHITE);
    }

    // Back button
    Rectangle confirmButton = {screenWidth / 2 - 75, screenHeight - 100, 150, 40};
    bool confirmHovered = CheckCollisionPointRec(mousePoint, confirmButton);
    DrawRectangleRec(confirmButton, confirmHovered ? buttonHoverColor : buttonColor);
    DrawText("Back to Menu", confirmButton.x + confirmButton.width / 2 - MeasureText("Back to Menu", 20) / 2,
             confirmButton.y + 10, 20, buttonTextColor);

    if (confirmHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        currentScreen = MAIN_MENU;
    }
}

void ShowLeaderBoard() {
    
    DrawTextureEx(backgroundTextures[5], (Vector2){0, 0}, 0.0f, // Desert background
                  fmax((float)screenWidth / backgroundTextures[5].width, 
                      (float)screenHeight / backgroundTextures[5].height), WHITE);

    
    DrawText("LEADERBOARD", screenWidth/2 - 120, 40, 40, DARKBLUE);
    
    
    int startY = 100;
    for (int i = 0; i < leaderboardCount; i++) {
        DrawText(TextFormat("%d. %s - %d", i+1, leaderboardUsernames[i], leaderboardScores[i]),
                screenWidth/2 - 200, // Centered X position
                startY + i*40, // Proper spacing
                30, 
                (strcmp(username, leaderboardUsernames[i]) == 0) ? WHITE : WHITE);
    }

    //Back button
    Rectangle backBtn = {screenWidth/2 - 75, screenHeight - 100, 150, 40};
    bool hovered = CheckCollisionPointRec(GetMousePosition(), backBtn);
    
    DrawRectangleRec(backBtn, hovered ? GRAY : DARKGRAY);
    DrawText("BACK", backBtn.x+50, backBtn.y+10, 20, WHITE);
    
    if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        currentScreen = MAIN_MENU;
    }
}


void DrawTextBox(Rectangle rec, char *text, int letterCount, bool isMasked, bool isFocused)
{
    Color boxColor = isFocused ? Fade(SKYBLUE, 0.2) : LIGHTGRAY;
    DrawRectangleRec(rec, boxColor);
    DrawRectangleLinesEx(rec, 1, isFocused ? BLUE : GRAY);

    if (isMasked)
    {
        for (int i = 0; i < letterCount; i++)
        {
            passwordMask[i] = '*';
        }
        passwordMask[letterCount] = '\0';
        DrawText(passwordMask, rec.x + 5, rec.y + 10, 20, BLACK);
    }
    else
    {
        DrawText(text, rec.x + 5, rec.y + 10, 20, BLACK);
    }
}

void ProcessTextInput(char *text, int *letterCount, int maxLetters)
{
    int key = GetCharPressed();
    while (key > 0)
    {
        if ((key >= 32) && (key <= 125) && (*letterCount < maxLetters - 1))
        {
            text[*letterCount] = (char)key;
            (*letterCount)++;
            text[*letterCount] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE))
    {
        if (*letterCount > 0)
        {
            (*letterCount)--;
            text[*letterCount] = '\0';
        }
    }
}

void DrawAccountMenu()
{
    // Background Image
    DrawTextureEx(backgroundTextures[0], (Vector2){0, 0}, 0.0f, 
                 fmax((float)screenWidth / backgroundTextures[0].width, 
                     (float)screenHeight / backgroundTextures[0].height), WHITE);
    
    DrawText("Snake Game - Account Menu", screenWidth / 2 - 180, 50, 30, DARKBLUE);

    // Draw buttons
    loginButton.color = loginButton.isHovered ? buttonHoverColor : buttonColor;
    createAccountButton.color = createAccountButton.isHovered ? buttonHoverColor : buttonColor;
    exitButton.color = exitButton.isHovered ? buttonHoverColor : buttonColor;

    DrawRectangleRec(loginButton.rect, loginButton.color);
    DrawRectangleRec(createAccountButton.rect, createAccountButton.color);
    DrawRectangleRec(exitButton.rect, exitButton.color);

    DrawText(loginButton.text, loginButton.rect.x + loginButton.rect.width / 2 - MeasureText(loginButton.text, 20) / 2,
             loginButton.rect.y + 15, 20, buttonTextColor);
    DrawText(createAccountButton.text, createAccountButton.rect.x + createAccountButton.rect.width / 2 - MeasureText(createAccountButton.text, 20) / 2,
             createAccountButton.rect.y + 15, 20, buttonTextColor);
    DrawText(exitButton.text, exitButton.rect.x + exitButton.rect.width / 2 - MeasureText(exitButton.text, 20) / 2,
             exitButton.rect.y + 15, 20, buttonTextColor);
}

void DrawLoginScreen()
{
    // Background Image
    DrawTextureEx(backgroundTextures[0], (Vector2){0, 0}, 0.0f, 
                 fmax((float)screenWidth / backgroundTextures[0].width, 
                     (float)screenHeight / backgroundTextures[0].height), WHITE);

    Color paddingColor = LIGHTGRAY;
    int textPadding = 5;
    int fontSize = 20;

    // Username Text Label
    const char* usernameLabel = "Username:";
    int usernameTextWidth = MeasureText(usernameLabel, fontSize);
    int usernameTextX = screenWidth / 2 - 150;
    int usernameTextY = screenHeight / 2 - 90;
    DrawRectangle(usernameTextX - textPadding, usernameTextY - textPadding,
                  usernameTextWidth + 2 * textPadding, fontSize + 2 * textPadding, paddingColor);
    DrawText(usernameLabel, usernameTextX, usernameTextY, fontSize, BLACK);
    DrawTextBox(usernameField, username, usernameLetterCount, false, isUsernameFocused);

    // Password Text Label
    const char* passwordLabel = "Password:";
    int passwordTextWidth = MeasureText(passwordLabel, fontSize);
    int passwordTextX = screenWidth / 2 - 150;
    int passwordTextY = screenHeight / 2 - 20;
    DrawRectangle(passwordTextX - textPadding, passwordTextY - textPadding,
                  passwordTextWidth + 2 * textPadding, fontSize + 2 * textPadding, paddingColor);
    DrawText(passwordLabel, passwordTextX, passwordTextY, fontSize, BLACK);
    DrawTextBox(passwordField, password, passwordLetterCount, true, isPasswordFocused);

    Rectangle loginButtonRect = {screenWidth / 2 - 150, screenHeight / 2 + 60, 140, 40};
    Rectangle backButtonRect = {screenWidth / 2 + 10, screenHeight / 2 + 60, 140, 40};

    bool isLoginHovered = CheckCollisionPointRec(GetMousePosition(), loginButtonRect);
    bool isBackHovered = CheckCollisionPointRec(GetMousePosition(), backButtonRect);

    DrawRectangleRec(loginButtonRect, isLoginHovered ? buttonHoverColor : buttonColor);
    DrawRectangleRec(backButtonRect, isBackHovered ? buttonHoverColor : buttonColor);

    DrawText("Login", loginButtonRect.x + loginButtonRect.width / 2 - MeasureText("Login", 20) / 2,
             loginButtonRect.y + 10, 20, buttonTextColor);
    DrawText("Back", backButtonRect.x + backButtonRect.width / 2 - MeasureText("Back", 20) / 2,
             backButtonRect.y + 10, 20, buttonTextColor);

    if (showErrorMessage && strlen(errorMessage) > 0)
    {
        DrawText(errorMessage, screenWidth / 2 - 150, screenHeight / 2 + 120, 20, errorColor);
    }
}

void DrawCreateAccountScreen()
{
    // Background Image
    DrawTextureEx(backgroundTextures[0], (Vector2){0, 0}, 0.0f, 
                 fmax((float)screenWidth / backgroundTextures[0].width, 
                     (float)screenHeight / backgroundTextures[0].height), WHITE);

    Color paddingColor = LIGHTGRAY;
    int textPadding = 8;
    int fontSize = 20;
    const char* titleText = "Create New Account";
    int titleFontSize = 30;
    int titleTextWidth = MeasureText(titleText, titleFontSize);
    int titleTextX = screenWidth / 2 - 150;
    int titleTextY = 50;
    
    DrawText(titleText, titleTextX, titleTextY, titleFontSize, DARKBLUE);

    // Username Text Label
    const char* usernameLabel = "Username:";
    int usernameTextWidth = MeasureText(usernameLabel, fontSize);
    int usernameTextX = screenWidth / 2 - 150;
    int usernameTextY = screenHeight / 2 - 90;
    DrawRectangle(usernameTextX - textPadding, usernameTextY - textPadding,
                  usernameTextWidth + 2 * textPadding, fontSize + 2 * textPadding, paddingColor);
    DrawText(usernameLabel, usernameTextX, usernameTextY, fontSize, BLACK);
    DrawTextBox(usernameField, username, usernameLetterCount, false, isUsernameFocused);

    // Password Text Label
    const char* passwordLabel = "Password:";
    int passwordTextWidth = MeasureText(passwordLabel, fontSize);
    int passwordTextX = screenWidth / 2 - 150;
    int passwordTextY = screenHeight / 2 - 20;
    DrawRectangle(passwordTextX - textPadding, passwordTextY - textPadding,
                  passwordTextWidth + 2 * textPadding, fontSize + 2 * textPadding, paddingColor);
    DrawText(passwordLabel, passwordTextX, passwordTextY, fontSize, BLACK);
    DrawTextBox(passwordField, password, passwordLetterCount, true, isPasswordFocused);
    Rectangle createButtonRect = {screenWidth / 2 - 150, screenHeight / 2 + 60, 140, 40};
    Rectangle backButtonRect = {screenWidth / 2 + 10, screenHeight / 2 + 60, 140, 40};

    bool isCreateHovered = CheckCollisionPointRec(GetMousePosition(), createButtonRect);
    bool isBackHovered = CheckCollisionPointRec(GetMousePosition(), backButtonRect);

    DrawRectangleRec(createButtonRect, isCreateHovered ? buttonHoverColor : buttonColor);
    DrawRectangleRec(backButtonRect, isBackHovered ? buttonHoverColor : buttonColor);

    DrawText("Create", createButtonRect.x + createButtonRect.width / 2 - MeasureText("Create", 20) / 2,
             createButtonRect.y + 10, 20, buttonTextColor);
    DrawText("Back", backButtonRect.x + backButtonRect.width / 2 - MeasureText("Back", 20) / 2,
             backButtonRect.y + 10, 20, buttonTextColor);

    if (showErrorMessage && strlen(errorMessage) > 0)
    {
        DrawText(errorMessage, screenWidth / 2 - 150, screenHeight / 2 + 120, 20, errorColor);
    }
}

void DrawMainMenu() {
    //Background Image
    DrawTextureEx(backgroundTextures[0], (Vector2){0, 0}, 0.0f,
                  fmax((float)screenWidth / backgroundTextures[0].width,
                      (float)screenHeight / backgroundTextures[0].height), WHITE);
    

    DrawText("Snake Game - Main Menu", screenWidth / 2 - 170, 50, 30, DARKBLUE);
    DrawText(TextFormat("Welcome, %s!", username), screenWidth / 2 - 150, 100, 20, DARKGRAY);
    DrawText(TextFormat("Highscore: %d", highscore), screenWidth / 2 - 150, 130, 20, DARKGRAY);


    // Draw buttons
    startGameButton.color = startGameButton.isHovered ? buttonHoverColor : buttonColor;
    changeLevelButton.color = changeLevelButton.isHovered ? buttonHoverColor : buttonColor;
    selectFruitButton.color = selectFruitButton.isHovered ? buttonHoverColor : buttonColor;
    changeBackgroundButton.color = changeBackgroundButton.isHovered ? buttonHoverColor : buttonColor;
    showLeaderboard.color = showLeaderboard.isHovered ? buttonHoverColor : buttonColor;   
    logoutButton.color = logoutButton.isHovered ? buttonHoverColor : buttonColor;
    
    DrawRectangleRec(startGameButton.rect, startGameButton.color);
    DrawRectangleRec(changeLevelButton.rect, changeLevelButton.color);
    DrawRectangleRec(selectFruitButton.rect, selectFruitButton.color);
    DrawRectangleRec(changeBackgroundButton.rect, changeBackgroundButton.color);
    DrawRectangleRec(showLeaderboard.rect, showLeaderboard.color);
    DrawRectangleRec(logoutButton.rect, logoutButton.color);
    
    DrawText(startGameButton.text, 
             startGameButton.rect.x + startGameButton.rect.width / 2 - MeasureText(startGameButton.text, 20) / 2,
             startGameButton.rect.y + 15, 20, buttonTextColor);
             
    DrawText(changeLevelButton.text, 
             changeLevelButton.rect.x + changeLevelButton.rect.width / 2 - MeasureText(changeLevelButton.text, 20) / 2,
             changeLevelButton.rect.y + 15, 20, buttonTextColor);
             
    
             
    DrawText(selectFruitButton.text, 
             selectFruitButton.rect.x + selectFruitButton.rect.width / 2 - MeasureText(selectFruitButton.text, 20) / 2,
             selectFruitButton.rect.y + 15, 20, buttonTextColor);
    
             
    DrawText(changeBackgroundButton.text, changeBackgroundButton.rect.x + changeBackgroundButton.rect.width / 2 - MeasureText(changeBackgroundButton.text, 20) / 2,
                      changeBackgroundButton.rect.y + 15, 20, buttonTextColor);
    
    
    DrawText(showLeaderboard.text, showLeaderboard.rect.x + showLeaderboard.rect.width / 2 - MeasureText(showLeaderboard.text, 20) / 2,
    showLeaderboard.rect.y + 15, 20, buttonTextColor);
    
    
    DrawText(logoutButton.text, 
             logoutButton.rect.x + logoutButton.rect.width / 2 - MeasureText(logoutButton.text, 20) / 2,
             logoutButton.rect.y + 15, 20, buttonTextColor);
}

void DrawSelectFruitScreen()
{
    //Background Image
    DrawTextureEx(backgroundTextures[0], (Vector2){0, 0}, 0.0f,
                  fmax((float)screenWidth / backgroundTextures[0].width,
                      (float)screenHeight / backgroundTextures[0].height), WHITE);    
                 
    Vector2 mousePoint = GetMousePosition();
    int startX = screenWidth / 2 - (MAX_FRUITS * 70) / 2;
    int y = screenHeight / 2 - 40;

    for (int i = 0; i < MAX_FRUITS; i++)
    {
        Rectangle fruitRec = {startX + i * 70, y, 60, 60};
        if (CheckCollisionPointRec(mousePoint, fruitRec))
        {
            DrawRectangleLinesEx(fruitRec, 3, highlightColor);
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                selectedFruit = i;
            }
        }
    

        DrawTextureEx(fruitTextures[i], (Vector2){fruitRec.x, fruitRec.y},
                      0.0f, 60.0f / fruitTextures[i].width, WHITE);
    }

    //Confirm Button
    Rectangle confirmButton = {screenWidth / 2 - 75, screenHeight - 100, 150, 40};
    bool confirmHovered = CheckCollisionPointRec(mousePoint, confirmButton);
    DrawRectangleRec(confirmButton, confirmHovered ? buttonHoverColor : buttonColor);
    DrawText("Back to Menu", confirmButton.x + confirmButton.width / 2 - MeasureText("Back to Menu", 20) / 2,
             confirmButton.y + 10, 20, buttonTextColor);

    if (confirmHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        currentScreen = MAIN_MENU;
    }

    DrawText("Select Your Fruit", screenWidth / 2 - 120, 40, 30, DARKBLUE);
}

int main()
{

    InitWindow(screenWidth, screenHeight, "Snake Game - Raylib Version");
    InitAudioDevice();
    SetTargetFPS(60);

    SetExitKey(KEY_NULL); // raylib by defauly exits program when esc is pressed to avoid that
    srand(time(NULL));
    InitializeDatabase();
    InitializeUI();

    //Loading sounds
    eatSound = LoadSound(".\\audio\\eat_fruit.wav");
    gameOverSound = LoadSound(".\\audio\\game_over.wav");
    bgMusic = LoadMusicStream(".\\audio\\background_music.wav");
    
    PlayMusicStream(bgMusic);


    // Loading fruit images
    fruitTextures[0] = LoadTexture("assets/fruits/apple.png");
    fruitTextures[1] = LoadTexture("assets/fruits/banana.png");
    fruitTextures[2] = LoadTexture("assets/fruits/cherry.png");
    fruitTextures[3] = LoadTexture("assets/fruits/grape.png");
    fruitTextures[4] = LoadTexture("assets/fruits/orange.png");

    //Loading Background Images
    backgroundTextures[0] = LoadTexture("assets/backgrounds/titlefin.png");
    backgroundTextures[1] = LoadTexture("assets/backgrounds/forest.png");
    backgroundTextures[2] = LoadTexture("assets/backgrounds/grassland.png");
    backgroundTextures[3] = LoadTexture("assets/backgrounds/desert.png");
    backgroundTextures[4] = LoadTexture("assets/backgrounds/snow.png");
    backgroundTextures[5] = LoadTexture("assets/backgrounds/leaderboard.png");


    
    

    

    while (!WindowShouldClose())
    {
        UpdateMusicStream(bgMusic);
        Vector2 mousePoint = GetMousePosition();

        BeginDrawing();
        ClearBackground(bgColor);

        switch (currentScreen)
        {
        case ACCOUNT_MENU:
            // Checking button hover
            loginButton.isHovered = CheckCollisionPointRec(mousePoint, loginButton.rect);
            createAccountButton.isHovered = CheckCollisionPointRec(mousePoint, createAccountButton.rect);
            exitButton.isHovered = CheckCollisionPointRec(mousePoint, exitButton.rect);

            // Button clicks
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                if (loginButton.isHovered)
                {
                    currentScreen = LOGIN_SCREEN;
                    memset(username, 0, sizeof(username));
                    memset(password, 0, sizeof(password));
                    usernameLetterCount = 0;
                    passwordLetterCount = 0;
                    showErrorMessage = false;
                }
                else if (createAccountButton.isHovered)
                {
                    currentScreen = CREATE_ACCOUNT;
                    memset(username, 0, sizeof(username));
                    memset(password, 0, sizeof(password));
                    usernameLetterCount = 0;
                    passwordLetterCount = 0;
                    showErrorMessage = false;
                }
                else if (exitButton.isHovered)
                {
                    exit_game = true;
                    break;
                }
            }

            DrawAccountMenu();
            break;

        case LOGIN_SCREEN:
            // Text input
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                isUsernameFocused = CheckCollisionPointRec(mousePoint, usernameField);
                isPasswordFocused = CheckCollisionPointRec(mousePoint, passwordField);

                // Login and Back buttons
                Rectangle loginButtonRect = {screenWidth / 2 - 150, screenHeight / 2 + 60, 140, 40};
                Rectangle backButtonRect = {screenWidth / 2 + 10, screenHeight / 2 + 60, 140, 40};

                if (CheckCollisionPointRec(mousePoint, loginButtonRect))
                {
                    // Attempt login
                    if (usernameLetterCount > 0 && passwordLetterCount > 0)
                    {
                        if (AuthenticateUser(username, password))
                        {
                            highscore = Scorer();
                            currentScreen = MAIN_MENU;
                            showErrorMessage = false;
                        }
                        else
                        {
                            strcpy(errorMessage, "Invalid username or password");
                            showErrorMessage = true;
                        }
                    }
                    else
                    {
                        strcpy(errorMessage, "Please enter both username and password");
                        showErrorMessage = true;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, backButtonRect))
                {
                    currentScreen = ACCOUNT_MENU;
                    showErrorMessage = false;
                }
            }

            if (isUsernameFocused)
            {
                ProcessTextInput(username, &usernameLetterCount, 50);
            }

            if (isPasswordFocused)
            {
                ProcessTextInput(password, &passwordLetterCount, 50);
            }

            DrawLoginScreen();
            break;

        case CREATE_ACCOUNT:
            // Text Input
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                isUsernameFocused = CheckCollisionPointRec(mousePoint, usernameField);
                isPasswordFocused = CheckCollisionPointRec(mousePoint, passwordField);

                // Create and Back buttons
                Rectangle createButtonRect = {screenWidth / 2 - 150, screenHeight / 2 + 60, 140, 40};
                Rectangle backButtonRect = {screenWidth / 2 + 10, screenHeight / 2 + 60, 140, 40};

                if (CheckCollisionPointRec(mousePoint, createButtonRect))
                {
                    // Checking validity of username and password
                    if (usernameLetterCount > 0 && passwordLetterCount > 0)
                    {
                        if (CreateAccount(username, password))
                        {
                            strcpy(errorMessage, "Account created successfully! You can now login.");
                            showErrorMessage = true;
                            memset(username, 0, sizeof(username));
                            memset(password, 0, sizeof(password));
                            usernameLetterCount = 0;
                            passwordLetterCount = 0;
                        }
                        else
                        {
                            strcpy(errorMessage, "Username already exists");
                            showErrorMessage = true;
                        }
                    }
                    else
                    {
                        strcpy(errorMessage, "Please enter both username and password");
                        showErrorMessage = true;
                    }
                }
                else if (CheckCollisionPointRec(mousePoint, backButtonRect))
                {
                    currentScreen = ACCOUNT_MENU;
                    showErrorMessage = false;
                }
            }

            if (isUsernameFocused)
            {
                ProcessTextInput(username, &usernameLetterCount, 50);
            }

            if (isPasswordFocused)
            {
                ProcessTextInput(password, &passwordLetterCount, 50);
            }

            DrawCreateAccountScreen();
            break;

            case MAIN_MENU:
            // Checking button hover
            startGameButton.isHovered = CheckCollisionPointRec(mousePoint, startGameButton.rect);
            changeLevelButton.isHovered = CheckCollisionPointRec(mousePoint, changeLevelButton.rect);
            selectFruitButton.isHovered = CheckCollisionPointRec(mousePoint, selectFruitButton.rect);
            changeBackgroundButton.isHovered = CheckCollisionPointRec(mousePoint, changeBackgroundButton.rect);
            showLeaderboard.isHovered = CheckCollisionPointRec(mousePoint, showLeaderboard.rect);
            logoutButton.isHovered = CheckCollisionPointRec(mousePoint, logoutButton.rect);

            // Process button clicks
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                if (startGameButton.isHovered)
                {
                    SetLevel();
                    Setup();
                    currentScreen = GAMEPLAY;
                }
                else if (changeLevelButton.isHovered)
                {
                    cl++;
                    if (cl > '4')
                        cl = '1';
                    SetLevel();
                    sprintf(changeLevelButton.text, "Change Level (Current: %c)", cl);
                }
                else if (selectFruitButton.isHovered)
                {
                    currentScreen = SELECT_FRUIT_SCREEN;
                }
                else if (changeBackgroundButton.isHovered)
                {
                    currentScreen = SELECT_BACKGROUND_SCREEN;
                }

                else if (showLeaderboard.isHovered)
                {
                    currentScreen = SELECT_LEADERBOARD;
                    leaderboardCount = GetLeaderboard(leaderboardUsernames, leaderboardScores, LEADERBOARD_SIZE); // Refresh data
                }
                else if (logoutButton.isHovered)
                {
                    currentScreen = ACCOUNT_MENU;
                }
            }

            leaderboardCount = GetLeaderboard(leaderboardUsernames, leaderboardScores, LEADERBOARD_SIZE);

            DrawMainMenu();
            break;

        case SELECT_FRUIT_SCREEN:
        
            DrawSelectFruitScreen();
            break;

        case SELECT_BACKGROUND_SCREEN:
            DrawSelectBackgroundScreen();
            break;

        case SELECT_LEADERBOARD:
            ShowLeaderBoard();
            break;
        

        

        case GAMEPLAY:
        
            if(selectedBackground ==  0){
                selectedBackground=1;
            }

            DrawTextureEx(backgroundTextures[selectedBackground], (Vector2){0, 0}, 0.0f, 
            fmax((float)screenWidth / backgroundTextures[selectedBackground].width, 
            (float)screenHeight / backgroundTextures[selectedBackground].height), WHITE);
            if (IsKeyPressed(KEY_A) && dir != 2)
                dir = 1;
            if (IsKeyPressed(KEY_D) && dir != 1)
                dir = 2;
            if (IsKeyPressed(KEY_W) && dir != 4)
                dir = 3;
            if (IsKeyPressed(KEY_S) && dir != 3)
                dir = 4;
            if (IsKeyPressed(KEY_R) && gameOver)
                ResetGame();
            if (IsKeyPressed(KEY_X))
                gameOver = 1;
            if (IsKeyPressed(KEY_ESCAPE))
                currentScreen = MAIN_MENU;

            if (!gameOver)
            {
                static float moveTimer = 0;
                moveTimer += GetFrameTime();
                cur_time = GetTime() - init_time;

                if (moveTimer >= (1.0f / (FRAME_RATE * gameSpeed)))
                {
                    UpdateGameState();
                    moveTimer = 0;
                }

                if (cur_time >= lvlt && cl != '4')
                    gameOver = 1;
            }

            // Draw border around the game area
            DrawRectangleLinesEx((Rectangle){0, 0, WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE}, 3, borderColor);

            // Draw fruit
            DrawTextureEx(fruitTextures[selectedFruit],
                          (Vector2){fruitX * CELL_SIZE, fruitY * CELL_SIZE},
                          0.0f,
                          (float)CELL_SIZE / fruitTextures[selectedFruit].width,
                          WHITE);

            // Draw snake tail
            for (int i = 0; i < nTail; i++)
            {
                DrawRectangle(tailX[i] * CELL_SIZE, tailY[i] * CELL_SIZE, CELL_SIZE, CELL_SIZE, bodyColor);
            }

            // Draw snake head
            DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, headColor);

            // Draw UI information
            DrawText(TextFormat("Score: %d", score), 10, HEIGHT * CELL_SIZE + 10, 25, BLACK);
            DrawText(TextFormat("Highscore: %d", highscore), 180, HEIGHT * CELL_SIZE + 10, 25, BLACK);
            DrawText(TextFormat("Time: %.0fs", cur_time), 650, HEIGHT * CELL_SIZE + 10, 25, BLACK);
            DrawText(TextFormat("Level: %c", cl), 950, HEIGHT * CELL_SIZE + 10, 25, BLACK);
            DrawText("Press ESC to return to menu", 10, HEIGHT * CELL_SIZE + 40, 25, BLACK);

            // Draw game over message
            if (gameOver)
            {
                float alpha = sinf(GetTime() * 2.0f) * 0.5f + 0.5f; // Animate (fade in and out) game over message
                DrawText("Game Over!", screenWidth / 2 - 100, screenHeight / 2 - 40, 40, Fade(RED, alpha));
                DrawText("Press [R] to Restart", screenWidth / 2 - 100, screenHeight / 2 + 10, 20, Fade(BLACK, alpha));
            }
            break;

        default:
            break;
        }

        EndDrawing();

        if (WindowShouldClose() || exit_game)
            break;
    }

    for (int i = 0; i < MAX_FRUITS; i++)
    {
        UnloadTexture(fruitTextures[i]);
    }

    for (int i = 0; i < MAX_BACKGROUNDS; i++) {
        UnloadTexture(backgroundTextures[i]);
    }
    
    UnloadSound(eatSound);
    UnloadSound(gameOverSound);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}