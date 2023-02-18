#include <iostream>
#include <across.hpp>
#include <omp.h>
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
    cout << "Init Across Map :: write width and height" << endl;
    AcrossMap map = across::AcrossMap(1024, 1024);

    cout << "Calculating" << endl;
    int x = 1, y = 0, z = 0;

    while (0 && x--) {
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

const int longx =  1024 * 1024 * 344;
bool xxx = true;
    long* e = new long[longx];
    while (true) {
#pragma omp parallel for
        for (int i = omp_get_num_threads(); i < longx; ++i) {
            e[i] = ~0;
        }
    }

    delete[] e;
}
