#include <iostream>
#include <across.hpp>

int check_dmg(across::AcrossMap* map)
{
    int dmgCount = 0;
    auto x = map->front();

    while (x != map->back()) {
        if (x->cost || x->flags || x->h) {
            ++dmgCount;
        }
        ++x;
    }
    return dmgCount;
}

int main()
{
    using namespace across;

    using namespace std;

    cout << "Init Across Map :: write width and height" << endl;
    AcrossMap map = across::AcrossMap(102, 104);
    AcrossMap::NavigateionResult result;

    cout << "Calculating" << endl;
    int x = 1, y = 0, z = 0;

    while (x--) {
        map.randomGenerate(0xdea);

        auto front = map.front();
        auto back = map.back();

        map.find(result, NavMethodRule::NavigationIntelegency, front, back);
        if (result.status == NavStatus::Opened && z < result.RelativePaths.size()) {
            ++y;
            z = result.RelativePaths.size();
        }
    }
    cout << "Damaged: " << check_dmg(&map) << endl;
    cout << "Status: " << y << " opened" << endl;
    cout << "Paths: " << z << endl;
    cout << "End." << endl;
}
