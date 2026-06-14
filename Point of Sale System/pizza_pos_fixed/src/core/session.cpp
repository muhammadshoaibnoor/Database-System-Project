#include "session.h"

// Define the global session instance
SessionData gSession;

void setSession(const std::string& username,
                const std::string& fullName,
                UserRole role,
                int branchId,
                const std::string& branchName) {
    gSession.username   = username;
    gSession.fullName   = fullName;
    gSession.role       = role;
    gSession.branchId   = branchId;
    gSession.branchName = branchName;
    gSession.loggedIn   = true;
}

void clearSession() {
    gSession.username   = "";
    gSession.fullName   = "";
    gSession.role       = ROLE_RECEPTIONIST;
    gSession.branchId   = -1;
    gSession.branchName = "";
    gSession.loggedIn   = false;
}

bool isLoggedIn() {
    return gSession.loggedIn;
}

bool hasRole(UserRole role) {
    return gSession.loggedIn && gSession.role == role;
}

std::string getUsername() {
    return gSession.username;
}

std::string getFullName() {
    return gSession.fullName;
}

UserRole getRole() {
    return gSession.role;
}

int getBranchId() {
    return gSession.branchId;
}

std::string getBranchName() {
    return gSession.branchName;
}