/*
서울시립대학교 컴퓨터 알고리즘
: 수학과 - 박진수
: 컴퓨터과학과 - 김민서, 김영민
: DSolving+ algorithm
*/

#include<vector> 
#include <bits/stdc++.h>
#include<iostream>
#include<queue>
#include <algorithm>
#include <chrono>
using namespace std;

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3 

const int m = 5; // 행
const int n = 6; // 열

struct node{
    vector<vector<int> > state;
    int y; int x;
    int next_seq; int hash_value;
    int prev_seq; int reverse_direct;
};

struct STT{
    int full_prev_seq[361];
    int full_reverse_direct[361];
};

int GetInverse(int puzzle[m][n], int y, int x, bool solvable); int GetInverse2(vector<vector<int> > sub_puzzle, int r, int c, int y, int x, bool solvable);
vector<int> GetLocation(int puzzle[m][n], int num); vector<int> GetLocation2(vector<vector<int> > sub_puzzle, int v);
int GetHash(vector<vector<int> >); void BBFS(int r, int c, STT* stt, int* hash_array);
void PrintPuzzle(int puzzle[m][n]); int factorial(const int); bool IsSolvable(int puzzle[m][n]); bool IsSolvable2(vector<vector<int> > sub_puzzle);

tuple<vector<vector<int> >, vector<int> >MoveSTT(vector<vector<int> >state, int r, int c, int y, int x, int direct); vector<int> move(int puzzle[m][n], vector<int> blank_loc, int direct);
vector<int> DirectMove(int puzzle[m][n], vector<int> blank_loc, int target_number, vector<int> target_loc, int global_row, int global_col, bool forward, vector<int> fbd_loc, vector<int> fbd_loc2);
vector<int> GetBlankPath(vector<int> blank_loc, vector<int> number_loc, int number_direct, int target_row, int target_col, bool forward, int global_row, int global_col, vector<int> fbd_loc, vector<int> fbd_loc2);
vector<vector<int> > Slice(int puzzle[m][n], int r1, int r2, int c1, int c2); bool in(vector<vector<int> >, int value);
tuple<vector<int>, vector<int> > GetShortestDouble(int puzzle[m][n], int target_row, int t1, int t2,vector<int> loc1,vector<int>loc2); tuple<vector<int>, vector<int> > GetShortestDouble2(int puzzle[m][n], int target_col, int t1, int t2,vector<int> loc1,vector<int>loc2);
vector<int> SelectMovingOrder_3x2(int puzzle[5][6], vector<int> blank_loc, int t1,int t2,vector<int> loc1,vector<int> loc2, int row, vector<int> target_loc1, vector<int> target_loc2); vector<int> SelectMovingOrder_2x3(int puzzle[5][6], vector<int> blank_loc, int t1,int t2,vector<int> loc1,vector<int> loc2, vector<int> target_loc1, vector<int> target_loc2);
vector<int> CheckBlankPath(int puzzle[m][n], vector<int> blank_loc, int target_row, vector<int> loc1, vector<int>loc2); vector<int> CheckBlankPath2(int puzzle[m][n], vector<int> blank_loc, int target_col, vector<int> loc1, vector<int> loc2);
vector<vector<int> > MatchToStandard(vector<vector<int> >sub_puzzle, int t1, int t2, bool last); // last : 마지막 3x2 sub_puzzle
vector<int> GetShortestPath(STT* stt, int* hash_array, vector<vector<int> > initial_state);


STT stt1; STT stt2; //최단거리 참조용 sequence
int hash_array1[721]={0,}; //최단거리 참조용 해쉬
int hash_array2[721]={0,};
int move_count=0; // 이동 횟수
vector<string> move_way; // 이동 경로

int puzzle[5][6] =
{{1,  5,  6, 28, 25, 22},
 { 4, 14, 13, 18,  2,  8},
 { 3, 21,  7, 10, 26, 29},
 {16, 19, 27, 12, 24,  0},
 {15, 11, 17, 20,  9, 23}};

void Solver(int puzzle[5][6]);

int main(){
    chrono::system_clock::time_point startTime = chrono::system_clock::now();
    Solver(puzzle);
    chrono::system_clock::time_point endTime = chrono::system_clock::now();
    chrono::milliseconds millisecond = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
    cout << "- Time Cost: " << millisecond.count() << " ms\n";  
    return 0;
}

void Solver(int puzzle[m][n]){
    cout<<"---------------- Initial State1----------------"<<endl;
        PrintPuzzle(puzzle);
    if (!IsSolvable(puzzle)){
        cout<<"Given puzzle is unsolvable"<<endl;
        return;
    }
    // Make STT
    // if (!(hash_array1[0]==-1)){
    BBFS(3, 2, &stt1, hash_array1);
    BBFS(2, 3, &stt2, hash_array2);
    // } 
    move_way.reserve(512);

    // blank_tile 추적 
    vector<int> blank_loc;
    blank_loc= GetLocation(puzzle, 0);
    // first (m-2) rows
    for (int row=0; row<m-2; ++row){
        // first (n-2) columns
        for(int col=0; col<n-2; ++col){
            int target_number = row*n + col + 1;
            vector<int> target_loc = {row, col};
            // 이미 배치가 잘 되어 있으면 패스
            if (puzzle[target_loc[0]][target_loc[1]] == target_number) continue;
            blank_loc = DirectMove(puzzle, blank_loc, target_number, target_loc, row, col, true, {}, {}); // target_number를 target_loc로 이동
        
            // cout<<"---------------- First State ----------------"<<endl;
            // PrintPuzzle(puzzle);
        }

        // last 2 columns
        // t1,t2를 3x2  puzzle 안에 최단거리로 할당 
        int t1 =row*n + n-1;
        int t2 =  t1+1;
        vector<int> loc1 = GetLocation(puzzle, t1);
        vector<int> loc2 = GetLocation(puzzle, t2);

        if ((loc1[0]==row && loc1[1]==n-2) && (loc2[0]==row && loc2[1]==n-1)) continue; 
        auto [target_loc1, target_loc2] =GetShortestDouble(puzzle, row, t1, t2, loc1, loc2);
        // t1,t2 할당+이동
        blank_loc = SelectMovingOrder_3x2(puzzle, blank_loc, t1, t2, loc1, loc2, row, target_loc1, target_loc2);
        // blank tile 할당 
        vector <int> blank_path = CheckBlankPath(puzzle, blank_loc, row, target_loc1, target_loc2);
        // balnk tile 이동
        
        for(int i=0; i<blank_path.size(); ++i){
            blank_loc = move(puzzle, blank_loc, blank_path[i]);
        }
        // Run STT
        vector<vector<int> > sub_puzzle = Slice(puzzle, row, row+3, n-2, n);
        vector<vector<int> > reg_sub_puzzle = MatchToStandard(sub_puzzle, t1, t2, false);
        vector<int> STT_path = GetShortestPath(&stt1, hash_array1,reg_sub_puzzle);

        for(int i=0; i<STT_path.size(); ++i){
            // t1, t2만 맞춰지면 Stop 
            if ((puzzle[row][n-2] ==t1) &&(puzzle[row][n-1]==t2)){
                break;}
            blank_loc = move(puzzle, blank_loc, STT_path[i]);
        }
    }
    
    // 마지막 두 행
    int target_row = m-3; // 이미 배열 완료된 행
    for(int col=0; col<(n-3); ++col){
        int t1= (target_row+1)*n+col +1 ;
        int t2 = t1 + n;
        
        vector <int> loc1 = GetLocation(puzzle, t1);
        vector <int> loc2 = GetLocation(puzzle, t2); 

        // 해당 열에 숫자 2개가 배치가 잘 되어있을 시 다음 column으로 pass
        if ((loc1[0]==(target_row+1) &&(loc1[0]==col)) && (loc2[0]==(target_row+2)&&(loc2[1]==col)))  continue;
        
        // t1,t2의 최단거리 할당 
        auto [target_loc1, target_loc2] =GetShortestDouble2(puzzle, col,t1, t2, loc1, loc2);
        // t1,t2 이동
        blank_loc = SelectMovingOrder_2x3(puzzle, blank_loc, t1, t2, loc1, loc2, target_loc1, target_loc2);  // 좌측 상단의 타일부터 이동 
        
        // blank_tile 할당 
        vector <int> blank_path = CheckBlankPath2(puzzle, blank_loc, col, target_loc1, target_loc2);
        // blank_tile 이동 
        for(int i=0; i<blank_path.size(); ++i){
            blank_loc = move(puzzle, blank_loc, blank_path[i]);
        }
        // run STT
        vector<vector<int> > sub_puzzle = Slice(puzzle, m-2, m, col, col+3);
        vector<vector<int> > reg_sub_puzzle = MatchToStandard(sub_puzzle, t1, t2, false);
        vector <int> STT_path = GetShortestPath(&stt2, hash_array2,reg_sub_puzzle);

        for(int i=0; i<STT_path.size(); ++i){
            // t1, t2만 맞춰지면 Stop
            if ((puzzle[m-2][col] ==t1) && (puzzle[m-1][col]==t2)) break;
            blank_loc = move(puzzle, blank_loc, STT_path[i]);
        }

        // cout<<"---------------- Middle State ----------------"<<endl;
        // PrintPuzzle(puzzle);
        
    }

    // 마지막 2x3 puzzle
    vector<vector<int> > sub_puzzle = Slice(puzzle, m-2, m, n-3, n);
    int t1 = m*n-n-2; 
    int t2 = t1+n;
    // run STT

    vector<vector<int> > reg_sub_puzzle = MatchToStandard(sub_puzzle, t1, t2, true);
    vector <int> STT_path = GetShortestPath(&stt2, hash_array2,reg_sub_puzzle);

    for(int i=0; i<STT_path.size(); ++i){
        blank_loc = move(puzzle, blank_loc, STT_path[i]);
    }

    cout<<"---------------- Final State ----------------"<<endl;
    PrintPuzzle(puzzle);
    cout<<"---------------------------------------------"<<endl;
    
    cout<<"- Way : "; 
    for(int mv=0; mv<move_way.size(); ++mv) cout<<move_way[mv]<<" ";
    cout<<"\n- Moves: "<< move_count<<" 회"<<endl;

}
    
void BBFS(int r, int c, STT* stt, int* hash_array){ // row, column
 
    int order[4] = {UP, RIGHT, DOWN, LEFT};
    int global_seq=1;

    node init;
    
    if(r==3 && c==2){
        init.state = {{1,2}, {3,4}, {5,0}};
    }
    else{
        init.state = {{1,2,3}, {4,5,0}};
    }
    
    init.y=r-1; init.x=c-1;
    init.hash_value = GetHash(init.state); 
   
    init.next_seq = 1;
    init.prev_seq = -1;
    init.reverse_direct = -1; // Out

    hash_array[init.hash_value]=init.next_seq; // 방문처리
    
    queue<node> que;
    
    que.push(init);
    while(!que.empty()){
        node current = que.front();
        que.pop();
        // STT에 저장
        stt->full_prev_seq[current.next_seq]=current.prev_seq;
        stt->full_reverse_direct[current.next_seq]=current.reverse_direct;
        
        for(int i=0; i<4; ++i){
            node next;
            next.reverse_direct = order[(i+2)%4];
            auto [next_state, next_blank]  = MoveSTT(current.state, r, c, current.y, current.x, order[i]);
            next.state=next_state; next.y=next_blank[0]; next.x=next_blank[1];
            if (next_blank[0]!=-1){
                next.hash_value = GetHash(next_state);
                if (!hash_array[next.hash_value]){
                    next.next_seq= global_seq + 1;
                    global_seq++;
                    next.prev_seq = current.next_seq;
                    que.push(next);
                    hash_array[next.hash_value] = next.next_seq; 
                }
            }

        }
    }
    hash_array[0]=-1; // 체크.
}

// Number [target_number]를 위치 target_loc으로 이동하는 함수
vector<int> DirectMove(int puzzle[m][n], vector<int> blank_loc, int target_number, vector<int> target_loc, int global_row=m-2, int global_col=n-1, bool forward=true, vector<int> fbd_loc={}, vector<int> fbd_loc2={}){
    // initial loc
    vector<int> number_loc = GetLocation(puzzle, target_number);
    vector<int> blank_path;
    blank_path.reserve(16);

    int blank_loc_to_number; int blank_direct_to_number;
    if (number_loc == target_loc) return blank_loc;

    // 좌, 우를 맞추고 위로 가자.
    int lr = target_loc[1] - number_loc[1]; // -4면 왼쪽으로 4칸, +2면 오른쪽으로 2칸
    int up = target_loc[0] - number_loc[0]; // 0이면 그대로, +3이면 위로 3칸, 음수는 나올 일 없(는줄 알았는데 있다. )
    if (lr<0){
        lr *= -1; // 그냥 편의상 양수로.
        blank_loc_to_number = LEFT; // 숫자를 왼쪽으로 가려면 blank tile을 숫자 왼쪽으로 
        blank_direct_to_number = RIGHT; // 숫자가 왼쪽으로 가려면 blank tile을 오른쪽으로 
        }
    else if (lr>0){
        blank_loc_to_number = RIGHT;
        blank_direct_to_number = LEFT;
    }
    else{} 

    for (int j=0; j<lr; j++){// blank_loc, number_loc, DIRECT, target_row, target_col, forward, global_row, globa_col, fbd_loc, fbd_loc2
        blank_path = GetBlankPath(blank_loc, number_loc, blank_loc_to_number, target_loc[0], target_loc[1], forward, global_row, global_col ,fbd_loc, fbd_loc2);
    
        for(int i=0; i<blank_path.size(); ++i){
            blank_loc = move(puzzle, blank_loc, blank_path[i]);
        }
        number_loc = blank_loc; // 숫자가 옮겨질 위치는 move 이전의 blank tilepr
        blank_loc = move(puzzle, blank_loc, blank_direct_to_number);
    }
    // 위로 이동
    if (up<0){
        for(int i=0; i<abs(up); ++i){
            blank_path = GetBlankPath(blank_loc, number_loc, UP, target_loc[0], target_loc[1], forward, global_row, global_col ,fbd_loc, fbd_loc2);
            for(int i=0; i<blank_path.size(); ++i){
                blank_loc = move(puzzle, blank_loc, blank_path[i]);
            }
            
            number_loc = blank_loc; 
            blank_loc = move(puzzle, blank_loc, DOWN);
        }
    }
    else{} // 때로는 아래로 보내야 될 수도 있다. 그래도 낭비를 줄이기 위해 이전단계에서 철저하게 없애주자.  
        
    return blank_loc;
}

vector<int> GetBlankPath(vector<int> blank_loc, vector<int> number_loc, int number_direct, int target_row, int target_col=false, bool forward=true, int global_row=m-3, int global_col=n-2, vector<int> fbd_loc = {}, vector<int> fbd_loc2={}){
    /*
    # number_direct: 즉, 각각 숫자가 가려는 위치와 같음)
    # forward:  앞의 1,2행 처리할 때 Rule
    # target_row : 위치하려는 높이
    # global_row, col : 배치 완료된 행과 열 
    # fbd_loc : 마지막 두 열 동시에 처리할 때, 처음 이동했던 타일은 안 건드리게끔 예외처리(전체적으로 이런 상황이 벌어지지 않게끔 프로그래밍 했으나, 극히 일부 샘플에서 문제를 보임).
    # fbd_loc2 : 마지막 두 행 처리시 어느 다른 하나를 역방향으로 움직이지 않게끔 우회 ("left" 시에만 신경쓰면 된다) 
    */
    vector<int> path;
    path.reserve(16);
    int lr = number_loc[1] - blank_loc[1]; // e.g., -2이면 blank가 2칸 오른쪽에 있다. +1이면 blank가 1칸 왼쪽에 있다. 
    int ud = number_loc[0] - blank_loc[0]; // e.g., -2이면 blank를 2칸 아래쪽에 있다. +1이면 blank가 1칸 위쪽에 있다. 
    
    if(number_direct == LEFT) lr -= 1; // Block tile이 숫자의 좌측에 위치해야 
    else if (number_direct ==RIGHT) lr += 1;
    else if (number_direct ==UP) ud -=1;  // Block tile이 숫자의 위쪽으로 위치해야 (위가 음수임)
    
    // 일단 "left"&"right"에서,  
    if ((number_direct == LEFT) || (number_direct ==RIGHT)){
        // 마지막 두 행에서, 두 타일 모두 최단거리를 보전해주기 위함. 
        if (!fbd_loc2.empty()){// # 마지막 두 행 처리할 때, [[19, *, 0], [*, *, 25]] 꼴이라면 25를 건들지 않는 선에서 0을 19의 왼쪽으로 옮겨주어야 한다.
            if  (fbd_loc2[1]>=(number_loc[1]+1)){//  25가 19의 우측에 존재할 때 
                if (blank_loc[1]>=fbd_loc2[1]){ // 공백이 25를 지나칠 가능성이 있을 경우에만 
                    if ((number_loc[0]==(m-2)) && (fbd_loc2[0]==(m-1))){// 19가 위, 25가 아래에 있다면 (아마 blank path도 거의 위에 있을 것..)
                        for(int i=0; i<abs(lr)-2; ++i) path.push_back(LEFT);
                        lr = lr + (abs(lr)-2); // 가야될 숫자 보정.v
                    }
                    else if ((number_loc[0]==(m-1)) && (fbd_loc2[0]==(m-2))){ // 25가 위, 19가 아래에 있다면
                        path.push_back(DOWN);
                        for(int i=0; i<abs(lr)-2; ++i) path.push_back(LEFT); // 25가 역방향으로 가지 않게 하기 위해 우회를 해줄 필요가 있다. 
                        lr = lr + (abs(lr)-2); 
                        ud -=1;
                        blank_loc = {blank_loc[0]+1, blank_loc[1]-(abs(lr)-2)}; // 얘만 반영 해주어야 한다. 사전 우회임.
                    }
                    else; // 두 숫자의 행이 같을 땐 신경 안 써줘도 된다. 
                }
            }
        }
        if ((target_row==blank_loc[0]) && (number_loc[1]<blank_loc[1]) && (number_loc[1]<global_col)){
            path.push_back(DOWN);
            ud-=1;
        }

        // blank tile과 number tile이 같은 행에 있고(ud==0이고),
        if (number_loc[0] == blank_loc[0]){
            // blank tile이 number tile을 거슬러가야 한다면      
            if ( ( (number_direct == LEFT) && (lr<0) ) || ( (number_direct ==RIGHT) && (lr>0) ) ){ 
                if (blank_loc[0] >(global_row+1)){ // 그리고 이미 맞춰진 행을 건들지 않는 선에서   
                    if ((number_loc[0])==(m-1)){ // # 마지막 행이라면 무조건 위로만 이동. 
                        path.push_back(UP);
                        ud+=1;
                    }
                    else{
                        if (!fbd_loc.empty()){
                            if (fbd_loc[0]==blank_loc[0]-1){ //  위로 갈 수 없는 경우 가면 큰일나는 경우
                                path.push_back(DOWN); // 아래로 피해준다. cout<<"버그찾자"<<endl;
                                ud-=1;
                            }
                            else{
                                path.push_back(UP); // 나머진 위로 피해도 무방.
                                ud+=1; 
                            }
                        }
                        else{ // 마지막 행이 아니라면 너 마음대로 해라.
                            path.push_back(UP);
                            ud+=1;
                        }
                    }
                }
                else{ // 맞춰진 행을 건들 위험성이 있다면, 무조건 아래쪽으로 가야 한다. 
                    path.push_back(DOWN); // 단, 이미 맞춰진 행을 안 건드려
                    ud-=1; // 역시 마지막에 위로 한 번에 더 이동해주어야 함.
                    
                }
            }
        }
        if (!fbd_loc.empty()){
            // blank_tile이 바로 오른쪽으로 못 가고, 움직일 타일이 아래나 오른쪽에 있다면(즉, blank_tile이 오른쪽으로 가야한다면)
            if ((blank_loc[0]==fbd_loc[0]) && ((blank_loc[1]+1)==fbd_loc[1]) && (blank_loc[0]<number_loc[0]) && (blank_loc[1]<=number_loc[1])){
                if ((blank_loc[0]+1)==number_loc[0] && ((blank_loc[1]==number_loc[1]))){ // 특히, 움직일 타일이 바로 아래쪽에 있다면
                    path.insert(path.end(), {LEFT, DOWN, DOWN});
                    lr+=1;
                    ud-=2;
                }
                else if ((blank_loc[0]+1)==number_loc[0]){ // 아니더라도, 바로 아래에 있으면 두 번 우회해야.
                    path.insert(path.end(), {DOWN, DOWN}); 
                    ud-=2l;
                } 
                else{ // 그 외의 경우에는, 알고리즘 시스템 상, 아래로만 미리 한 번 가주면 된다.
                    path.push_back(DOWN);
                    ud-=1;
                }
            }
            // blank_tile이 2칸 이상은 오른쪽으로 못 갈 경우. 
            else if ((blank_loc[0]==fbd_loc[0]) && ((blank_loc[1]+2)<=fbd_loc[1]) && (blank_loc[0]<number_loc[0]) && (blank_loc[1]<number_loc[1])){
                // 우측 하단에 움직일 타일이 있는 경우 
                if (((blank_loc[0]+1)==number_loc[0]) && (blank_loc[1]+1<=number_loc[1])){ //&& (blank_loc[1]+2==fbd_loc[1])){
                    path.insert(path.end(), {DOWN, DOWN});
                    ud-=2;
                }
                
                else if (number_loc[1]>=(fbd_loc[1]-1)){ // fbd를 침범할 가능성이 있는 경우.
                    path.push_back(DOWN);
                    ud-=1;
                }
            }
        }
        // 이제 blank tile과 number tile은 같은 곳에 있지 않다. 좌우로 이동해주면 된다. 
        // 단, blank tile이 맞춰진 행을 건들지 않는 선에서.   
        // 일단, 초반엔 맞춰진 열을 건드리는 경우를 방지하기 위해 처리
        if (global_row<m-2){
            if ((blank_loc[1]==(global_col)) && (blank_loc[0]==global_row)){
                path.push_back(DOWN);
                ud-=1;
            }
        }
        if (lr<0){
            for(int i=0; i<abs(lr); ++i){
                path.push_back(LEFT);
            }
        }
        else if (lr>0){
            for(int i=0; i<lr; ++i){
                path.push_back(RIGHT);
            }
        }
        // 마지막으로 상,하를 맞춰주면 된다.
        if (ud<0){
            for(int i=0; i<abs(ud); ++i){
                path.push_back(UP);
            }
        }
        else if (ud>0){
            for(int i=0; i<ud; ++i){
                path.push_back(DOWN);
            }
        }
    }
    
    // 위로 갈 때
    if (number_direct == UP){
        // blank tile과 number tile이 같은 열에 있고(lr==0이고),
        if (number_loc[1] == blank_loc[1]){  //
            // blank tile이 number tile을 거슬러가야 한다면      
            if (ud<0){ 
                if (blank_loc[1]==n-1){ // 단, 마지막 열이면
                    path.push_back(LEFT); // 왼쪽으로 피해주고 
                    lr+=1; // 마지막에 오른쪽으로 이동 
                }
                else if (!fbd_loc.empty()){ // fbd_loc 존재할 때
                    if (fbd_loc[1]==(blank_loc[1]+1)){ // 오른쪽에 건드려선 안될 애가 있다면
                        path.push_back(LEFT); // 왼쪽으로 우회하자.
                        lr+=1;
                    }
                    else{  
                        path.push_back(RIGHT); // 오른쪽으로 피해주고 
                        lr-=1; // 역시 마지막에 왼쪽으로 이동해주어야 함.     
                    }
                }
                else{
                    path.push_back(RIGHT); // 오른쪽으로 피해주고 
                    lr-=1; // 역시 마지막에 왼쪽으로 이동해주어야 함.     
                }
                }
            }       
        // 이제 blank tile과 number tile은 같은 곳에 있지 않다. 위로만 움직여주자.
        // 바로 위에 정리된 타겟이 있다면?
        if (forward && (number_loc[0]==target_row+1) && blank_loc[1]<=(target_col-1)){
            if (blank_loc[0]==number_loc[0]){ // 행이 같다면? 
                path.push_back(DOWN); // 우회필요
                ud-=1; // 위로 다시 가주어야 함. 
            }
            int temp_gap= target_col - blank_loc[1] +1 ;
            for(int i=0; i<temp_gap; ++i) path.push_back(RIGHT);
            lr-=temp_gap; // 역시 왼쪽으로 다시 가주어야 함.
        }
        if (ud<0){
            for(int i=0; i<abs(ud); ++i){
                path.push_back(UP);
            }
        }
        else if (ud>0){
            for(int i=0; i<ud; ++i){
                path.push_back(DOWN);
            }
        }
        // 마지막으로 좌,우를 맞춰주면 된다.
        // lr이 -3이면 왼쪽으로 3칸 이동, lr이 +2면 오른쪽으로 2칸 이동
        if (lr<0){
            for(int i=0; i<abs(lr); ++i){
                path.push_back(LEFT);
            }
        }
        else if (lr>0){
            for(int i=0; i<lr; ++i){
                path.push_back(RIGHT);
            }
        }
    }
    return path;
}

tuple<vector<int>, vector<int> > GetShortestDouble(int puzzle[m][n], int target_row, int t1, int t2,vector<int> loc1,vector<int>loc2){

    vector <int> target_loc1 = {-1, -1};
    vector <int> target_loc2 = {-1, -1};
    int target_loc1_row, target_loc2_row, target_loc1_col, target_loc2_col,target_loc_row;
    vector <int> temp_in_loc, temp_out_loc, temp_out_target;

    vector<vector<int> > sub_puzzle=Slice(puzzle,target_row, target_row+3, n-2,n);

    // 이미 퍼즐 안에 t1,t2이 들어와 있다면
    if (in(sub_puzzle, t1)&&in(sub_puzzle, t2)){ // 3x2 Puzzle
        if (target_row == (m-3)){ // 마지막 수행할 때
            if ((!(in(sub_puzzle, 0))) && (loc1[1]==n-2) && (loc2[1]==n-2) && (loc1[0]>m-3) && (loc2[0]>m-3)){ // blank가 포함 안 되고, 앞 길을 다 막는다면 
                if (loc1[0]<loc2[0]){ // 위에 있는 친구를
                    target_loc1 = {loc1[0],n-1}; // 끝으로 미뤄준다.
                    target_loc2 = loc2;
                }
                else{
                    target_loc2 = {loc2[0], n-1};
                    target_loc1 = loc1;
                }
            }
            else{
                target_loc1 = loc1;
                target_loc2 = loc2;
            }
        }
        else{
            target_loc1=loc1;
            target_loc2=loc2;
        }
    }
    // 두 숫자가 모두 안 들어와있을 때
    else if ( !((in(sub_puzzle, t1)) || (in(sub_puzzle, t2)) )){
        // 같은 행에 있으면
        if (loc1[0]==loc2[0]){
            // SubPuzzle에 해당하는 행에 없다면
            if (loc1[0]>target_row+2){
                target_loc_row = target_row+2; // Sub puzzle 내 가장 아래로. 
            }
            else{ // 있다면 상하로는 움직이지 않는다. 
                target_loc_row = loc1[0];// 
            }
            if (loc1[1]<loc2[1]){
                target_loc2={target_loc_row-1, n-1}; // 오른쪽에 있는 타일은 맨 끝으로 (단, 서로 건드리지 않게 하기 위해 단차를 주자.)
                target_loc1={target_loc_row, n-2}; // 나머지 하나는 그 왼쪽으로
            }
            else{
                target_loc1={target_loc_row-1, n-1}; // 오른쪽에 있는 타일을 맨 끝으로
                target_loc2={target_loc_row, n-2}; // 나머지 하나는 그 왼쪽으로
            }
        }
        else{ // 다른 행에 있으면
            // t1이 더 위에 있을 때 
            if (loc1[0]<loc2[0]){
                if (loc1[0]>(target_row+2)){ // 높이있는 타일도 SubPuzzle 아래에 있을 때  
                    target_loc1_row = target_row+1;
                    target_loc2_row = target_loc1_row + 1 ;// 그 아래
                }
                else if (loc1[0]<=(target_row+2) && loc2[0]>(target_row+2)){
                    target_loc1_row = target_row+1;  // 2행
                    target_loc2_row = target_loc1_row +1 ;// 그 아래에
                }
                else{ // 두 타일 모두 SubPuzzle과 같은 높이에 있을 때에 (아마 마지막은 항상 여기))
                    if (loc2[1]>=loc1[1]){
                        if (loc2[1]-1 == loc1[1]){ // t2를 옮길 때 t1의 행이 안 바뀐다.
                            target_loc1_row = loc1[0];
                            target_loc2_row = loc2[0];
                        }
                        else{ // 나머지는 t2의 행이 바뀐다.
                            target_loc1_row = loc1[0];
                            target_loc2_row = target_loc1_row ;// t1을 바꾸다보면 t2도 행이 바뀌므로.
                        }
                    }
                    else{
                        vector<int> blank_loc = GetLocation(puzzle, 0);
                        if ((blank_loc[0]==loc1[0]) && (blank_loc[1]==loc2[1])){ // blank_tile 아래에 t1가 있을 때 
                            target_loc1_row = loc1[0];
                            target_loc2_row = loc1[0] ;// 행 역전 
                        }
                        else{
                            target_loc1_row = loc1[0];
                            target_loc2_row = loc2[0];
                        }
                    }   
                }
            }
            else if (loc2[0]<loc1[0]){// t2이 더 위에 있을 때
                if (loc2[0]>(target_row+2)){ // 높이있는 타일도 SubPuzzle 아래에 있을 때  
                    target_loc2_row = target_row+1;
                    target_loc1_row = target_loc2_row + 1 ;// 그 아래 
                }
                else if (loc2[0]<=(target_row+2) && loc1[0]>(target_row+2)){
                    target_loc2_row = target_row+1; // 2행에 위치시켜야.
                    target_loc1_row = target_loc2_row +1; // 그 아래에
                }
                else{ // 두 타일 모두 SubPuzzle과 같은 높이에 있을 때
                    if (loc1[1]>=loc2[1]){ // 그리고 t1이 더 오른쪽에 있을 때. 
                        if (loc1[1]-1 == loc2[1]){ // t2를 옮길 때 t1의 행이 안 바뀌는 경우
                            target_loc2_row = loc2[0];
                            target_loc1_row = loc1[0];
                        }
                        else{ // 나머지는 t1의 행이 바뀐다.
                            target_loc2_row = loc2[0];
                            target_loc1_row = target_loc2_row ;// 같다.
                        }
                    }
                    else{ // 위에 있는 t2가 더 오른쪽에 있을 때
                        vector <int> blank_loc = GetLocation(puzzle, 0);
                        if ((blank_loc[0]==loc2[0]) && (blank_loc[1]==loc1[1])){ // blank_tile 아래에 t1이 있을 때 
                            target_loc2_row = loc2[0];
                            target_loc1_row = loc2[0];// 행 역전 
                        }
                        else{
                            target_loc2_row = loc2[0];
                            target_loc1_row = loc1[0];
                        }
                    }
                }
            }
            else{}
                
            // 혹시 마지막 퍼즐이라면
            if (target_row==m-3){ 
                if (loc1[1]<loc2[1]){ // t2이 더 오른쪽에 있다면
                    target_loc2 = {target_loc2_row, n-1};
                    target_loc1 = {target_loc1_row, n-2};
                }
                else if (loc1[1]>loc2[1]){ // t1이 더 오른쪽에 있다면 
                    target_loc1 = {target_loc1_row, n-1};
                    target_loc2 = {target_loc2_row, n-2};
                }
                else{ // 둘이 같은 수직선상에 있다면, 그냥 위에 있는 친구를 오른쪽으로 몰아준다.
                    if (loc1[0]<loc2[0]){
                        target_loc1 = {target_loc1_row, n-1};
                        target_loc2 = {target_loc2_row, n-2}; // 이 때, target_loc1_row == target_loc2_row이다(어차피 t1를 이동하는 과정에서 t2가 위로 올라오게 된다. )
                    }else{
                        target_loc2 = {target_loc2_row, n-1};
                        target_loc1 = {target_loc1_row, n-2}; // 마찬가지로 target_loc2_row == target_loc1_row
                    }
                }
            }
            else{
                if ((loc1[0]<loc2[0]) && (target_loc1_row==target_loc2_row)){ // 이 때 target_loc1_row와 target_loc2_row는 같다.
                    target_loc1_col = n-1;
                    target_loc2_col = n-2;
                }
                else if ((loc2[0]<loc1[0]) && (target_loc1_row==target_loc2_row)){
                    target_loc2_col = n-1;
                    target_loc1_col = n-2;
                }
                else{// 나머지
                    // 열 배정
                    if (loc1[1]>=n-2){
                        target_loc1_col = loc1[1];
                    }
                    else{
                        target_loc1_col = n-2;
                    }
                    if (loc2[1]>=n-2){
                        target_loc2_col = loc2[1];
                    }
                    else{
                        target_loc2_col = n-2;
                    }
                }
                target_loc1 = {target_loc1_row, target_loc1_col};
                target_loc2 = {target_loc2_row, target_loc2_col};
            }
        }
    }

    else{ // 두 숫자 중 하나만 퍼즐에 들어있을 때
        if (in(sub_puzzle, t1)){
            target_loc1=loc1;
            temp_in_loc = loc1;
            temp_out_loc = loc2;
        }
        else{
            target_loc2=loc2;
            temp_in_loc = loc2;
            temp_out_loc = loc1;
        }
        if (temp_out_loc[0]<=target_row+2){ // out이 SubPuzzle과 같은 높이일 때
            if (temp_in_loc[1]==(n-1)){ // in의 위치가 6열이라면
                temp_out_target = {temp_out_loc[0], n-2};
            }// 5열에 위치
            else{ // in의 위치가 5열이라면
                if (temp_in_loc[0]!=temp_out_loc[0]){ // 서로 행이 다르다면
                    vector<int> blank_loc = GetLocation(puzzle, 0);
                    if ((temp_in_loc[0]!=(target_row))&& (target_row == (m-3))){ // 마지막 3x2 puzzle일 때는 길막 방지
                        if ((temp_in_loc[0] == (m-2))  && (temp_out_loc[1] == blank_loc[1]) && (temp_out_loc[0]>blank_loc[0])){ // 단, 블랭크 타일 아래에 out이 있다면 in을 밀다가 out을 끌어올려버린다.
                            temp_in_loc = {temp_in_loc[0], n-1} ;// in을 6열에 위치
                            temp_out_target = {temp_in_loc[0], n-2};// out을 5열에 위치시키되, in과 같은 행에 배치한다.
                        }
                        else{
                            temp_in_loc = {temp_in_loc[0], n-1};// in을 6열에 위치  
                            temp_out_target = {temp_out_loc[0], n-2};// out을 5열에 위치
                        }
                    }
                    else{
                        temp_out_target = {temp_out_loc[0], n-2};// 5열에 그냥 위치
                    }
                }
                else{ // 서로 행이 같다면
                    temp_in_loc = {temp_in_loc[0], n-1}; // in을 6열에 일치
                    temp_out_target = {temp_out_loc[0], n-2}; // out을 5열에 위치 
                }
            }
        }
        else{ // out이 Subpuzzle 밖에 있을 때(1,2행 풀 때)
            if (temp_in_loc[0] == target_row+2){
                if (temp_in_loc[1] == n-2){ // in이 최하단좌측에 위치한다면
                    temp_in_loc = {temp_in_loc[0]-1, temp_in_loc[1]}; // in을 한 칸 위로 미뤄주고(out에 걸리지 않게)
                    {}
                    if (temp_out_loc[1]!=n-1){ // 마지막 열이 아니라면
                        temp_out_target = {target_row+2, n-2} ;// out을 최하단좌측에 위치 
                    }
                    else{ // 마지막 열이라면
                        temp_out_target = {target_row+2, n-1} ;// 최하단우측에 위치 
                    }
                }
                else{ // in이 최하단우측에 존재한다면
                    if (temp_out_loc[1]==(n-1)){ // out이 최우측에 존재할 때는 
                        temp_in_loc = {temp_in_loc[0]-1, n-1}; // 한 칸 위로 미루고, 
                        temp_out_target = {target_row+2, n-1}; // 최하단좌측에 위치 
                    }
                    else{ // 나머지는
                        temp_out_target = {target_row+2, n-2}; // 최하단 좌측에 위치
                    }
                }
            }
            else{ // in이 최하단에 없을 때는 편하게
                if (temp_out_loc[1]==(n-1)){
                    temp_out_target = {target_row+2, n-1};
                }
                else{
                    temp_out_target = {target_row+2, n-2};
                }
            }
        }
        if (target_loc1[0]==-1 && target_loc1[1]==-1){ // t1이 out이었을 때 
            target_loc2 = temp_in_loc ;
            target_loc1=temp_out_target;
        }
        else{
            target_loc1 = temp_in_loc;
            target_loc2=temp_out_target;
        }
    }
    return {target_loc1, target_loc2};
}

// 2x3 퍼즐을 풀 때 t1, t2를 최단거리로 위치시키는 함수.
tuple<vector<int>, vector<int> >GetShortestDouble2(int puzzle[m][n], int target_col, int t1,int t2, vector<int> loc1,vector<int> loc2){
    int r =2; int c = 3;
    vector <int>target_loc1 = {-1, -1};
    vector <int>target_loc2 = {-1, -1};
    vector <int> temp_blank_loc;
    vector <int> temp_in_loc;
    vector <int> temp_out_loc;
    vector <int> temp_out_target;
    vector<vector <int> > sub_puzzle = Slice(puzzle, m-2, m, target_col, target_col+3);

    // 이미 퍼즐 안에 t1,t2이 들어와 있다면
    if (in(sub_puzzle, t1)&&in(sub_puzzle, t2)){ // 2x3 Puzzle 
        if (in(sub_puzzle, 0)){
            target_loc1 = loc1;
            target_loc2 = loc2;
        }
        else{ // 0이 없다면, 0이 들어갈 자리를 내주어야 한다. 
            if (loc1[1]!=loc2[1]){
                target_loc1=loc1;
                target_loc2=loc2;
            }
            else if ((loc1[1]==loc2[1]) && (loc1[1]==(target_col+2))){ // 길을 막고 있다면
                if (loc1[0]<loc2[0]){ // 위에 있는 친구를 한칸 더 미뤄준다.
                    target_loc1 = {loc1[0], target_col+1};
                    target_loc2 = loc2;
                }
                else{
                    target_loc2 = {loc2[0], target_col+1};
                    target_loc1 = loc1;
                }
            }
            else{ // 길을 안 막고 있을 때
                target_loc1 = loc1;
                target_loc2 = loc2;
            }
        }
    }
    // 두 숫자가 모두 안 들어와있을 때
    else if (!((in(sub_puzzle, t1)||in(sub_puzzle,t2)))){
        // 같은 행에 있으면
        if (loc1[0]==loc2[0]){
            if (loc1[1]<loc2[1]){ // 그대로 평행이동
                target_loc1 = {loc1[0], target_col+1};
                target_loc2 = {loc2[0], target_col+2};
            }
            else{
                target_loc2 = {loc2[0], target_col+1};
                target_loc1 = {loc1[0], target_col+2};
            }
        }
        else{ // 다른 행에 있으면
            temp_blank_loc = GetLocation(puzzle, 0);  //(공백이 왼쪽에 있을 때랑, 오른 쪽에 있을 때랑 다르다.
            if (loc1[1]==loc2[1]){ // 같은 열에 있으면
                if (loc1[0]<loc2[0]){ // 위에 있는 친구부터 옮긴다. 
                    if (temp_blank_loc[1]<loc1[1]){ // 만일 공백이 왼쪽에 있다면, 
                        target_loc1 = {loc1[0], target_col+1};
                        target_loc2 = {loc1[0], target_col+2}; // 행 변환이 일어난다.
                    }
                    else if(temp_blank_loc[1]>loc1[1]){ // 공백이 오른쪽에 있다면, 
                        target_loc1= {loc1[0], target_col+1};
                        target_loc2= {loc2[0], target_col+2}; // 행 변환이 일어나지 않는다(뒤로 간).
                    }
                    else{}
                }
                else{
                    if (temp_blank_loc[1]<loc2[1]){
                        target_loc2 = {loc2[0], target_col+1};
                        target_loc1 = {loc2[0], target_col+2};  // 단, 이 때 행 변환이 일어난다.
                    }
                    else if(temp_blank_loc[1]>loc2[1]){ 
                        target_loc2= {loc2[0], target_col+1};
                        target_loc1= {loc1[0], target_col+2}; // 행 변환이 일어나지 않는다.
                    }
                }
            }
            
            else{// 다른 열에 있으면 
                if (loc1[1]<loc2[1]){ // 왼쪽에 있는 친구부터 옮긴다.
                    if (temp_blank_loc[1]>loc1[1]){ // 공백이 오른 쪽에 있을 때 
                        if (loc1[1]==(loc2[1]-1)){ // t2의 행 바뀐다(단, t2 안 건들게 우회시켜줄 때 - fbd_loc2 in GetBlankPath + 공백이 더 오른 쪽에 있을 때 
                            target_loc1 = {loc1[0], target_col+1}; 
                            target_loc2 = {loc1[0], target_col+2};
                        }
                        else{
                            target_loc1 = {loc1[0], target_col+1};
                            target_loc2 = {loc2[0], target_col+2};    
                        }
                    }
                    else{ // 공백이 왼 쪽에 있거나 같을 때 
                        target_loc1 = {loc1[0], target_col+1};
                        target_loc2 = {loc2[0], target_col+2}; 
                    }
                }
                else{
                    if (temp_blank_loc[1]>loc2[1]){
                        if (loc2[1]==(loc1[1]-1)){ // t1의 행 바뀐다. 
                            target_loc2 = {loc2[0], target_col+1}; 
                            target_loc1 = {loc2[0], target_col+2};
                        }
                        else{
                            target_loc2 = {loc2[0], target_col+1};
                            target_loc1 = {loc1[0], target_col+2};
                        }
                    }
                    else{
                        target_loc2 = {loc2[0], target_col+1};
                        target_loc1 = {loc1[0], target_col+2};
                    }
                }
            }
        }
    }
                    
    else{ // 두 숫자 중 하나만 퍼즐에 들어있을 때
        if (in(sub_puzzle, t1)){
            target_loc1=loc1;
            temp_in_loc = loc1;
            temp_out_loc = loc2;
        }
        else{
            target_loc2=loc2;
            temp_in_loc = loc2;
            temp_out_loc = loc1;
        }
        if (temp_in_loc[1]<=(target_col+1)){ // 좌측 2개 열에 들어있는 경우 
            temp_out_target = {temp_out_loc[0], target_col+2};
        }
        else{ // 3번째 열에 들어가 있는 경우
            temp_in_loc = {temp_in_loc[0], target_col+1};// in을 한 칸 미뤄주고, 
            temp_blank_loc = GetLocation(puzzle, 0);

            if (temp_in_loc[1]<temp_blank_loc[1]){
                if ((temp_in_loc[0]!=temp_out_loc[0]) && ((temp_in_loc[1]+1)==(temp_out_loc[1]-1))){// 역전현상 일어나는 경우
                    temp_out_target = {temp_in_loc[0], target_col+2};
                }
                else{ 
                    temp_out_target = {temp_out_loc[0], target_col+2}; // out을 끌고온다.
                }
            }
            else{  // 공백이 out을 건드릴 일이 없다면
                temp_out_target = {temp_out_loc[0] ,target_col+2};
            }
        }
     
        // target 할당
        if (target_loc1[0]==-1 && target_loc1[1]&&-1){
            target_loc1 = temp_out_target;
            target_loc2 = temp_in_loc;
        }
        else{
            target_loc2 = temp_out_target;
            target_loc1 = temp_in_loc;
        }
    }
    return {target_loc1, target_loc2};
}

void PrintPuzzle(int puzzle[m][n]){
    for (int i=0; i<m; ++i){
        for (int j=0; j<n; ++j){
            cout<<puzzle[i][j]<<" ";
        }
        cout<<endl;
    }
}
// 3x2 퍼즐용 Blank Tile이 최단거리로 들어갈 위치 찾기.
vector<int> CheckBlankPath(int puzzle[m][n], vector<int> blank_loc, int target_row, vector<int> loc1, vector<int>loc2){
    vector<vector<int> > sub_puzzle = Slice(puzzle, target_row, target_row+3, n-2, n);
    vector<int> path;
    vector<int> temp_low_loc, temp_high_loc, temp_blank_loc;
    int temp_gap; 
    // blank도 들어가 있으면 끝
    if (in(sub_puzzle, 0)){
        return path;
    }
    
    // 일단 t1,t2이 Subpuzzle의 마지막 2행에 위치할 때(일반적인 경우) 와.. 비교
    if (((target_row<loc1[0])&&(loc1[0]<=(target_row+2))) && ((target_row<loc2[0]) && (loc2[0]<=(target_row+2)))){
        if(loc1[0]==loc2[0]){ // t1, t2이 같은 행에 있다면
            if (blank_loc[0]==loc1[0]){ // 공백도 같은 행에 있다면 
                if (blank_loc[0]==(target_row+1)){
                    if (blank_loc[1]==n-3){
                        return {DOWN, RIGHT};
                    }
                    else{
                        temp_gap = (n-2) - blank_loc[1];
                        path.push_back(DOWN);
                        for(int i=0; i<temp_gap; ++i) path.push_back(RIGHT);
                        return path;
                    }
                }
                else if (blank_loc[0]==(target_row+2)){
                    return {UP, RIGHT};
                }
            }
            else{// 공백과 숫자들이 다른 행에 있다면
                if (blank_loc[0]<=(target_row+2)){ // Subpuzzle과 같은 높이에 있을 때 
                    temp_gap = (n-2) - blank_loc[1];
                    for(int i=0; i<temp_gap; ++i) path.push_back(RIGHT);
                    return path; // n-1열 까지만 위치시키면 된다. (어차피 공백은 항상 4열쯤에 있다 - 가 너무 안일한 생각이었다. 경우의수가 10^18개인데..)
                }
                else if (blank_loc[0]==(target_row+3)){ // 다른 높이에 있을 시 돌아간다.
                    
                    if (blank_loc[1]==(n-1)){
                        return {LEFT, LEFT ,UP, UP, RIGHT}; // Check ([[*, *], [*, *], [t1, t2])이고, 공백이 t2 아래에 있을 때 
                    }else{ // 위에서, 공백이 t1 아래에 있을 때 
                        return {LEFT, UP, UP, RIGHT};
                    }
                }    
                else{}  
            }
        }
        else{ // t1, t2이 다른 행에 있다
            
            if (loc1[0]>loc2[0]){ // t1이 더 아래에 있다면
                temp_low_loc=loc1;
                temp_high_loc=loc2;
            }
            else{
                temp_low_loc=loc2;
                temp_high_loc=loc1;
            }
            // loc1이 낮은 숫자이다. loc1 대신 low_loc, lo2대신 high_loc
            if (temp_low_loc[1]==(n-2)){ // low { 5열 
                if (temp_high_loc[1] == (n-2)){ // low { 5열, high { 5열 
                    return GetBlankPath(blank_loc, temp_low_loc, RIGHT, temp_low_loc[0], false, true, m-3, n-2, temp_high_loc);
                }
                else{ // low { 5열, high { 6열 
      
                    if (blank_loc[0]==(temp_low_loc[0]+1)){ // 공백이 t1 아래에 있다면 
                        return GetBlankPath(blank_loc, temp_low_loc, RIGHT, temp_low_loc[0]);
                    }
                    else if (blank_loc[1]==(temp_low_loc[1]-1)){ // 공백이 t1 왼쪽에 있다면? t1의 위로. 
                        return GetBlankPath(blank_loc, temp_low_loc, UP, temp_low_loc[0]-1); // 위로 길을 열어준다.
                    }else{
                        int temp_gap = (n-2) - blank_loc[1];
                        for(int i; i<temp_gap; ++i) path.push_back(RIGHT);
                        return path;

                    }

                }
            }           
            else if (temp_low_loc[1] ==(n-1)){ // t1이 6열에 있다면?
                if (blank_loc[0]==(temp_low_loc[0]+1)){ // 공백이 t1 아래에 있다면
                    return GetBlankPath(blank_loc, temp_low_loc, LEFT, temp_low_loc[0]);
                }
                else if (blank_loc[1]==(temp_high_loc[1]-1)){ // 공백이 t2 왼쪽에 있다면
                    return {DOWN, RIGHT};
                }
                else if (blank_loc[0]==temp_low_loc[0]-1){
                    int temp_gap = (n-2) - blank_loc[1];
                    path.push_back(DOWN);
                    for(int i=0; i<temp_gap; ++i){
                        path.push_back(RIGHT);
                    }
                    return path;
                }
                else if (temp_high_loc[1]==(n-1)){ // t2도 6열에 있다면? 
                    if (blank_loc[0]==temp_high_loc[0]){
                        return GetBlankPath(blank_loc, temp_high_loc, LEFT, temp_high_loc[0]);
                    }
                    else if (blank_loc[0]==temp_low_loc[0]){   // 이런 건 나올 일 거의 없을 것 같은데, 혹시 모르니 해주자. 
                        return GetBlankPath(blank_loc, temp_low_loc, LEFT, temp_low_loc[0]);
                    }
                    else{
                        throw out_of_range("Check Blank tile");
                    }
                }
            }
        }
    }
    else if ( ( (blank_loc[0]-1)==loc1[0] && blank_loc[1]==loc1[1]) || ((blank_loc[0]-1)==loc2[0] && blank_loc[1]==loc2[1])) { // 위쪽에 누가 있을 때 (Up tile)
        if (blank_loc[1]==n-2){
            return {RIGHT, UP};
        }
        else{  // blank_loc[1]==n-1
            return {LEFT, UP};
        }
    }
    else{ // 그 외 Sub puzzle의 왼쪽 공간에 blank_tile이 위치할 때  
        temp_gap = (n-2) - blank_loc[1];
        temp_blank_loc = {blank_loc[0], blank_loc[1]+temp_gap};
        if ((temp_blank_loc[0]==loc1[0] && temp_blank_loc[1]==loc1[1]) or (temp_blank_loc[0]==loc2[0] && temp_blank_loc[1]==loc2[1])){
            if (blank_loc[0]== (target_row+1)){
                path.push_back(DOWN); for(int i=0; i<temp_gap; ++i) path.push_back(RIGHT);
                return path;
            }
            else{
                path.push_back(UP); for(int i=0; i<temp_gap; ++i) path.push_back(RIGHT);
                return path;
          
            }
        }

        else{
            for(int i=0; i<temp_gap; ++i)
            path.push_back(temp_gap);
            return path;
        }  
    }
}
// 2x3 퍼즐용 Blank Tiile 찾기.
vector<int> CheckBlankPath2(int puzzle[m][n], vector<int> blank_loc, int target_col, vector<int> loc1, vector<int> loc2){
    vector<vector<int> > sub_puzzle = Slice(puzzle, m-2, m, target_col, target_col+3);
    vector<int> path;
    // blank도 들어가 있으면 끝
    if (in(sub_puzzle, 0)) return path;

    int target_loc_col = target_col+2;
    int gap_to_target = blank_loc[1]-target_loc_col; // 왼쪽으로 gap_to_target만큼 가야한다. 
    if (blank_loc[0]==(m-2)){ // 공백이 4행에 있고
        // 같은 행에 누군가가 가로막고 있다면
        if (((blank_loc[0]==loc1[0]) && (target_loc_col == loc1[1])) || ((blank_loc[0]==loc2[0]) && (target_loc_col ==loc2[1]))){
            path.push_back(DOWN);
            for(int i=0; i<gap_to_target; ++i) path.push_back(LEFT);
            return path;
            }
        else{ // 안 가로막고 있다면
            for(int i=0; i<gap_to_target; ++i){
                path.push_back(LEFT);
            } return path;
        }
    }
     
    else{ // 공백이 5행에 있고
        if ((blank_loc[0]==loc1[0] && target_loc_col==loc1[1]) || (blank_loc[0]=loc2[0] && target_loc_col==loc2[1])){
            path.push_back(UP);
            for(int i=0; i<gap_to_target; ++i) path.push_back(LEFT);
            return path;
        }
        else{ // 안 가로막고 있다면
            for(int i=0; i<gap_to_target; ++i) path.push_back(LEFT);
            return path;
            }
        }
}




vector<int> GetLocation(int puzzle[m][n], int v){
    for(int i=0; i<m; ++i){
        for(int j=0; j<n; ++j)

        if (puzzle[i][j]==v){
            return {i, j};
        }
    }
}

vector<int> GetLocation2(vector<vector<int> > sub_puzzle, int v){
    
    int r=sub_puzzle.size(); int c=sub_puzzle[0].size();
    for(int i=0; i<r; ++i){
        for(int j=0; j<c; ++j)

        if (sub_puzzle[i][j]==v){
            return {i, j};
        }
    }
}

bool IsSolvable(int puzzle[m][n]){
    vector <int> loc;
    loc=GetLocation(puzzle, 0); // blanktile의 row (1부터 시작)
    int bi = loc[0]+1;
    int total_inverse = 0;
    int inverse_value;

    for(int i=0; i<m; ++i){
        for(int j=0; j<n; ++j){
            inverse_value=GetInverse(puzzle, i,j, true);
            total_inverse+=inverse_value;
        }
    }        
            
    if((n%2) && !(total_inverse%2)){ // n이 홀수이면, inver는 짝수
        return true;
    }
    else if (!(n%2) && ((m-bi)%2 ==(total_inverse%2))){ // blank tile이 (아래에서부터) 홀수에 위치하면 inverse는 짝수
        return true;
    } else return false;
        
}

bool IsSolvable2(vector<vector<int> > sub_puzzle){
    vector <int> loc;
    int r=sub_puzzle.size();
    int c=sub_puzzle[0].size();
    loc=GetLocation2(sub_puzzle, 0); // blanktile의 row (1부터 시작)
    int bi = loc[0]+1;
    int total_inverse = 0;
    int inverse_value;
    
    for(int i=0; i<r; ++i){
        for(int j=0; j<c; ++j){
            inverse_value=GetInverse2(sub_puzzle, r,c,i,j, true);
            total_inverse+=inverse_value;
        }
    }        
            
    if((c%2) && !(total_inverse%2)){ // n이 홀수이면, inver는 짝수
        return true;
    }
    else if (!(c%2) && ((r-bi)%2 ==(total_inverse%2))){ // blank tile이 (아래에서부터) 홀수에 위치하면 inverse는 짝수
        return true;
    } else return false;
        
}

// STT를 이용해 3x2 Sub-puzzle을 최단경로로 풀어주는 함수      
vector<int> GetShortestPath(STT* stt, int* hash_array, vector<vector<int> > initial_state){
    int next_seq, prev_seq, next_direct;
    int hash_value = GetHash(initial_state);
    next_seq=hash_array[hash_value]; // initial state
    prev_seq = stt->full_prev_seq[next_seq]; // next seq -> prev seq ()
    vector<int> full_direct;
    full_direct.reserve(16);
    while (prev_seq!=-1){ 
        next_direct = stt->full_reverse_direct[next_seq];
        full_direct.push_back(next_direct);
        
        next_seq = prev_seq;
        prev_seq = stt->full_prev_seq[next_seq];
    }
    
    return full_direct;
}

int GetInverse(int puzzle[m][n], int y, int x, bool solvable=false){

    int anchor_value = puzzle[y][x]; // 타일의 값
    int count = 0;
    
    for(int i=y; i<m; ++i){
        if (i==y){ 
            for(int j=(x+1); j<n; j++){
                if(puzzle[i][j] < anchor_value){
                    if (solvable){
                        if (puzzle[i][j]!=0){
                            count++;
                        }
                    }
                    else{
                        count++;
                    }
                }
            }
        }
        else{
            for(int j=0; j<n; ++j){
                if(puzzle[i][j] < anchor_value){
                    if (solvable){
                        if(puzzle[i][j]!=0){
                            count++;
                        }
                    }
                    else{
                        count++;
                    }
                }
            }
        }
    }
    return count;

}

int GetInverse2(vector<vector<int> > puzzle, int r, int c, int y, int x, bool solvable=false){

    int anchor_value = puzzle[y][x]; // 타일의 값
    int count = 0;
    
    for(int i=y; i<r; ++i){
        if (i==y){ 
            for(int j=(x+1); j<c; j++){
                if(puzzle[i][j] < anchor_value){
                    if (solvable){
                        if (puzzle[i][j]!=0){
                            count++;
                        }
                    }
                    else{
                        count++;
                    }
                }
            }
        }
        else{
            for(int j=0; j<c; ++j){
                if(puzzle[i][j] < anchor_value){
                    if (solvable){
                        if(puzzle[i][j]!=0){
                            count++;
                        }
                    }
                    else{
                        count++;
                    }
                }
            }
        }
    }
    return count;
}

vector<int> SelectMovingOrder_3x2(int puzzle[5][6], vector<int> blank_loc, int t1,int t2,vector<int> loc1,vector<int> loc2, int row, vector<int> target_loc1, vector<int> target_loc2){
    if (row<m-3){
        if (loc1[0]<loc2[0]){
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false);
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false,  target_loc1);
        }else if (loc1[0]>loc2[0]){
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false);
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false,  target_loc2);
            
        }else{ 
            if (loc1[1]<loc2[1]){ // 만일 같다면 오른쪽에 있는 친구부터. 
                blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false);
                blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false,  target_loc2);
            }else{
                blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false);
                blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false,  target_loc1);
            }
        }
    }
    else{ // row == m-3일 때(마지막 3x2)는 오른쪽부터 움직이는 게 낫다.
        if (loc1[1]<loc2[1]){
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false);
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false);//,  target_loc2) ;
        }else if(loc1[1]>loc2[1]){;
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false);
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false);//target_loc1) ;
        }else{ // 같다면 위쪽부터 이동하는 게 나음.
            if (loc1[0]<loc2[0]){
                blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false);
                blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false);//  target_loc1); 
            }if (loc1[0]>loc2[0]){
                blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, row, n-2, false);
                blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, row, n-2, false);//  target_loc2) ;
            }
        }
    }
    return blank_loc;

}
vector<int> SelectMovingOrder_2x3(int puzzle[5][6], vector<int> blank_loc, int t1,int t2,vector<int> loc1,vector<int> loc2, vector<int> target_loc1, vector<int> target_loc2){
    if (loc1[1]<loc2[1]){
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, m-3,n-1,false, {}, loc2);
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, m-3,n-1,false);
    }
    else if (loc1[1]>loc2[1]){
        blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, m-3,n-1,false, {},loc1);
        blank_loc = DirectMove(puzzle,  blank_loc, t1, target_loc1, m-3,n-1,false);
    }
    else{
        if (loc1[0]>loc2[0]){
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, m-3,n-1,false, {},loc1);
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, m-3,n-1,false);
        }
        else{
            blank_loc = DirectMove(puzzle, blank_loc, t1, target_loc1, m-3,n-1,false, {}, loc2);
            blank_loc = DirectMove(puzzle, blank_loc, t2, target_loc2, m-3,n-1,false);  
        }
    }    
    return blank_loc;
}


// 3x2 puzzle을 풀기위해 [1,2,3,4,5,0] 형태로 정규화가 필요함.
vector<vector<int> > MatchToStandard(vector<vector<int> >sub_puzzle, int t1, int t2, bool last=false){ // last : 마지막 3x2 sub_puzzle
    // 단, solvable하게.
    int r =sub_puzzle.size(); int c = sub_puzzle[0].size();
    vector<vector<int> > reg_sub_puzzle = sub_puzzle;
    vector<int> others;

    if (r==3 && c==2){
         others = {3,4,5}; // for solvable
    } 
    else{ // 아마 2x3 퍼즐만.
        if (!last){
            others = {2,3,5}; // Solvable
        }else{ // last
        }
    }
    int idx=0;  
    for(int i=0; i<r; ++i){
        for(int j=0; j<c; ++j){
            if (sub_puzzle[i][j] == t1){
                reg_sub_puzzle[i][j] = 1;
            }
            else if (sub_puzzle[i][j]==t2){
                if (r==3&&c==2){
                    reg_sub_puzzle[i][j] = 2;
                }else{ 
                    reg_sub_puzzle[i][j] = 4;
                }
            }
                    
            else if (sub_puzzle[i][j]==0){
                reg_sub_puzzle[i][j] = 0;
            }else{ 
                if (!last){
                    reg_sub_puzzle[i][j] = others[idx];
                    idx++;

                    
                }
                if (last){ // 정해진 매칭(자동으로 solvable)
                    if (sub_puzzle[i][j]==(m*n-n-1)){
                        reg_sub_puzzle[i][j] = 2;
                    }else if (sub_puzzle[i][j]==(m*n-n)){
                        reg_sub_puzzle[i][j] = 3;
                    }
                    else{ // 29(mn-1)
                        reg_sub_puzzle[i][j] = 5;
                    }
                }
            }
        }
    }
    if (!IsSolvable2(reg_sub_puzzle) && !last){
        
        vector <int> loc2 = GetLocation2(reg_sub_puzzle, others[0]);
        vector <int> loc5 = GetLocation2(reg_sub_puzzle, others[2]);

       
        swap(reg_sub_puzzle[loc2[0]][loc2[1]], reg_sub_puzzle[loc5[0]][loc5[1]]);


        if (!IsSolvable2(reg_sub_puzzle)){
            throw out_of_range("Match To Standard에서 정규화오류");
        }
    }
    return reg_sub_puzzle; 
}

// SST table 만들기용 move 함수 (C++에선 Struct)
tuple<vector<vector<int> >, vector<int> > MoveSTT(vector<vector<int> > state2, int r, int c, int y,int x, int direct){
    vector<vector<int> > state = state2;
    
    if (direct==LEFT){
        if (x-1<0) return {state, {-1,-1}};
        else{
            swap(state[y][x], state[y][x-1]);
            return {state, {y, x-1}};
        }
    }

    else if (direct==RIGHT){
        if (x+1>=c) return {state, {-1,-1}};
        else{
            swap(state[y][x], state[y][x+1]);
            return {state, {y, x+1}};
        }
    }

    else if (direct==UP){
        if (y-1<0) return {state, {-1,-1}};
        else{   
            swap(state[y][x], state[y-1][x]);
            return {state, {y-1, x}};
        }
    }

    else if (direct==DOWN){
        if (y+1>=r) return {state, {-1,-1}};
        else{
            swap(state[y][x], state[y+1][x]);
            return {state, {y+1,x}};
        }
    }

}

// 기본적인 in-plaace moving
vector<int> move(int puzzle[m][n], vector<int>blank_loc, int direct){
    int y=blank_loc[0]; int x=blank_loc[1];

    move_count+=1;
    
    if (direct==LEFT){
        if (x-1<0) throw out_of_range("이동 불가");
        else{
            swap(puzzle[y][x], puzzle[y][x-1]);
            move_way.push_back("L");
            // cout<<"LEFT"<<" ";
            return {y, x-1};
        }
    }

    else if (direct==RIGHT){
        if (x+1>=n) throw out_of_range("이동 불가");
        else{
            swap(puzzle[y][x], puzzle[y][x+1]);
             move_way.push_back("R");
            //  cout<<"Right"<<" ";
            return {y, x+1};
        }
    }

    else if (direct==UP){
        if (y-1<0) throw out_of_range("이동 불가");
        else{   
            swap(puzzle[y][x], puzzle[y-1][x]);
            move_way.push_back("U");
            // cout<<"Up"<<" ";

            return {y-1, x};
        }
    }

    else if (direct==DOWN){
        if (y+1>m) throw out_of_range("이동 불가");
        else{
            swap(puzzle[y][x], puzzle[y+1][x]);
            move_way.push_back("D");
            // cout<<"Down"<<" ";
            return {y+1,x};
        }
    }

}

vector<vector<int> > Slice(int puzzle[m][n], int r1, int r2, int c1, int c2){
    vector<vector<int> > sub_puzzle;
    for(int i=r1; i<r2; ++i){
        vector<int> row;

        for(int j=c1; j<c2; ++j){
            row.push_back(puzzle[i][j]);
        }
        sub_puzzle.push_back(row);
    }
    return sub_puzzle;
}

bool in(vector<vector<int> > arr2d, int value){
    
    for(int i=0; i<arr2d.size(); i++){
        for(int j=0; j<arr2d[0].size(); j++){
            if (arr2d[i][j]==value) return true; 
        }
    }
    return false;
}

int GetHash(vector<vector<int> > sub_puzzle){    
    int r = sub_puzzle.size(); int c = sub_puzzle[0].size();
    int hash_value = 1;
    int i = 0;
    int temp_inverse=0;

    for(int row=r-1; row>-1; --row){ //range(m-1, -1, -1):
        for(int col=c-1; col>-1; --col){//col in range(n-1, -1, -1):
            temp_inverse = GetInverse2(sub_puzzle,r, c, row, col, false);
            hash_value = hash_value +(temp_inverse * factorial(i));
            i++;
        }
    }

    return hash_value;
}
int factorial(const int a)
{
    int f = 1;
    for (int i=1; i<=a; ++i)
        f *= i;
    return f;
}
