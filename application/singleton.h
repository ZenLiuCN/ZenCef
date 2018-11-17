//
// Created by Zen Liu on 2018-11-17.
//

#ifndef PROJECT_SINGLETON_H
#define PROJECT_SINGLETON_H
template<typename T>
class Singleton
{
public:
static T& getInstance()
{
    static T value;
    return value;
}
private:
    Singleton();
    ~Singleton();
};

#endif //PROJECT_SINGLETON_H
