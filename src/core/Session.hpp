#pragma once

class Session {
public:
    Session(int fd);
private:
    int fd_;
};