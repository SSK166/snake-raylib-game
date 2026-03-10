#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include "sqlite3.h"

#define DB_NAME "accounts.db"

int Scorer();
void InitializeDatabase();
int AuthenticateUser(char *username, char *password);
int CreateAccount(char *username, char *password);
void SetColour(int ForgC);

typedef struct
{
    char username[50];
    int highscore;
    int coins;
} User;

User currentUser;

// Initialize database and create table if it doesn't exist
void InitializeDatabase()
{
    sqlite3 *db;
    char *errMsg = 0;
    int rc = sqlite3_open(DB_NAME, &db);

    if (rc)
    {
        printf("Can't open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    char *sql = "CREATE TABLE IF NOT EXISTS Users ("
                "Username TEXT PRIMARY KEY, "
                "Password TEXT, "
                "Highscore INTEGER DEFAULT 0, "
                "Coins INTEGER DEFAULT 0);";

    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

//To accept password
int AuthenticateUser(char *username, char *password)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char storedPassword[50];

    if (sqlite3_open(DB_NAME, &db))
    {
        printf("Error opening database.\n");
        return 0;
    }

    char sql[] = "SELECT Password, Highscore, Coins FROM Users WHERE Username = ?";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        printf("Error preparing statement.\n");
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        strcpy(storedPassword, (const char *)sqlite3_column_text(stmt, 0));

        if (strcmp(password, storedPassword) == 0)
        {
            strcpy(currentUser.username, username);
            currentUser.highscore = sqlite3_column_int(stmt, 1);
            currentUser.coins = sqlite3_column_int(stmt, 2);
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return 1;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0;
}

// To return if creating was a success
int CreateAccount(char *username, char *password)
{
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int result = 0;

    // Check if the username already exists
    if (sqlite3_open(DB_NAME, &db))
    {
        return 0;
    }

    char checkSql[] = "SELECT Username FROM Users WHERE Username = ?";
    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, NULL) != SQLITE_OK)
    {
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        // Username already exists
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 0;
    }
    sqlite3_finalize(stmt);

    // Create the account
    char insertSql[] = "INSERT INTO Users (Username, Password, Highscore, Coins) VALUES (?, ?, 0, 0)";
    if (sqlite3_prepare_v2(db, insertSql, -1, &stmt, NULL) != SQLITE_OK)
    {
        sqlite3_close(db);
        return 0;
    }

    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_DONE)
    {
        result = 1; // Account created successfully
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return result;
}

int Scorer()
{
    sqlite3 *db;
    sqlite3_stmt *stmt;

    if (sqlite3_open(DB_NAME, &db))
    {
        return currentUser.highscore;
    }

    const char *sqlSelect = "SELECT Highscore FROM Users WHERE Username = ?";
    if (sqlite3_prepare_v2(db, sqlSelect, -1, &stmt, NULL) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, currentUser.username, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int dbHighscore = sqlite3_column_int(stmt, 0);
            extern int score;
            if (score > dbHighscore)
            {
                sqlite3_finalize(stmt);

                const char *sqlUpdate = "UPDATE Users SET Highscore = ? WHERE Username = ?";
                if (sqlite3_prepare_v2(db, sqlUpdate, -1, &stmt, NULL) == SQLITE_OK)
                {
                    sqlite3_bind_int(stmt, 1, score);
                    sqlite3_bind_text(stmt, 2, currentUser.username, -1, SQLITE_STATIC);
                    sqlite3_step(stmt);
                    currentUser.highscore = score;
                }
            }
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return currentUser.highscore;
}

// Set console text color
void SetColour(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}



int GetLeaderboard(char usernames[][50], int scores[], int maxEntries) {
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int count = 0;

    if (sqlite3_open(DB_NAME, &db)) {
        return 0;
    }

    // Query top scores, descending order
    const char *sql = "SELECT Username, Highscore FROM Users ORDER BY Highscore DESC LIMIT ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, maxEntries);

        while (sqlite3_step(stmt) == SQLITE_ROW && count < maxEntries) {
            const unsigned char *uname = sqlite3_column_text(stmt, 0);
            int score = sqlite3_column_int(stmt, 1);
            strcpy(usernames[count], (const char *)uname);
            scores[count] = score;
            count++;
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_close(db);
    return count; // Number of entries fetched
}
