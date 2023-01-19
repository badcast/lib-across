#include <iostream>
#include <across.hpp>

int check_dmg(across::AcrossMap* map)
{
    int dmgCount = 0;
    across::Neuron* x = map->front();

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
    NavResult<APoint> result;
    cout << "Init Across Map" << endl;
    AcrossMap map = across::AcrossMap(100, 100);

    cout << "Calculating" << endl;
    int x = 1, y = 0, z = 0;

    while (x--) {
        map.randomGenerate(2);

        map.find(result, NavMethodRule::PlusMethod, map.front(), map.back());
        if (result.status == Opened && z < result.RelativePaths.size()) {
            ++y;
            z = result.RelativePaths.size();
        }
    }
    cout << "Damaged: " << check_dmg(&map) << endl;
    cout << "Status: " << y << " opened" << endl;
    cout << "Paths: " << z << endl;
    cout << "End." << endl;
}
