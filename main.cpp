//#include "tanyao.h"
#include "Map.h"
#include "Path.h"
#include "Point.h"

int _main() {
    Map::init();
    Point start;
    start.x = 1;
    start.y = 2;
    Point end;
    end.x = 3;
    end.y = 4;
//    Path path = Path(start, end);
//    for (Point p = path.getNextPoint(); p != Point(-1, -1); p = path.getNextPoint()) {
//        cerr << p.x << ' ' << p.y << '\n';
//    }
    while (true) Map::update();
    return 0;
}
