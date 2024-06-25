#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>


#define WIDTH  25
#define HEIGHT 25

// 구조체 정의: 미로에서의 점을 나타냅니다.
typedef struct {
    int x, y;
} Point;

// 함수 선언
void make_map(int map[HEIGHT][WIDTH], int height, int width);
void print_map(int map[HEIGHT][WIDTH], int height, int width, Point player, int count);
void dfs(int map[HEIGHT][WIDTH], int height, int width);
Point randomNeighbor(int map[HEIGHT][WIDTH], int visited[HEIGHT][WIDTH], int x, int y, int width, int height);
int isInside(int x, int y, int width, int height);
void move_player(Point* player, int map[HEIGHT][WIDTH], char direction, int* count, int* totalcount);
void end_game(Point player, int totalcount, int width, int height);

int map[HEIGHT][WIDTH];
int running = 1;
int main() {
    if (running) {
        int totalcount = 0; // 총 이동 횟수
        int count = 0;      // 현재 맵에서의 이동 횟수
        Point player = { 1, 1 }; // 플레이어의 초기 위치
        char command = NULL;

        srand(time(NULL)); // 랜덤 숫자 생성 초기화
        make_map(map, HEIGHT, WIDTH); // 초기 맵 생성
        dfs(map, HEIGHT, WIDTH);      // DFS를 이용해 미로 생성
        print_map(map, HEIGHT, WIDTH, player, count); // 초기 맵 출력

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

// 미로 맵을 생성하는 함수
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

// 맵을 출력하는 함수
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

// DFS를 이용해 미로를 생성하는 함수
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

// 랜덤한 이웃 점을 반환하는 함수
Point randomNeighbor(int map[HEIGHT][WIDTH], int visited[HEIGHT][WIDTH], int x, int y, int width, int height) {
    Point neighbors[4];
    int count = 0;

    // 상하좌우 이웃 점을 확인
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

    // 이웃이 없으면 (-1, -1) 반환
    if (count == 0) {
        return (Point) { -1, -1 };
    }
    else {
        int index = rand() % count; // 랜덤한 이웃 점 선택
        return neighbors[index];
    }
}

// 주어진 점이 맵 안에 있는지 확인하는 함수
int isInside(int x, int y, int width, int height) {
    return x > 0 && x < width - 1 && y > 0 && y < height - 1;
}

// 플레이어를 이동시키는 함수
void move_player(Point* player, int map[HEIGHT][WIDTH], char direction, int* count, int* totalcount) {
    int new_x = player->x;
    int new_y = player->y;

    // 사용자의 입력에 따라 이동할 위치 결정
    switch (direction) {
    case 'w': new_y--; break;
    case 's': new_y++; break;
    case 'a': new_x--; break;
    case 'd': new_x++; break;
    default: return; // 다른 입력은 무시
    }

    // 이동할 위치가 맵 안에 있고 길인 경우에만 이동
    if (isInside(new_x, new_y, WIDTH, HEIGHT) && map[new_y][new_x] == 0) {
        player->x = new_x;
        player->y = new_y;
        (*count)++;
        (*totalcount)++;
    }
}

// 게임 종료 조건을 확인하는 함수
void end_game(Point player, int totalcount, int width, int height) {
    if (player.x == width - 2 && player.y == height - 2) {

        // 플레이어에게 다시 플레이할 것인지 물어봄
        printf("total move: %d\n", totalcount);
        printf("Play Again?(Y/N): ");
        char ask_start = getchar();
        if (ask_start == 'Y' || ask_start == 'y') {
            // main 함수를 다시 호출하여 게임을 재시작
            main();
        }
        else if (ask_start == 'N' || ask_start == 'n') {
            exit(1);
        }

    }
}
