#ifndef SESSION_H
#define SESSION_H

#include "globals.h"
#include <string>

struct SessionData {
    std::string username;
    std::string fullName;
    UserRole role;
    int branchId;
    std::string branchName;
    bool loggedIn;

    SessionData() : role(ROLE_RECEPTIONIST), branchId(-1), loggedIn(false) {}
};

// Global session instance — declared extern, defined in session.cpp
extern SessionData gSession;

// Session management functions
void setSession(const std::string& username,
                const std::string& fullName,
                UserRole role,
                int branchId,
                const std::string& branchName);

void clearSession();

// Accessor functions
bool isLoggedIn();
bool hasRole(UserRole role);
std::string getUsername();
std::string getFullName();
UserRole getRole();
int getBranchId();
std::string getBranchName();

#endif // SESSION_H