#include "driver.h"


Driver::Driver() {

}

Driver::~Driver() {

}

void Driver::Activate() {

}

int Driver::Reset() {
    return 0;
}

void Driver::Deactivate() {

}


DriverManager::DriverManager() {
        num_drivers = 0;
}

void DriverManager::AddDriver(Driver* dr) {
    drivers[num_drivers++] = dr;
}

void DriverManager::ActivateAll() {
    for(int i = 0; i < num_drivers; ++i) {
        drivers[i]->Activate();
    }
}
