#include "keyboard.h"

/*
    define initial static values
*/

std::vector<void(*)(GLFWwindow* window, int key, int scancode, int action, int mods)> Keyboard::keyCallbacks;

// key state array (true for down, false for up)
bool Keyboard::keys[GLFW_KEY_LAST] = { 0 };
// key changed array (true if changed)
bool Keyboard::keysChanged[GLFW_KEY_LAST] = { 0 };

/*
    static callback
*/

// key state changed
void Keyboard::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // update state
    if (action != GLFW_RELEASE) {
        if (!keys[key]) {
            keys[key] = true;
        }
    }
    else {
        keys[key] = false;
    }
    keysChanged[key] = action != GLFW_REPEAT;

    // call registered callbacks
    for (void(*func)(GLFWwindow*, int, int, int, int) : Keyboard::keyCallbacks)
    {
        func(window, key, scancode, action, mods);
    }
}

/*
    static accessors
*/

// get key state
bool Keyboard::key(int key) {
    return keys[key];
}

// get if key recently changed
bool Keyboard::keyChanged(int key) {
    return keysChanged[key];
}

// get if key recently changed and is up
bool Keyboard::keyWentDown(int key) {
    return keys[key] && keyChanged(key);
}

// get if key recently changed and is down
bool Keyboard::keyWentUp(int key) {
    return !keys[key] && keyChanged(key);
}

void Keyboard::clearKeysChanged() {
    memset(keysChanged, 0, (size_t)GLFW_KEY_LAST);
}