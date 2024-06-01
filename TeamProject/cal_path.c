#include "all_header.h"

bool isValid(Point p) {
    return (p.x >= 0 && p.x < MAP_ROW&& p.y >= 0 && p.y < MAP_COL);
}

Point* Bangaljook(int opp_x, int opp_y, int my_x, int my_y, int* count) {
    static Point all_points[25] = {
        {0,0}, {0,1}, {0,2}, {0,3}, {0,4},
        {1,0}, {1,1}, {1,2}, {1,3}, {1,4},
        {2,0}, {2,1}, {2,2}, {2,3}, {2,4},
        {3,0}, {3,1}, {3,2}, {3,3}, {3,4},
        {4,0}, {4,1}, {4,2}, {4,3}, {4,4}
    };

    float bangal_x = (opp_x + my_x) / 2.0;
    float bangal_y = (opp_y + my_y) / 2.0;
    float geeoolgii;

    if (my_x == opp_x && my_y == opp_y) {
        *count = 25; // Return all points if the coordinates are the same
        return all_points;
    }
    else if (my_x == opp_x) {
        geeoolgii = 10000; // Vertical bisector slope is infinite
    }
    else if (my_y == opp_y) {
        geeoolgii = 0; // Horizontal bisector slope is 0
    }
    else {
        float dx = my_x - opp_x;
        float dy = my_y - opp_y;
        geeoolgii = -dx / dy;
    }

    int my_part = (my_y >= geeoolgii * (my_x - bangal_x) + bangal_y) ? 1 : -1;
    Point* return_points = (Point*)malloc(25 * sizeof(Point));
    *count = 0;

    for (int i = 0; i < 25; ++i) {
        int x = all_points[i].x;
        int y = all_points[i].y;
        int part = (y >= geeoolgii * (x - bangal_x) + bangal_y) ? 1 : -1;
        // 현재 위치는 포함시키지 않는다
        if (part == my_part && !(all_points[i].x == my_x && all_points[i].y == my_y)) {
            return_points[(*count)++] = all_points[i];
        }
    }

    return_points = (Point*)realloc(return_points, (*count) * sizeof(Point));
    return return_points;
}

// StartPoint로부터 BFS 를 통해 가까운 점부터 차례대로 방문하여 주어진 지점의 점수가 4면 해당 지점을 즉시 반환하고 
// 그렇지 않으면 최고 점수를 가진 지점을 반환하는 함수
Point* Find_MaxScorePoint(Point* StartPoint, Point* points, int count) {
    Point directions[4] = { {0,1}, {1,0}, {0,-1}, {-1,0} };
    bool visited[MAP_ROW][MAP_COL] = { false };
    QueueNode queue[MAX_QUEUE_SIZE];
    int front = 0, rear = 0;

    queue[rear++] = (QueueNode){ *StartPoint, 0 };
    visited[(*StartPoint).x][(*StartPoint).y] = true;

    Point* returnpoint = StartPoint;
    int currmaxscore = DGIST_OBJ.map[(*StartPoint).x][(*StartPoint).y].item.score;

    while (front < rear) {
        QueueNode current = queue[front++];
        Point* currpoint = &(current.point);

        // 점수가 4면 이미 최대이므로 그냥 리턴을 한다
        if (DGIST_OBJ.map[(*currpoint).x][(*currpoint).y].item.score == MAX_SCORE) {
            return currpoint;
        }

        // 현재 점수가 최고 점수보다 크면 갱신
        if (DGIST_OBJ.map[(*currpoint).x][(*currpoint).y].item.score > currmaxscore) {
            returnpoint = currpoint;
            currmaxscore = DGIST_OBJ.map[(*currpoint).x][(*currpoint).y].item.score;
        }

        // 다음 지점을 큐에 추가
        for (int i = 0; i < 4; ++i) {
            Point nextpoint = { (*currpoint).x + directions[i].x, (*currpoint).y + directions[i].y };
            if (isValid(nextpoint) && !visited[nextpoint.x][nextpoint.y]) {
                visited[nextpoint.x][nextpoint.y] = true;
                queue[rear++] = (QueueNode){ nextpoint, current.distance + 1 };
            }
        }
    }

    // 점수가 4인 게 없다면 최대 점수를 가진 포인트를 리턴한다
    return returnpoint;
}
//포인터 복사
void copy_path(Path* dest, Path* src) {
    dest->length = src->length;
    dest->score = src->score;
    memcpy(dest->points, src->points, src->length * sizeof(Point));
}
//최적의 경로 탐색해서 best_path에 저장
void find_paths(int row_moves, int column_moves, Path* path, int current_score, Path* best_path, int start_x, int start_y) {
    int max_score = -10;
    if (row_moves == 0 && column_moves == 0) {
        // 경로가 완성된 경우
        if (current_score > max_score) {
            max_score = current_score;
            copy_path(best_path, path);
        }
    }
    //x방향 양의 이동
    if (row_moves > 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x + 1, (*path).points[(*path).length - 1].y };
        (*path).length++;
        start_x++;
        find_paths(row_moves - 1, column_moves, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_x--;
    }
    //y방향 양의 이동
    if (column_moves > 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x, (*path).points[(*path).length - 1].y + 1 };
        (*path).length++;
        start_y++;
        find_paths(row_moves, column_moves - 1, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_y--;
    }
    //x방향 음의 이동
    if (row_moves < 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x - 1, (*path).points[(*path).length - 1].y };
        (*path).length++;
        start_x--;
        find_paths(row_moves + 1, column_moves, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_x++;
    }
    //y방향 음의 이동
    if (column_moves < 0) {
        (*path).points[(*path).length] = (Point){ (*path).points[(*path).length - 1].x, (*path).points[(*path).length - 1].y - 1 };
        (*path).length++;
        start_y--;
        find_paths(row_moves, column_moves + 1, path, current_score + DGIST_OBJ.map[start_x][start_y].item.score, best_path, start_x, start_y);
        (*path).length--;
        start_y++;
    }
}

Point* find_best_road(Point* StartPoint, Point* EndPoint, int* path_length) {
    int start_x = (*StartPoint).x;
    int start_y = (*StartPoint).y;
    int end_x = (*EndPoint).x;
    int end_y = (*EndPoint).y;
    int row_move = end_x - start_x;
    int column_move = end_y - start_y;
    Path best_path;
    Path initial_path;
    initial_path.length = 1;
    initial_path.points[0] = *StartPoint;
    initial_path.score = 0;
    //경로 찾아서 best_path에 저장
    find_paths(row_move, column_move, &initial_path, 0, &best_path, start_x, start_y);
    *path_length = best_path.length;
    //best_path의 정보를 Point*로 변환
    Point* highest = (Point*)malloc((abs(row_move) + abs(column_move) + 1) * sizeof(Point));
    for (int i = 0; i < abs(row_move) + abs(column_move) + 1;i++) {
        highest[i] = best_path.points[i];
    }
    return highest;
}

// 내 포인트와 상대 포인트, 내가 다음에 갈 포인트를 바탕으로 폭탄을 세팅할지 말지를 알 수 있다.
bool SetBomb_Checker(Point* currpoint, Point* opponentpoint) {
    bool returnbool = false;
    // 다음 포인트가 뭔지 알아야 함
    Point* nextpoint=currpoint; //초기화
    if (DGIST_OBJ.map[(*nextpoint).x][(*nextpoint).y].item.score == 4) {
        if ((*currpoint).x == (*nextpoint).x && (*nextpoint).x == (*opponentpoint).x && (*opponentpoint).y - (*currpoint).y == (*currpoint).y - (*nextpoint).y) {
            returnbool = true;
        }
        else if ((*currpoint).y == (*nextpoint).y && (*nextpoint).y == (*opponentpoint).y && (*opponentpoint).x - (*currpoint).x == (*currpoint).x - (*nextpoint).x) {
            returnbool = true;
        }
    }
    return returnbool;
}