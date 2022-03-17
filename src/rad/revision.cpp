#include "rad/revision.h"

#include <cstring>

static constexpr const char* REVISION = "xxxx";
static constexpr const char* DESCRIBE = "xxxx";
static constexpr const char* TAG_NAME = "notag";
static constexpr unsigned int TAG_DISTANCE = 0;
static constexpr bool DIRTY = false;


static constexpr int length(const char* str) {
    return (*str ? 1 + length(str + 1) : 0);
}


const char* getRevision() {
    return REVISION;
}

const char* getDescribe() {
    return DESCRIBE;
}

const char* getTagName() {
    return TAG_NAME;
}

unsigned int getTagDistance() {
    return TAG_DISTANCE;
}

bool isDirty() {
    return DIRTY;
}


std::string getPostfixRevision(const char* version) {
    if constexpr (length(REVISION) == 0) {
        return "";
    }

    if constexpr (length(DESCRIBE) == 0) {
        return std::string("-").append(REVISION);
    }

    // TODO: Assume there is a v prefix
    if (std::strcmp(TAG_NAME + 1, version) == 0) {
        return (DESCRIBE + length(TAG_NAME));
    }
        
    return std::string("-").append(DESCRIBE);
}

std::string getReleaseRevision(const char* version) {
    if constexpr (length(REVISION) == 0) {
        return "";
    }
    
    if constexpr (length(DESCRIBE) == 0) {
        return std::string("-").append(REVISION);
    }

    // TODO: Assume there is a v prefix
    if (std::strcmp(TAG_NAME + 1, version) == 0) {
        if constexpr (TAG_DISTANCE == 0 && !DIRTY) {
            return "";
        }
        
        return (DESCRIBE + length(TAG_NAME));
    }
    
    return std::string("-").append(DESCRIBE);
}
