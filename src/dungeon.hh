#include <memory>
#include <iostream>
#include <random>
#include <algorithm>
#include <functional>
#include <cmath>
#include <queue>
#include <cassert>

#define PRINT_QUAD_TREE 0
#define PRINT_BIN_WALLS 0
#define PRINT_ROOM_LAYOUT 1

#define PRINT_BRANCHES_LENGTH 1
#define PRINT_PATHS 0
#define PRINT_PASSAGES 0
#define PRINT_ROOMS 1

//Prim MST algoritthm
struct PII {
    int id;
    int distance;

    inline bool operator <(const PII& other) const { return distance < other.distance; }
    inline bool operator >(const PII& other) const { return distance > other.distance; }
};

std::vector<int> generateMinimumSpanningTree(const std::vector<std::vector<PII>>& adjacencyList)
{       
    int size = adjacencyList.size();
    std::vector<int> minimumSpanningTree(size,-1);
    std::priority_queue<PII,std::vector<PII>,std::greater<PII>> Q;
    std::vector<int> marked(size);
    std::vector<int> distances(size,std::numeric_limits<int>::max());
    Q.push({0,0});
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

std::vector<int> getGraphGrades(const std::vector<int>& graph)
{
    std::vector<int> grades(graph.size());
    for (int i = 0; i < graph.size(); i++)
    {
        if(graph[i] < 0) continue;
        grades[graph[i]]++;
    }
    return grades;
}
std::vector<std::vector<int>> reverseGraph(const std::vector<int>& graph)
{
    std::vector<std::vector<int>> R(graph.size());
    for (size_t i = 0; i < graph.size(); i++)
    {
        if(graph[i] < 0) continue;
        R[graph[i]].push_back(i);
    }
    return R;
}

void printRect(int x,int y,int width,int height,int r,int g,int b)
{
    std::cout << "<rect x=\"" << x << "\" y=\"" << y << "\" width=\"" << width << "\" height=\""  << height << "\" style=\"fill:rgb(255,255,255);stroke-width:1;stroke:rgb(" << r << "," << g << "," << b << ")\"/>" << std::endl;
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
                    std::cout << "<text x=\"" << x << "\" y=\"" << y << "\">" << depth << "</text>" << std::endl;
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

    std::shared_ptr<Node> root;
    QuadTree(int size) : root(std::make_shared<Node>())
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

    const std::shared_ptr<Node> getRoot() {
        return root;
    }
};
std::vector<int> generateLines(int count,int maxSize) {
    std::vector<int> lines(count);   
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
void printLines(const std::vector<int>& lines,const std::vector<bool>& mask,int orientation,int size) {
    
    for(int i = 0; i < lines.size(); ++i)
    {
        if(!mask[i]) continue;

        if(orientation == 0) printLine(lines[i],0,lines[i],size);
        else printLine(0,lines[i],size,lines[i]);
    }
}

struct Walls
{
    std::vector<int> horizontalLines;
    std::vector<int> verticalLines;
    
    std::vector<bool> usedHorizontal;
    std::vector<bool> usedVertical;

    std::vector<std::vector<int>> occupiedCells;

    int size;
    Walls(int _size) : size(_size) { }

    void generate(int count) {
        horizontalLines = generateLines(count,size);
        verticalLines = generateLines(count,size);
        usedHorizontal = std::vector<bool>(count);
        usedVertical = std::vector<bool>(count);
        occupiedCells = std::vector<std::vector<int>>(count,std::vector<int>(count));
    }


    void print() {
        printLines(horizontalLines,usedHorizontal,1,size);
        printLines(verticalLines,usedVertical,0,size);
    }
};

struct Room {
    int x1,y1,x2,y2;
    int roomId;
    int type;

    bool generateRoom(int x1,int y1,int x2,int y2,std::vector<int>& horizontalLines,std::vector<int>& verticalLines,std::vector<bool>& usedHorizontal,std::vector<bool>& usedVertical,std::vector<std::vector<int>>& occupiedCells) {
        
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


    inline int xcommon(const Room& other) const {
       // if(other.x1 == x1 && other.x2 == x2) return x2 - x1;
        if(other.x1 >= x1 && other.x1 <= x2) return x2 - other.x1;
        if(other.x2 >= x1 && other.x2 <= x2) return other.x2 - x1;
        return 0;
    }

    inline int ycommon(const Room& other) const {
       // if(other.y1 == y1 && other.y2 == y2) return y2 - y1;
        if(other.y1 >= y1 && other.y1 <= y2) return y2 - other.y1;
        if(other.y2 >= y1 && other.y2 <= y2) return other.y2 - y1;
        return 0;
    }
    
    inline int common(const Room& other) const {
        return std::max((other.xcommon(*this),other.ycommon(*this)),std::max(xcommon(other),ycommon(other)));
    }


    inline bool xclose(Room& other) const {
        return ((other.x1 > x1 && other.x1 < x2) || (other.x2 > x1 && other.x2 < x2) || (other.x1 == x1 && other.x2 == x2));
    }
    inline bool yclose(Room& other) const {
        return ((other.y1 > y1 && other.y1 < y2) || (other.y2 > y1 && other.y2 < y2) || (other.y1 == y1 && other.y2 == y2));
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

        float size;
        std::normal_distribution<float> normal(0,1.0);
        float n = normal(randomEngine);
        n = n * n;
        size = 0.5;

        if(aClosex) {
            
            int low;
            int high;

            if((b.x1 >= a.x1 && b.x1 <= a.x2)) {
                low = b.x1;
                high = a.x2;
            }
            else {
                low = a.x1;
                high = b.x2;
            }

            int midx = (low + high) / 2;
            int s = size * (high - low)/2;
            px1 = std::max(midx - s - 1,low);
            py1 = std::max(a.y1,b.y1);
            px2 = std::min(midx + s + 1,high);
            py2 = std::min(a.y2,b.y2);
        }

        if(aClosey) {

            int low;
            int high;

            if(b.y1 >= a.y1 && b.y1 <= a.y2) {
                low = b.y1;
                high = a.y2;
            }
            else {
                low = a.y1;
                high = b.y2;
            }
            int midy = (low + high) / 2;
            int s = size * (high - low)/2;
            px1 = std::max(a.x1,b.x1);
            py1 = std::max(midy - 1 -s,low);
            px2 = std::min(a.x2,b.x2);
            py2 = std::min(midy + 1 + s,high);
        }

        

        Room passage;
        passage.setCoords(px1,py1,px2,py2);
        return passage;
    }
};

struct RoomLayout
{
    std::vector<Room> rooms;
    std::vector<Room> pasages;

    std::vector<int> roomLayout;
    std::vector<std::vector<int>> reverseRoomLayout;

    std::vector<int> branchesLength;

    void appendRoom(int x1,int y1,int x2,int y2,std::vector<int>& horizontalLines,std::vector<int>& verticalLines,std::vector<bool>& usedHorizontal,std::vector<bool>& usedVertical,std::vector<std::vector<int>>& occupiedCells)
    {
        rooms.emplace_back();
        rooms.back().roomId = rooms.size() - 1;
        if(!rooms.back().generateRoom(x1,y1,x2,y2,horizontalLines,verticalLines,usedHorizontal,usedVertical,occupiedCells)) rooms.pop_back();
    }

    std::vector<std::vector<PII>> generateGraph()
    {
        std::vector<std::vector<PII>> adjacentList(rooms.size());
        #pragma omp parallel for
        for(int i = 0; i < rooms.size(); i++)
        {
            for (int j = 0; j < rooms.size(); j++)
            {
                int dst = rooms[i].distance(rooms[j]);
                if(rooms[i].common(rooms[j]) > 4)
                    adjacentList[i].push_back(PII{j,dst});
            }
        }
        return adjacentList;
    }

    void generatePaths() {
        roomLayout = generateMinimumSpanningTree(generateGraph());
        reverseRoomLayout = reverseGraph(roomLayout);
        for (int i = 0; i < roomLayout.size(); i++)
        {
            int u = i;
            int v = roomLayout[i];

            if(v < 0) continue;

            pasages.push_back(Room::createPassage(rooms[u],rooms[v]));
        }
        
    }

    //------[Debug print functions]--------------------------------------

    void printCentered(const std::string& text) {
        
        for (int i = 0; i < rooms.size(); i++) {
            int x,y;
            rooms[i].center(x,y);
            std::cout << "<text x=\"" << (x - 5) << "\" y=\"" << (y + 7) << "\">" << branchesLength[i] << "</text>" << std::endl;
        }
    }
    void printBranchesLength()
    {
        for (int i = 0; i < rooms.size(); i++)
        {
            int x,y;
            rooms[i].center(x,y);
            std::cout << "<text x=\"" << (x - 5) << "\" y=\"" << (y + 7) << "\">" << branchesLength[i] << "</text>" << std::endl;
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
        if(PRINT_ROOMS) {
            for (int i = 0; i < rooms.size(); i++) {
                printRect(rooms[i].x1,rooms[i].y1,rooms[i].x2 - rooms[i].x1,rooms[i].y2 - rooms[i].y1,255,0,200);
            }
        }
        
        if(PRINT_PASSAGES) {
            for (int i = 0; i < pasages.size(); i++) {
                printRect(pasages[i].x1,pasages[i].y1,pasages[i].x2 - pasages[i].x1,pasages[i].y2 - pasages[i].y1,0,0,255);
            }
        }
        if(PRINT_PATHS)printPath();
        if(PRINT_BRANCHES_LENGTH) printBranchesLength();
    }
};

float get_random() {
    static std::default_random_engine e;
    static std::uniform_real_distribution<> dis(0, 1); // rage 0 - 1
    return dis(e);
}

struct RoomTypeGenerator {
    
    struct RoomTypeSpecification {
        float averageSize;
        float rarity;
    };

    std::vector<RoomTypeSpecification> roomTypes;

    void generateRoomType(Room& room) const {
       int id = 1;
       for(const RoomTypeSpecification& spec : roomTypes) {
           float expectedSizeDifference = std::sqrt(std::abs(room.area() - spec.averageSize));
           if(get_random() * expectedSizeDifference * spec.rarity > 0.5f) {
                room.roomId = id;
           }

           id++;
       } 
    }
    void generateRoomTypes(RoomLayout& layout) const {
        for(Room& room : layout.rooms) {
            generateRoomType(room);
        }
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

    void generate(int depth,int wallcount,const RoomTypeGenerator& generator = RoomTypeGenerator()) {
        quadTree.generate(depth);
        walls.generate(wallcount);

        quadTree.getRoot()->forEachLeave([&](int x1,int y1,int x2,int y2){
            roomLayout.appendRoom(x1,y1,x2,y2,walls.horizontalLines,walls.verticalLines,walls.usedHorizontal,walls.usedVertical,walls.occupiedCells);
        });

        roomLayout.generatePaths();
        generator.generateRoomTypes(roomLayout);
    }

    void print() {

        std::cout << "<svg width=\"" << size << "\" height=\"" << size << "\">" << std::endl;

        if(PRINT_QUAD_TREE)   quadTree.print();
        if(PRINT_BIN_WALLS)   walls.print();
        if(PRINT_ROOM_LAYOUT) roomLayout.print();

        std::cout << "</svg>" << std::endl;
    }
};
