#include "all_header.h"

int main() {

    int my_x = 4; int my_y = 4;
    int opp_x = 2; int opp_y = 3;
    int count = 0;
    Point* reachable_points = Bangaljook(opp_x, opp_y, my_x, my_y, &count);  
    max_score_point = Find_MaxScorePoint(&(Point) { my_x, my_y }, reachable_points, count);  
    Point* local_optimal_path = find_best_road(my_point, max_score_point, &path_length);
    int* Directions = getDirection(local_optimal_path, path_length);
    int* Dirs_for_Movs = getDirection_for_Mov(Directions, path_length, RECENT_HEAD_DIRECTION);
    pMovements =  getMovement(Dirs_for_Movs, path_length);
    RECENT_HEAD_DIRECTION = Directions[path_length - 2];
    met_Node = 0;

    free(reachable_points);
    free(Directions);
    free(Dirs_for_Movs);

    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);
    Car_Stop(&car);
    delay(10000);
    line_following(&car);


    return 0;
}
