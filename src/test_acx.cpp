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
    NavResult<APoint> aspr;
    cout << "Init Across Map" << endl;
    AcrossMap adp = across::AcrossMap(1000, 1000);

    cout << "Calculating" << endl;
    cout << "Damaged: " << check_dmg(&adp) << endl;
    int x = 2;
    int y = 0;
    int z = 0;
    while (x--) {
        adp.randomGenerate(5);
        adp.find(aspr, NavMethodRule::NavigationIntelegency, adp.front(), adp.back());
        if (aspr.status == Opened && z < aspr.RelativePaths.size()) {
            ++y;
            z = aspr.RelativePaths.size();
        }
    }
    cout << "Damaged: " << check_dmg(&adp) << endl;
    cout << "Status: " << y << " opened" << endl;
    cout << "Paths: " << z << endl;
    cout << "End." << endl;
}
