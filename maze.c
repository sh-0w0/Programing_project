#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include<conio.h>

#define WIDTH  7
#define HEIGHT 7

typedef struct {
    int x, y;
} Point;

typedef struct Node {
    Point pos;
    int g, h;
    struct Node* parent;
} Node;

void make_map(int map[HEIGHT][WIDTH], int height, int width);
void print_map(int map[HEIGHT][WIDTH], int height, int width, Point player, int count);
void dfs(int map[HEIGHT][WIDTH], int height, int width);
Point randomNeighbor(int map[HEIGHT][WIDTH], int visited[HEIGHT][WIDTH], int x, int y, int width, int height);
int isInside(int x, int y, int width, int height);
void move_player(Point* player, int map[HEIGHT][WIDTH], char direction, int* count, int* totalcount);
int end_game(Point player, int totalcount, int width, int height);
void a_star(Point start, Point goal, int map[HEIGHT][WIDTH], int width, int height, Point* path, int* path_length , int* totalcount);
int heuristic(Point a, Point b);
Node* create_node(Point pos, int g, int h, Node* parent);


int main() {
    int map[HEIGHT][WIDTH];
    int count = 0;      // 현재 맵에서의 이동 횟수
    int totalcount = 0; // 총 이동 횟수
    Point player = { 1, 1 }; // 플레이어의 초기 위치
    char command;

    srand(time(NULL)); // 랜덤 숫자 생성 초기화
    make_map(map, HEIGHT, WIDTH); // 초기 맵 생성
    dfs(map, HEIGHT, WIDTH);      // DFS를 이용해 미로 생성

    printf("1.자동모드\n2.수동모드\n입력 : ");
    scanf_s("%c", &command);
    getchar();
    if (command == 49) {
        // 자동 모드 시작: A* 알고리즘을 사용하여 자동으로 목표 지점까지 이동
        Point path[WIDTH * HEIGHT];
        int path_length = 0;
        a_star(player, (Point) { WIDTH - 2, HEIGHT - 2 }, map, WIDTH, HEIGHT, path, &path_length, &totalcount);

        // 자동 이동을 시각적으로 보여주기
        for (int i = 0; i < path_length; i++) {
            player = path[i];
            print_map(map, HEIGHT, WIDTH, player, count);
            Sleep(200); // 0.2초마다 이동
            totalcount++;
            count++;
            if (end_game(player, totalcount, WIDTH, HEIGHT))
                break;
        }
    }
    else if (command == 50) {
        while (1) {
            if (_kbhit()) {
                command = _getch(); // 사용자 입력 받기
                move_player(&player, map, command, &count, &totalcount); // 플레이어 이동 처리
                print_map(map, HEIGHT, WIDTH, player, count); // 맵 상태 출력
                end_game(player, totalcount, WIDTH, HEIGHT); // 게임 종료 조건 확인
                // 이동 횟수가 10번이면 맵을 새로 생성
                if (count == 10) {
                    count = 0;
                    make_map(map, HEIGHT, WIDTH);
                    dfs(map, HEIGHT, WIDTH);
                    print_map(map, HEIGHT, WIDTH, player, count); // 새 맵 상태 출력
                }
            }



            // 사용자가 'q'를 누르면 종료
            if (command == 'q') {
                break;
            }
        }
    }
   

    return 0;
}

void make_map(int map[HEIGHT][WIDTH], int height, int width) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if ((i % 2 == 0) || (j % 2 == 0))
                map[i][j] = 1; // 벽
            else
                map[i][j] = 0; // 길
        }
    }
    map[0][1] = 0; // 시작점
    map[height - 1][width - 2] = 0; // 끝점
    map[height - 2][width - 2] = 3; // 목표 지점 표시
}

void print_map(int map[HEIGHT][WIDTH], int height, int width, Point player, int count) {
    system("cls");
    printf("\n\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (player.y == i && player.x == j) printf("P"); // 플레이어 위치
            else if (map[i][j] == 1) printf("■"); // 벽
            else if (map[i][j] == 0) printf("□"); // 길
            else printf("F"); // 목표 지점
        }
        printf("\n");
    }
    printf("%d moves left to change map\n", 10 - count); // 맵이 변경되기까지 남은 이동 횟수
}

void dfs(int map[HEIGHT][WIDTH], int height, int width) {
    int visited[HEIGHT][WIDTH] = { 0 }; // 방문 여부를 저장하는 배열
    Point stack[WIDTH * HEIGHT]; // DFS를 위한 스택
    int top = 0;

    Point start = { 1, 1 };
    stack[top++] = start;
    visited[start.y][start.x] = 1; // 시작점을 방문으로 표시

    while (top > 0) {
        Point current = stack[top - 1];
        Point neighbor = randomNeighbor(map, visited, current.x, current.y, width, height);

        if (neighbor.x != -1 && neighbor.y != -1) {
            map[neighbor.y][neighbor.x] = 0;
            map[(current.y + neighbor.y) / 2][(current.x + neighbor.x) / 2] = 0;
            visited[neighbor.y][neighbor.x] = 1; // 이웃 점을 방문으로 표시
            stack[top++] = neighbor;
        }
        else {
            top--; // 더 이상 방문할 이웃이 없으면 스택에서 제거
        }
    }
}

Point randomNeighbor(int map[HEIGHT][WIDTH], int visited[HEIGHT][WIDTH], int x, int y, int width, int height) {
    Point neighbors[4];
    int count = 0;

    if (isInside(x, y - 2, width, height) && !visited[y - 2][x]) {
        neighbors[count++] = (Point){ x, y - 2 };
    }
    if (isInside(x, y + 2, width, height) && !visited[y + 2][x]) {
        neighbors[count++] = (Point){ x, y + 2 };
    }
    if (isInside(x - 2, y, width, height) && !visited[y][x - 2]) {
        neighbors[count++] = (Point){ x - 2, y };
    }
    if (isInside(x + 2, y, width, height) && !visited[y][x + 2]) {
        neighbors[count++] = (Point){ x + 2, y };
    }

    if (count == 0) {
        return (Point) { -1, -1 };
    }
    else {
        int index = rand() % count;
        return neighbors[index];
    }
}

int isInside(int x, int y, int width, int height) {
    return x > 0 && x < width - 1 && y > 0 && y < height - 1;
}

void move_player(Point* player, int map[HEIGHT][WIDTH], char direction, int* count, int* totalcount) {
    int new_x = player->x;
    int new_y = player->y;

    switch (direction) {
    case 'w': new_y--; break;
    case 's': new_y++; break;
    case 'a': new_x--; break;
    case 'd': new_x++; break;
    default: return;
    }

    if (isInside(new_x, new_y, WIDTH, HEIGHT) && map[new_y][new_x] == 0) {
        player->x = new_x;
        player->y = new_y;
        (*count)++;
        (*totalcount)++;
    }
}

int end_game(Point player, int totalcount, int width, int height) {
    if (player.x == width - 2 && player.y == height - 2) {
        printf("Game Finished\nTotal movements: %d\n", totalcount);

        char ask_start;
        do {
            printf("Play Again? (Y/N): ");
            scanf_s(" %c", &ask_start);
            getchar();
            if (ask_start == 'Y' || ask_start == 'y') {
                main();
            }
            else if (ask_start == 'N' || ask_start == 'n') {
                exit(1);
            }
        } while (ask_start != 'Y' && ask_start != 'y' && ask_start != 'N' && ask_start != 'n');
    }
    return 1; // 게임 계속
}

void a_star(Point start, Point goal, int map[HEIGHT][WIDTH], int width, int height, Point* path, int* path_length, int* totalcount) {
    Node* open_list[WIDTH * HEIGHT] = { 0 };
    int open_count = 0;
    Node* closed_list[WIDTH * HEIGHT] = { 0 };
    int closed_count = 0;

    Node* start_node = create_node(start, 0, heuristic(start, goal), NULL);
    open_list[open_count++] = start_node;

    while (open_count > 0) {
        // Open list에서 f 값이 가장 작은 노드 선택
        int lowest_index = 0;
        for (int i = 0; i < open_count; i++) {
            if (open_list[i]->g + open_list[i]->h < open_list[lowest_index]->g + open_list[lowest_index]->h) {
                lowest_index = i;
            }
        }

        Node* current_node = open_list[lowest_index];
        open_list[lowest_index] = open_list[open_count - 1];
        open_count--;

        // 현재 노드를 closed list에 추가
        closed_list[closed_count++] = current_node;

        // 목표 지점에 도착하면 경로 복구 후 종료
        if (current_node->pos.x == goal.x && current_node->pos.y == goal.y) {
            Node* path_node = current_node;
            *path_length = 0;
            while (path_node != NULL) {
                path[*path_length] = path_node->pos;
                path_node = path_node->parent;
                (*path_length)++;
            }
            return;
        }

        // 이웃 노드 검사
        Point neighbors[4] = {
            { current_node->pos.x, current_node->pos.y - 1 },
            { current_node->pos.x, current_node->pos.y + 1 },
            { current_node->pos.x - 1, current_node->pos.y },
            { current_node->pos.x + 1, current_node->pos.y }
        };

        for (int i = 0; i < 4; i++) {
            Point neighbor = neighbors[i];

            // 이웃이 맵 범위 내에 있고, 벽이 아니며, closed list에 없는 경우 처리
            if (isInside(neighbor.x, neighbor.y, width, height)
                && map[neighbor.y][neighbor.x] == 0
                && !node_in_list(neighbor, closed_list, closed_count)) {

                int g = current_node->g + 1;
                int h = heuristic(neighbor, goal);
                Node* neighbor_node = create_node(neighbor, g, h, current_node);

                // 이웃이 open list에 이미 있고, 더 나은 경로가 아니면 스킵
                if (node_in_list(neighbor, open_list, open_count) && g >= neighbor_node->g) {
                    free(neighbor_node);
                    continue;
                }

                // open list에 추가
                open_list[open_count++] = neighbor_node;
            }
        }

        // 매 단계마다 맵을 출력
        system("cls");
        printf("\n\n");
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (current_node->pos.y == i && current_node->pos.x == j) printf("P"); // 현재 위치 표시
                else if (map[i][j] == 1) printf("■"); // 벽
                else if (map[i][j] == 0) printf("□"); // 길
                else printf("F"); // 목표 지점
            }
            printf("\n");
        }
        printf("Searching path...\n");
        Sleep(100); // 출력 시간 간격 조정 (0.1초)

        (*totalcount)++;
    }
}


int heuristic(Point a, Point b) {
    // 맨해튼 거리 사용
    return abs(a.x - b.x) + abs(a.y - b.y);
}

Node* create_node(Point pos, int g, int h, Node* parent) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->pos = pos;
    node->g = g;
    node->h = h;
    node->parent = parent;
    return node;
}

int node_in_list(Point pos, Node* list[], int count) {
    for (int i = 0; i < count; i++) {
        if (list[i]->pos.x == pos.x && list[i]->pos.y == pos.y) {
            return 1;
        }
    }
    return 0;
}
