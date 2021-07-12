#include <memory>
#include <iostream>
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>
#include <list>
#include <queue>
#include <cassert>
using namespace std;

struct PII {
    int id;
    int distance;

    inline bool operator <(const PII& other) const { return distance < other.distance; }
    inline bool operator >(const PII& other) const { return distance > other.distance; }
};

vector<int> generateMinimumSpanningTree(const vector<list<PII>>& adjacencyList)
{       
    int size = adjacencyList.size();
    vector<int> minimumSpanningTree(size,-1);
    priority_queue<PII,vector<PII>,greater<PII>> Q;
    vector<int> marked(size);
    vector<int> distances(size,std::numeric_limits<int>::max());
    Q.push({rand() % size,0});
    while(!Q.empty()) {
        PII current = Q.top();
        Q.pop();
        int u = current.id;
        if(marked[u]) continue;
        marked[u] = true;
        for(const auto& e : adjacencyList[u]) {
            int v = e.id;
            if(!marked[v] && e.distance < distances[v]) {
                minimumSpanningTree[v] = u;
                distances[v] = e.distance;
                Q.push(e);
            }
        }
    }
    return minimumSpanningTree;
}

void printRect(int x,int y,int width,int height,int r,int g,int b)
{
    cout << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\""  << height << "\" style=\"fill:rgb(255,255,255);stroke-width:1;stroke:rgb(" << r << "," << g << "," << b << ")\"/>" << endl;
}
std::default_random_engine randomEngine;

struct QuadTree
{
    struct Node
    {
        int x1,y1,x2,y2;
        Node* children[4];

        Node() {
            children[0] = nullptr;
        }

        inline Node* clone() {
            Node* child = new Node;
            child->x1 = x1;
            child->y1 = y1;
            child->x2 = x2;
            child->y2 = y2;
            return child;
        }
        inline void removeAll() {
            for (int i = 0; i < 4; i++) {
                delete children[i];
            }
        }
        void split(int x,int y) {
            for (int i = 0; i < 4; i++) {
                children[i] = clone();
                switch(i)
                {
                    case 0:
                        children[i]->x2 = x;
                        children[i]->y2 = y;
                        break;
                    case 1:
                        children[i]->x1 = x;
                        children[i]->y2 = y;
                        break;
                    case 2:
                        children[i]->x2 = x;
                        children[i]->y1 = y;
                        break;
                    case 3:
                        children[i]->x1 = x;
                        children[i]->y1 = y;
                        break;
                };
            }
        }

        void generateRandom(int depth = 0)
        {
            if(depth < 0) return;
            
            int SX = (x2 - x1);
            int SY = (y2 - y1);

            int sx = (x2 - x1)/2;
            int sy = (y2 - y1)/2;

            std::normal_distribution<double> normalX(sx,sx/4);
            std::normal_distribution<double> normalY(sy,sy/4);
            
            sx = normalX(randomEngine);
            sy = normalY(randomEngine);

            if(sx <= 20 || sy <= 20 || sx >= SX || sy >= SY) return;

            int x = sx + x1;
            int y = sy + y1;

            split(x,y);
            for(int i = 0; i < 4; i++) {
                children[i]->generateRandom(depth - 1);
            }
        }

        void print(int depth = 0)
        {
            if(!isLeave())
            {
                for(int i = 0; i < 4; i++)
                {
                    if(children[i] != nullptr) children[i]->print(depth + 1);
                }
            }
            else
            {
                int x = x1;
                int y = y1;
                int width = x2 - x1;
                int height = y2 - y1;
                
                printRect(x,y,width,height,0,0,0);
                if(true)
                {
                    x = (x1 + x2) / 2;
                    y = (y1 + y2) / 2;
                    cout << "<text x=\"" << x << "\" y=\"" << y << "\">" << depth << "</text>" << endl;
                }
            }
        }


        inline bool isLeave() const {
            return children[0] == nullptr && children[1] == nullptr && children[2] == nullptr && children[3] == nullptr;
        }

        void forEachLeave(const std::function<void(int,int,int,int)>& callback) const {

            if(isLeave()) return callback(x1,y1,x2,y2);
            
            for (int i = 0; i < 4; i++)
            {
                if(children[i] != nullptr) 
                {
                    children[i]->forEachLeave(callback);
                }
            }
        }
    };

    shared_ptr<Node> root;
    QuadTree(int size) : root(make_shared<Node>())
    {
        root->x1 = 0;
        root->y1 = 0;
        root->x2 = size;
        root->y2 = size;
    }

    void generate(int depth)
    {
        root->generateRandom(depth);
    }

    void print()
    {
        root->print();
    }

    const shared_ptr<Node> getRoot() {
        return root;
    }
};
vector<int> generateLines(int count,int maxSize) {
    vector<int> lines(count);   
    for(int i = 0; i < count; ++i) {
        lines[i] = rand() % maxSize;
    }

    sort(lines.begin(),lines.end());
    return lines;
}

void printLine(int x1,int y1,int x2,int y2)
{
    printf("<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(255,0,0);stroke-width:1\" />\n",x1,y1,x2,y2);
}
void printLines(const vector<int>& lines,const vector<bool>& mask,int orientation,int size) {
    
    for(int i = 0; i < lines.size(); ++i)
    {
        if(!mask[i]) continue;

        if(orientation == 0) printLine(lines[i],0,lines[i],size);
        else printLine(0,lines[i],size,lines[i]);
    }
}

struct Walls
{
    vector<int> horizontalLines;
    vector<int> verticalLines;
    
    vector<bool> usedHorizontal;
    vector<bool> usedVertical;

    vector<vector<int>> occupiedCells;

    int size;
    Walls(int _size) : size(_size) { }

    void generate(int count) {
        horizontalLines = generateLines(count,size);
        verticalLines = generateLines(count,size);
        usedHorizontal = vector<bool>(count);
        usedVertical = vector<bool>(count);
        occupiedCells = vector<vector<int>>(count,vector<int>(count));
    }


    void print() {
        printLines(horizontalLines,usedHorizontal,1,size);
        printLines(verticalLines,usedVertical,0,size);
    }
};

struct Room {
    int x1,y1,x2,y2;
    int roomId;

    bool generateRoom(int x1,int y1,int x2,int y2,vector<int>& horizontalLines,vector<int>& verticalLines,vector<bool>& usedHorizontal,vector<bool>& usedVertical,vector<vector<int>>& occupiedCells) {
        
        auto lix = upper_bound(verticalLines.begin(), verticalLines.end(),x1);
        auto lfx = lower_bound(verticalLines.begin(), verticalLines.end(),x2) - 1;

        auto liy = upper_bound(horizontalLines.begin(), horizontalLines.end(),y1);
        auto lfy = lower_bound(horizontalLines.begin(), horizontalLines.end(),y2) - 1;

        this->x1 = std::max(*lix,x1);
        this->y1 = std::max(*liy,y1);
        this->x2 = std::min(*lfx,x2);
        this->y2 = std::min(*lfy,y2);
        
        if(!minimumSize()) return false;

        usedHorizontal[std::distance(horizontalLines.begin(), liy)] = true;
        usedHorizontal[std::distance(horizontalLines.begin(), lfy)] = true;

        usedVertical[std::distance(verticalLines.begin(), lix)] = true;
        usedVertical[std::distance(verticalLines.begin(), lfx)] = true;

        for (int sx = std::distance(verticalLines.begin(), lix); sx < std::distance(verticalLines.begin(), lfx); sx++)
        {
            for (int sy = std::distance(horizontalLines.begin(), liy); sy < std::distance(horizontalLines.begin(), lfy); sy++)
            {
                occupiedCells[sx][sy] = roomId;
            }
        }
        
        
        return true;
    /*
        this->x1 = x1 + 5;
        this->x2 = x2 - 5;
        this->y1 = y1 + 5;
        this->y2 = y2 - 5;*/
    }

    inline void setCoords(int x1, int y1, int x2, int y2) {
        this->x1 = std::min(x1,x2);
        this->y1 = std::min(y1,y2);
        this->x2 = std::max(x1,x2);
        this->y2 = std::max(y1,y2);
    }
    inline bool minimumSize() const {
        return ((x2 - x1) > 20) && ((y2 - y1) > 20);
    }

    inline int area() const {
        return (x2 - x1) * (y2 - y1);
    }

    inline int distance(Room& other) const {
        
        int x1,y1,x2,y2;
        center(x1,y1);
        other.center(x2,y2);

        return (x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1);
    }

    inline void center(int& x,int& y) const {
        x = (x2 + x1)/2;
        y = (y2 + y1)/2;
    }

    inline bool xclose(Room& other) const {
        return ((other.x1 >= x1 && other.x1 <= x2) || (other.x2 >= x1 && other.x2 <= x2));
    }
    inline bool yclose(Room& other) const {
        return ((other.y1 >= y1 && other.y1 <= y2) || (other.y2 >= y1 && other.y2 <= y2));
    }

    inline bool close(Room& other) const {
        return  xclose(other) || yclose(other);
    }

    static Room createPassage(Room a,Room b) {
        
        bool aClosex = a.xclose(b);
        bool aClosey = a.yclose(b);
        bool bClosex = b.xclose(a);
        bool bClosey = b.yclose(a);

        if(bClosex || bClosey)
        {
            aClosex = bClosex;
            aClosey = bClosey;
            std::swap(a,b);
        }

        //A is father
        //B is child

        int px1,py1,px2,py2;
        int x1,y1,x2,y2;
        a.center(x1,y1);
        b.center(x2,y2);

        if(aClosex) {
            
            int midx;
            if((b.x1 >= a.x1 && b.x1 <= a.x2)) {
                midx = (b.x1 + a.x2) / 2;
            }
            else {
                midx = (a.x1 + b.x2) / 2;
            }

            px1 = midx - 1;
            py1 = std::max(a.y1,b.y1);
            px2 = midx + 1;
            py2 = std::min(a.y2,b.y2);
        }

        if(aClosey) {
            int midy;

            if(b.y1 >= a.y1 && b.y1 <= a.y2) {
                midy = (b.y1 + a.y2) / 2;
            }
            else {
                midy = (a.y1 + b.y2) / 2;
            }
            px1 = std::max(a.x1,b.x1);
            py1 = midy - 1;
            px2 = std::min(a.x2,b.x2);
            py2 = midy + 1;
        }

        

        Room passage;
        passage.setCoords(px1,py1,px2,py2);
        return passage;
    }
};

struct RoomLayout
{
    vector<Room> rooms;
    vector<Room> pasages;

    vector<int> roomLayout;

    void appendRoom(int x1,int y1,int x2,int y2,vector<int>& horizontalLines,vector<int>& verticalLines,vector<bool>& usedHorizontal,vector<bool>& usedVertical,vector<vector<int>>& occupiedCells)
    {
        rooms.emplace_back();
        rooms.back().roomId = rooms.size() - 1;
        if(!rooms.back().generateRoom(x1,y1,x2,y2,horizontalLines,verticalLines,usedHorizontal,usedVertical,occupiedCells)) rooms.pop_back();
    }

    vector<list<PII>> generateGraph()
    {
        vector<list<PII>> adjacentList(rooms.size());
        for(int i = 0; i < rooms.size(); i++)
        {
            for (int j = 0; j < rooms.size(); j++)
            {
                int dst = rooms[i].distance(rooms[j]);
                if(rooms[i].close(rooms[j]))
                    adjacentList[i].push_back(PII{j,dst});
            }
        }
        return adjacentList;
    }

    void generatePaths() {
        roomLayout = generateMinimumSpanningTree(generateGraph());
        
        for (int i = 0; i < roomLayout.size(); i++)
        {
            int u = i;
            int v = roomLayout[i];

            if(v < 0) continue;

            pasages.push_back(Room::createPassage(rooms[u],rooms[v]));
        }
        
    }

    void printPath(){
        for (int i = 0; i < roomLayout.size(); i++)
        {
            if(roomLayout[i] < 0) continue;
            int u = i;
            int v = roomLayout[i];

            int x1,y1,x2,y2;
            rooms[u].center(x1,y1);
            rooms[v].center(x2,y2);

            printLine(x1,y1,x2,y2);
        }
        
    }
    void print()
    {
        for (int i = 0; i < rooms.size(); i++) {
            printRect(rooms[i].x1,rooms[i].y1,rooms[i].x2 - rooms[i].x1,rooms[i].y2 - rooms[i].y1,255,0,200);
        }
        
        for (int i = 0; i < pasages.size(); i++) {
            printRect(pasages[i].x1,pasages[i].y1,pasages[i].x2 - pasages[i].x1,pasages[i].y2 - pasages[i].y1,0,0,255);
        }
        
        printPath();
    }
};
struct Dungeon
{
    QuadTree quadTree;
    Walls walls;
    RoomLayout roomLayout;

    int size;
    Dungeon(int _size) : quadTree(_size), walls(_size), size(_size) {

    }

    void generate(int depth) {
        quadTree.generate(depth);
        walls.generate(700);

        quadTree.getRoot()->forEachLeave([&](int x1,int y1,int x2,int y2){

            roomLayout.appendRoom(x1,y1,x2,y2,walls.horizontalLines,walls.verticalLines,walls.usedHorizontal,walls.usedVertical,walls.occupiedCells);
        });

        roomLayout.generatePaths();


    }
    void print() {

        cout << "<svg width=\"" << size << "\" height=\"" << size << "\">" << endl;
        //quadTree.print();
        //walls.print();
        roomLayout.print();
        cout << "</svg>" << endl;
    }
};
int main()
{
    Dungeon dungeon(8000);
    dungeon.generate(6);
    dungeon.print();
}
