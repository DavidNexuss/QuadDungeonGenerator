#include <dungeon.hh>
int main(int argc,char** argv)
{
    int depth = argc > 1 ? std::stoi(argv[1]) : 5;
    int walls = argc > 2 ? std::stoi(argv[2]) : 300;

    Dungeon dungeon(600);
    dungeon.generate(depth,walls);
    dungeon.print();
}
