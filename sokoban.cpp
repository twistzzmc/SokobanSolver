#include <iostream>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

/*

1. Find a path from the box position to the DWF position. (BFS)
2. Check if worker can get to the position where the box first moves.
3. Check if there is a way for a worker to move the box accross its path (only turns matter)
    a) if there is - move the box
    b) if there is not - finda new path
4. Repeat for all the boxes.

*/


struct spot_t {
    int x, y;
    char c;
    bool movable;
    bool isDWF;
};


struct shop_t {
    int height;
    int width;
    int worker[2];
    vector<vector<spot_t>> map;
    vector<vector<int>> boxes;
    vector<vector<int>> DWFs; // designated warehouse fields
};


ostream& operator<<(ostream& os, const spot_t s);
ostream& operator<<(ostream& os, const vector<spot_t> v);
ostream& operator<<(ostream& os, const vector<vector<spot_t>> v);
ostream& operator<<(ostream& os, const vector<int> v);
ostream& operator<<(ostream& os, const vector<char> v);
ostream& operator<<(ostream& os, const vector<vector<int>> v);
ostream& operator<<(ostream& os, const shop_t* s);
ostream& operator<<(ostream& os, const shop_t s);

shop_t* readMapFromFile(string filePath);

bool move(shop_t* s, char d);
bool pullBoxAcrossFullPath(shop_t* s, vector<vector<spot_t>> path);
char moveTypeFromMove(spot_t start, spot_t end, bool ifPull);

bool canDoMove(shop_t s, char d);
bool canDoMove(shop_t s, char d, spot_t pos);

bool canDoPull(shop_t s, char d);
bool canDoPull(shop_t s, char d, spot_t pos);

bool checkForWin(shop_t s);
vector<spot_t> shortest_path(shop_t s, spot_t start, vector<spot_t> ends, unordered_set<string> forbidden = {});
int checkPullPath(shop_t s, vector<spot_t> path);
vector<vector<spot_t>> checkWorkerPlacement(shop_t s, vector<spot_t> path);
vector<vector<spot_t>> canBoxBeMovedToDWF(shop_t s, int boxNumber);

void putMovesIntoVectorFromPath(vector<char>* moves, vector<vector<spot_t>> path);
vector<char>* solveSokoban(shop_t* sokoban);

void testManually(shop_t* sokoban);
void printMovability(shop_t s);
void print_set(unordered_set<string> s);

string spotsToString(spot_t s1, spot_t s2);
bool checkIfInSet(int x1, int y1, int x2, int y2, unordered_set<string> set, shop_t s);


int main(int argc, char* argv[])
{
    cout << "\n\nLet the sokoban begin!\n" << endl;

    if (argc > 3) {
        cout << "ERROR: To many arguments! Arguments needed - 2!" << endl;
        return 0;
    } else if (argc == 1) {
        cout << "ERROR: To little arguments! Arguments needed - 2!" << endl;
        return 0;
    }

    shop_t* sokoban = readMapFromFile(argv[1]);
    cout << "Initial sokoban map:\n\n" << sokoban;
    
    shop_t* sokobanCopy = new shop_t;
    *sokobanCopy = *sokoban;
    vector<char>* moves = solveSokoban(sokoban);

    cout << "\nFinal sokoban map:\n\n" << sokoban;

    cout << "Moves:\n" << *moves;

    ofstream outputFile(argv[2]);
    outputFile << *moves;

    delete(sokoban);
    delete(moves);

    // cout << "\n\nLet the sokoban end!\n" << endl;
}

ostream& operator<<(ostream& os, const spot_t s) {
    os << "(" << s.x << ", " << s.y << ")";
    os << " '" << s.c << "' ";
    os << " | Movable: " << s.movable << " ";
    os << " | isDWF: " << s.isDWF << " \n";
    return os;
}

ostream& operator<<(ostream& os, const vector<spot_t> v) {
    for (size_t i = 0; i < v.size(); i++) {
        os << v[i];
    }
    return os;
}

ostream& operator<<(ostream& os, const vector<vector<spot_t>> v) {
    for (size_t i = 0; i < v.size(); i++) {
        os << v[i] << "\n";
    }
    return os << "\n\n";
}

ostream& operator<<(ostream& os, const vector<int> v) {
    os << "(";
    for (size_t i = 0; i < v.size(); i++) {
        os << v[i];
        if (i != v.size() - 1) { os << ", "; }
    }
    os << ") ";
    return os;
}

ostream& operator<<(ostream& os, const vector<char> v) {
    for (int i = 0; i < (int) v.size(); i++) {
        os << v[i];
    }

    return os;
}

ostream& operator<<(ostream& os, const vector<vector<int>> v) {
    for (size_t i = 0; i < v.size(); i++) {
        os << v[i];
    }
    os << "\n";
    return os;
}

ostream& operator<<(ostream& os, const shop_t* s) {
    os << "Dim:    " << s->height << " " << s->width << endl; 
    os << "Worker: (" << s->worker[0] << "," << s->worker[1] << ")\n";
    os << "DWFs:   ";

    for (int i = 0; i < (int)s->DWFs.size(); i++) {
        os << "(" << s->DWFs[i][0] << "," << s->DWFs[i][1] << ") ";
    }
    os << endl;

    os << "Boxes:  ";
    for (int i = 0; i < (int)s->boxes.size(); i++) {
        os << "(" << s->boxes[i][0] << "," << s->boxes[i][1] << ") ";
    }
    os << endl;

    for (int i = 0; i < s->height; i++) {
        for (int j = 0; j < s->width; j++) {
            os << s->map[i][j].c;
        }
        os << endl;
    }
    os << endl << endl;

    return os;
}

ostream& operator<<(ostream& os, const shop_t s) {
    os << "Dim:    " << s.height << " " << s.width << endl; 
    os << "Worker: (" << s.worker[0] << "," << s.worker[1] << ")\n";
    os << "DWFs:   ";

    for (unsigned long int i = 0; i < s.DWFs.size(); i++) {
        os << "(" << s.DWFs[i][0] << "," << s.DWFs[i][1] << ") ";
    }
    os << endl;

    for (int i = 0; i < s.height; i++) {
        for (int j = 0; j < s.width; j++) {
            os << s.map[i][j].c;
        }
        os << endl;
    }
    os << endl << endl;

    return os;
}


shop_t* readMapFromFile(string filePath) {
    shop_t* sokoban = new shop_t;
    vector<vector<spot_t> > map;
    string line;
    ifstream myfile(filePath);

    if (myfile.is_open()) {
        for (int i = 0; getline(myfile, line); i++) {
            if (i == 0) {
                sokoban->height = stoi(line.substr(0, line.find(" ")));
                sokoban->width = stoi(line.substr(line.find(" ") + 1, line.length()));
            } else {
                vector<spot_t> temp;
                for (int j = 0; j < sokoban->width; j++) {
                    char c = line.at(j);
                    spot_t spot;

                    spot.c = c;
                    spot.x = i - 1;
                    spot.y = j;

                    switch (c) {
                        case 'O':
                            {
                                vector<int> tempBox;
                                tempBox.push_back(i - 1);
                                tempBox.push_back(j);
                                sokoban->boxes.push_back(tempBox);
                            }
                        case '#':
                            spot.movable = false;
                            spot.isDWF = false;
                            break;

                        case '*':
                            {
                                vector<int> tempDWF;
                                tempDWF.push_back(i - 1);
                                tempDWF.push_back(j);
                                sokoban->DWFs.push_back(tempDWF);
                            }

                            spot.isDWF = true;
                            spot.movable = true;
                            break;
                        
                        case '-':
                            sokoban->worker[0] = i - 1;
                            sokoban->worker[1] = j;
                        
                        default:
                            spot.movable = true;
                            spot.isDWF = false;
                            break;
                    }

                    temp.push_back(spot);
                }
                map.push_back(temp);
            }
        }
        myfile.close();
    } else { throw "Unable to open file"; }

    sokoban->map = map;

    return sokoban;
}


bool move(shop_t* s, char d) {
    if (!canDoMove(*s, d)) return false;

    int worX, worY, boxX, boxY;
    if (d == 'l' || d == 'L') { worX = 0; worY = -1; }
    else if (d == 'u' || d == 'U') { worX = -1; worY = 0; }
    else if (d == 'r' || d == 'R') { worX = 0; worY = 1; }
    else if (d == 'd' || d == 'D') { worX = 1; worY = 0; }

    // cout << "\nx: " << worX << " // y: " << worY << "\n\n";

    if (d == 'L' || d == 'U' || d == 'R' || d == 'D') {
        boxX = worX * -1;
        boxY = worY * -1;
        // cout << "x: " << boxX << " // y: " << boxY << "\n\n";

        // change current box spot
        if (s->map[s->worker[0] + boxX][s->worker[1] + boxY].isDWF) { 
            s->map[s->worker[0] + boxX][s->worker[1] + boxY].c = '*'; 
        } 
        else { s->map[s->worker[0] + boxX][s->worker[1] + boxY].c = ' '; }

        s->map[s->worker[0] + boxX][s->worker[1] + boxY].movable = true;

        // change future box spot
        s->map[s->worker[0]][s->worker[1]].c = 'O';
        s->map[s->worker[0]][s->worker[1]].movable = false;
    } else {
        // change current worker spot
        // cout << "DWF = " << s.map[s.worker[0]][s.worker[1]].isDWF << "\n";
        if (s->map[s->worker[0]][s->worker[1]].isDWF) { 
            s->map[s->worker[0]][s->worker[1]].c = '*'; 
        } 
        else { s->map[s->worker[0]][s->worker[1]].c = ' '; }

        s->map[s->worker[0]][s->worker[1]].movable = true;
    }

    // change future worker spot
    s->map[s->worker[0] + worX][s->worker[1] + worY].c = '-';
    s->map[s->worker[0] + worX][s->worker[1] + worY].movable = true;

    // change worker coordinates
    s->worker[0] += worX;
    s->worker[1] += worY;

    return true;
}

bool pullBoxAcrossFullPath(shop_t* s, vector<vector<spot_t>> path) {
    for (int i = 0; i < (int)path.size(); i++) {
        for (int j = path[i].size() - 3; j >= 0; j--) {
            // cout << "Moving to: " << path[i][j];
            if (!move(s, moveTypeFromMove(path[i][j + 1], path[i][j], false))) return false;
        }
        // cout << "\n";
        if (!move(s, moveTypeFromMove(path[i][path[i].size() - 1], path[i][0], true))) return false;
    }

    return true;
}

char moveTypeFromMove(spot_t start, spot_t end, bool ifPull) {
    if (abs(start.x - end.x) + abs(start.y - end.y) != 1) { 
        throw "Move size has to be equal 1!";
    }

    if (start.x - end.x == 1 && !ifPull) return 'u';
    if (start.x - end.x == 1 && ifPull) return 'U';

    if (start.x - end.x == -1 && !ifPull) return 'd';
    if (start.x - end.x == -1 && ifPull) return 'D';

    if (start.y - end.y == -1 && !ifPull) return 'r';
    if (start.y - end.y == -1 && ifPull) return 'R';

    if (start.y - end.y == 1 && !ifPull) return 'l';
    if (start.y - end.y == 1 && ifPull) return 'L';

    throw "moveTypeFromMove Unknown ERROR!";
}


bool canDoMove(shop_t s, char d) {
    spot_t pos = { -1, -1, ' ', false, false };
    return canDoMove(s, d, pos);
}

bool canDoMove(shop_t s, char d, spot_t pos) {
    int x, y;
    if (pos.x == -1) {
        x = s.worker[0]; 
        y = s.worker[1];
    } else {
        x = pos.x;
        y = pos.y;
    }

    switch(d) {
        case 'l':
        case 'L':
            if (y == 0 || !s.map[x][y - 1].movable)
                return false;
            break;
        case 'u':
        case 'U':
            if (x == 0 || !s.map[x - 1][y].movable)
                return false;
            break;
        case 'r':
        case 'R':
            if (y == s.width - 1 || !s.map[x][y + 1].movable)
                return false;
            break;
        case 'd':
        case 'D':
            if (y == s.height - 1 || !s.map[x + 1][y].movable)
                return false;
            break;
    }

    if (d == 'L' || d == 'U' || d == 'R' || d == 'D') return canDoPull(s, d, pos);
    return true;
}


bool canDoPull(shop_t s, char d) {
    spot_t pos = { -1, -1, ' ', false, false };
    return canDoPull(s, d, pos);
}

bool canDoPull(shop_t s, char d, spot_t pos) {
    int x, y;
    if (pos.x == -1) {
        x = s.worker[0]; 
        y = s.worker[1];
    } else {
        x = pos.x;
        y = pos.y;
    }
    
    switch(d) {
        case 'L':
            if (y == s.width - 1 || s.map[x][y + 1].c != 'O') 
                return false;
            break;
        case 'U':
            if (x == s.height - 1 || s.map[x + 1][y].c != 'O') 
                return false;
            break;
        case 'R':
            if (y == 0 || s.map[x][y - 1].c != 'O') 
                return false;
            break;
        case 'D':
            if (x == 0 || s.map[x - 1][y].c != 'O') 
                return false;
            break;
    }

    return true;
}


bool checkForWin(shop_t s) {
    for (unsigned long int i = 0; i < s.DWFs.size(); i++) {
        if (s.map[s.DWFs[i][0]][s.DWFs[i][1]].c != 'O') 
            return false;
    }

    return true;
}

vector<spot_t> shortest_path(shop_t s, spot_t source, vector<spot_t> ends, unordered_set<string> forbidden) {
    // cout << "\n--SHORTESTPATH--\n";
    // cout << s << source << ends;
    
    //  for (int i = 0; i < s.height; i++) {
    //     for(int j = 0; j < s.width; j++) {
    //         cout << s.map[i][j].movable;
    //     }
    //     cout << endl;
    // }
    bool visited[s.height][s.width];
    vector<spot_t> paths[s.height][s.width];

    for (int i = 0; i < s.height; i++) {
        for (int j = 0; j < s.width; j++) {
            visited[i][j] = s.map[i][j].movable;
        }
    }

    queue<spot_t> q;
    q.push(source);
    visited[source.x][source.y] = false;
    while(!q.empty()) {
        spot_t p = q.front();
        q.pop();

        bool finish = false;
        for (size_t i = 0; i < ends.size(); i++) {
            if (ends[i].x == p.x && ends[i].y == p.y) { finish = true; }
        }

        if (finish) {
            vector<spot_t> path;
            path.push_back(p);

            if (!paths[p.x][p.y].empty()) {
                spot_t tmp = paths[p.x][p.y][0];
                while (tmp.x != source.x || tmp.y != source.y) {
                    path.push_back(tmp);
                    tmp = paths[tmp.x][tmp.y][0];
                }
                path.push_back(source);
            }

            return path; 
        }

        bool up = checkIfInSet(p.x, p.y, p.x - 1, p.y, forbidden, s);
        bool down = checkIfInSet(p.x, p.y, p.x + 1, p.y, forbidden, s);
        bool left = checkIfInSet(p.x, p.y, p.x, p.y - 1, forbidden, s);
        bool right = checkIfInSet(p.x, p.y, p.x, p.y + 1, forbidden, s);

        // cout << "FORBIDDEN " << forbidden.size() << "\n";
        // print_set(forbidden);

        if (p.x - 1 >= 0 && visited[p.x - 1][p.y] && !up) {
            visited[p.x - 1][p.y] = false;
            paths[p.x - 1][p.y].push_back(p);
            q.push(s.map[p.x - 1][p.y]);
        }

        if (p.x + 1 < s.height && visited[p.x + 1][p.y] && !down) {
            visited[p.x + 1][p.y] = false;
            paths[p.x + 1][p.y].push_back(p);
            q.push(s.map[p.x + 1][p.y]);
        }

        if (p.y - 1 >= 0 && visited[p.x][p.y - 1] && !left) {
            visited[p.x][p.y - 1] = false;
            paths[p.x][p.y - 1].push_back(p);
            q.push(s.map[p.x][p.y - 1]);
        }

        if (p.y + 1 >= 0 && visited[p.x][p.y + 1] && !right) {
            visited[p.x][p.y + 1] = false;
            paths[p.x][p.y + 1].push_back(p);
            q.push(s.map[p.x][p.y + 1]);
        }
    }
    // cout << "\n--FAILED SHORTEST PATH--\n";
    vector<spot_t> failed;
    return failed;
}

int checkPullPath(shop_t s, vector<spot_t> path) {
    // cout << "\n--CHECKPULLPATH--\n";
    for (int i = path.size() - 1; i > 0; i--) {
        char moveType = moveTypeFromMove(path[i], path[i - 1], false);
        bool ifPullPossible = canDoMove(s, moveType, path[i - 1]);
        if (!ifPullPossible) return i - 1;
    }

    return -1;
}

vector<vector<spot_t>> checkWorkerPlacement(shop_t s, vector<spot_t> path) {
    //  cout << "\n--CHECKWORKERPLACEMENT--\n";
    //  cout << "--STARTING PATH--\n" << path << "\n";
    vector<vector<spot_t>> fullPath;

    for (int i = path.size() - 2, j = 0; i >= 0; i--, j++) {
        vector<spot_t> cur_shor_path;
        vector<spot_t> ends;
        spot_t start;
        ends.push_back(path[i]);
        // cout << "i:     " << i << "\n";

        if ((size_t)i == path.size() - 2) {
            start = s.map[s.worker[0]][s.worker[1]];
        } else {
            int x = path[i + 1].x + (path[i + 1].x - path[i + 2].x);
            int y = path[i + 1].y + (path[i + 1].y - path[i + 2].y);
            // cout << "x: " << x << " | y: " << y << "\n";
            // cout << "Path[i + 1]: " << path[i + 1] << "Path[i + 2]: " << path[i + 2];
            start = s.map[x][y];
        }
        // cout << "start: " << start;
        // cout << "end:   " << ends[0];
        cur_shor_path = shortest_path(s, start, ends);
        s.map[ends[0].x][ends[0].y].movable = false;
        s.map[path[i + 1].x][path[i + 1].y].movable = true;
        // cout << "Cur box position: " << s.map[ends[0].x][ends[0].y];
        // cout << "Ex box position:  " << s.map[path[i + 1].x][path[i + 1].y];
        // for (int i = 0; i < s.height; i++) {
        //     for (int j = 0; j <s.width; j++) {
        //         cout << s.map[i][j].movable;
        //     }
        //     cout << "\n";
        // }
        // cout << "Shortest path: \n" << cur_shor_path << "-----------------------------------\n\n";

        if (cur_shor_path.size() == 0) { 
            vector<vector<spot_t>> empty; 
            vector<spot_t> tmp;
            tmp.push_back(spot_t { (int)i + 1, (int)i + 1, ' ', false, false });
            empty.push_back(tmp);
            return empty; 
        }

        fullPath.push_back(cur_shor_path);
        fullPath[j].push_back(path[i + 1]);
    }

    return fullPath;
}

vector<vector<spot_t>> canBoxBeMovedToDWF(shop_t s, int boxNumber) {
    // cout << "\n--CANBOXBEMOVEDTODWF--\n";
    unordered_set<string> forbiddenMoves;

    shop_t sCopy = s;
    while(true) {
        // boxes starting position (start for shortest path from box to DWF)
        spot_t start = s.map[s.boxes[boxNumber][0]][s.boxes[boxNumber][1]];

        // DWFs positions (ends for shortest path from box to DWF)
        vector<spot_t> ends;
        for (size_t i = 0; i < s.DWFs.size(); i++) {
            ends.push_back(s.map[s.DWFs[i][0]][s.DWFs[i][1]]);
        }

        // Look for shortest path between box and DWFs
        vector<spot_t> spb = shortest_path(s, start, ends, forbiddenMoves);

        // cout << "PATH CHECK:\n" << spb;
        // printMovability(s);

        // if no path (path length 0) has been returned cannot move this box to DWFs
        if (spb.size() == 0) {
            vector<vector<spot_t>> empty;
            return empty;
        }

        // check if worker can pull the box accross found path
        // if can return -1 else return index of spot where worker cannot do pull
        int pathStatus = checkPullPath(sCopy, spb);
        // cout << pathStatus << "\n\n";

        // if worker can do pull all accross the path
        if (pathStatus == -1) {

            // Check if worker can get to the needed spots to perform pull
            // accross the path return whole path if possible else return 1 spot_t
            vector<vector<spot_t>> fullPath = checkWorkerPlacement(sCopy, spb);

            // cout << "STATUS CHECK\n" << fullPath;

            // if worker cannot get to needed spots block the first box movement in the path
            if (fullPath.size() == 1 && fullPath[0].size() == 1) {
                // pathStatus = fullPath[0][0].x;
                spot_t s1 = spb[spb.size() - 1];
                spot_t s2 = spb[spb.size() - 2];
                forbiddenMoves.insert(spotsToString(s1, s2));
            } else {
                return fullPath;
            }

        } else {
            spot_t s1 = spb[pathStatus + 1];
            spot_t s2 = spb[pathStatus];
            forbiddenMoves.insert(spotsToString(s1, s2));
        }

        // mark the impossibility of where the path failed and search for another one
        // s.map[spb[pathStatus].x][spb[pathStatus].y].movable = false;
    }
}


void putMovesIntoVectorFromPath(vector<char>* moves, vector<vector<spot_t>> path) {
    for (int i = 0; i < (int) path.size(); i++) {
        for (int j = path[i].size() - 3; j >= 0; j--) {
            moves->push_back(moveTypeFromMove(path[i][j + 1], path[i][j], false));

        }
        moves->push_back(moveTypeFromMove(path[i][path[i].size() - 1], path[i][0], true));
    }
}

vector<char>* solveSokoban(shop_t* sokoban) {
    vector<char>* moves = new vector<char>;
    // cout << sokoban;

    vector<int> boxesToPut;
    for (int i = 0; i < (int) sokoban->boxes.size(); i++) {
        boxesToPut.push_back(i);
    }

    int currentBox = -1;
    while (!boxesToPut.empty()) {
        currentBox = (currentBox + 1) % boxesToPut.size();

        vector<vector<spot_t>> boxFullPath = canBoxBeMovedToDWF(*sokoban, boxesToPut[currentBox]);

        if (boxFullPath.size() != 0) {
            if (!pullBoxAcrossFullPath(sokoban, boxFullPath)) 
                throw "Failed to move the box accross its path!";

            boxesToPut.erase(boxesToPut.begin() + currentBox);
            putMovesIntoVectorFromPath(moves, boxFullPath);
            // cout << sokoban;
        }
    }

    return moves;
}


void testManually(shop_t* sokoban) {
    cout << "\n\nManual testing!\n" << sokoban << "Move... ";
    
    char c;
    while (true) {
        cin >> c;

        if (c == 'E') break;

        move(sokoban, c);
        cout << sokoban;
    }
}

void printMovability(shop_t s) {
    for (int i = 0; i < s.height; i++) {
        for (int j = 0; j < s.width; j++) {
            cout << s.map[i][j].movable;
        }
        cout << endl;
    }
}

void print_set(unordered_set<string> s) {
    copy(s.begin(), s.end(), ostream_iterator<string>(cout, " "));
}

string spotsToString(spot_t s1, spot_t s2) {
    string s = to_string(s1.x) + "." + to_string(s1.y);
    s += "|";
    s += to_string(s2.x) + "." + to_string(s2.y);
    return s;
}

bool checkIfInSet(int x1, int y1, int x2, int y2, unordered_set<string> set, shop_t s) {
    spot_t s1 = s.map[x1][y1];
    spot_t s2 = s.map[x2][y2];
    string key = spotsToString(s1, s2);
    unordered_set<string>::const_iterator got = set.find(key);

    if (got == set.end()) return false;
    return true;
}
