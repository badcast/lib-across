#include <iostream>
#include <across.hpp>

int main()
{
    using namespace across;

    using namespace std;
    NavResult<APoint> aspr;
    cout << "Init Across Map" << endl;
    AcrossMap adp = across::AcrossMap(10000, 10000);

    cout << "Calculating" << endl;

    int x = 100;
    int y = 0;
    int z = 0;
    while (--x != 0) {
        adp.randomGenerate(x);
        adp.find(aspr, NavMethodRule::NavigationIntelegency, adp.GetNeuron(1, 1), adp.GetNeuron(adp.getWidth() - 3, adp.getHeight() - 3));
        if (aspr.status == Opened && z < aspr.RelativePaths.size()) {
            ++y;
            z = aspr.RelativePaths.size();
        }
        adp.clear();
    }

    cout << "Status: " << y << " opened" << endl;
    cout << "Paths: " << z << endl;
    cout << "End." << endl;
}
