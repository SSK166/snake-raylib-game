#include "sqlite3.h"
int Scorer();
void InitializeDatabase();
int AuthenticateUser(char *username, char *password);
int CreateAccount(char *username, char *password);
void SetColour(int ForgC);
int GetLeaderboard(char usernames[][50], int scores[], int maxEntries);
