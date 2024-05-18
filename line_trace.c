#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // POSIX 라이브러리의 write 함수 포함

#define I2C_ADDR 0x16

// 라인 트래킹 센서 핀 설정 (WiringPi 번호 체계 사용)
#define SENSOR_RIGHT1 7  // 물리적 핀 11, GPIO 17  맨 오른쪽 핀
#define SENSOR_RIGHT2 0  // 물리적 핀 7, GPIO 4    중앙 오른쪽 핀
#define SENSOR_LEFT1 2   // 물리적 핀 13, GPIO 27   맨 왼쪽 핀
#define SENSOR_LEFT2 3   // 물리적 핀 15, GPIO 22   중앙 왼쪽 핀

//global 변수 선언
int wait_before_stop = 0;

typedef struct {
    int _device;
    int _addr;
} YB_Pcb_Car;

void get_i2c_device(YB_Pcb_Car* car, int address) {
    car->_addr = address;
    car->_device = wiringPiI2CSetup(car->_addr); // 주어진 I2C 주소로 I2C 장치 초기화
    if (car->_device == -1) {
        printf("Failed to initialize I2C device\n");
        exit(1);
    }
    printf("I2C device initialized at address 0x%X\n", car->_addr);
}

void write_array(YB_Pcb_Car* car, int reg, unsigned char* data, int length) {
    unsigned char buffer[length + 1];
    buffer[0] = reg;
    for (int i = 0; i < length; ++i) {
        buffer[i + 1] = data[i];
    }
    if (write(car->_device, buffer, length + 1) != length + 1) {
        printf("write_array I2C error\n");
    } else {
        printf("write_array: Data written to reg %d\n", reg);
    }
}

void Ctrl_Car(YB_Pcb_Car* car, int l_dir, int l_speed, int r_dir, int r_speed) {
    printf("Ctrl_Car called with l_dir: %d, l_speed: %d, r_dir: %d, r_speed: %d\n", l_dir, l_speed, r_dir, r_speed);
    unsigned char data[4] = { l_dir, l_speed, r_dir, r_speed };
    write_array(car, 0x01, data, 4);
}

void Car_Run(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 1, l_speed, 1, r_speed);
}

void Car_Stop(YB_Pcb_Car* car) {
    unsigned char data[4] = {0, 0, 0, 0};
    write_array(car, 0x01, data, 4);
}

void Car_Back(YB_Pcb_Car* car, int l_speed, int r_speed) {
    Ctrl_Car(car, 0, l_speed, 0, r_speed);
}

void Car_Left(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 0, speed, 1, speed);
}

void Car_Right(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 1, speed, 0, speed);
}

void Car_Spin_Left(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 0, speed, 1, speed);
}

void Car_Spin_Right(YB_Pcb_Car* car, int speed) {
    Ctrl_Car(car, 1, speed, 0, speed);
}

void Ctrl_Servo(YB_Pcb_Car* car, int servo_id, int angle) {
    unsigned char data[2] = { servo_id, angle };
    write_array(car, 0x03, data, 2);
}

int read_sensor(int pin) {
    return digitalRead(pin);
}

void L_H_H_H (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    wait_before_stop = 0;
    // 왼쪽으로 회전
    while (left1 != HIGH || left2 != LOW) {
        Car_Spin_Left(car, 50);
        left1 = read_sensor(SENSOR_LEFT1);
        left2 = read_sensor(SENSOR_LEFT2);
        delay(10);
    }
    delay(100);
}
void H_L_H_H (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    wait_before_stop = 0;
    // 왼쪽으로 회전
    Car_Spin_Left(car, 50);
    delay(10);
}
void H_H_H_L (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 오른쪽으로 회전
    wait_before_stop = 0;
    while (right1 != HIGH || right2 != LOW) {
        Car_Spin_Right(car, 50);
        right1 = read_sensor(SENSOR_RIGHT1);
        right2 = read_sensor(SENSOR_RIGHT2);
        delay(10);
    }
    delay(100);
}
void H_H_L_H (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    wait_before_stop = 0;
    // 오른쪽으로 회전
    Car_Spin_Right(car, 50);
    delay(100);
}
void H_L_L_L (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    wait_before_stop = 0;
    // 직진 후 오른쪽으로 회전하여 라인 찾기
    Car_Run(car, 50, 50);
    delay(500); // 1000에서 500으로 줄임
    while (left2 != LOW) {
        Car_Spin_Right(car, 50);
        left2 = read_sensor(SENSOR_LEFT2);
        delay(10);
    }
    delay(100);
}
void L_L_L_H(int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    wait_before_stop = 0;
    // 직진 후 왼쪽으로 회전하여 라인 찾기
    Car_Run(car, 50, 50);
    delay(500); // 1000에서 500으로 줄임
    while (right2 != LOW) {
        Car_Spin_Left(car, 50);
        right2 = read_sensor(SENSOR_RIGHT2);
        delay(10);
    }
    delay(100);
}
void L_H_H_H_wo (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 왼쪽으로 회전
    while (left1 != HIGH || left2 != LOW) {
        Car_Spin_Left(car, 50);
        left1 = read_sensor(SENSOR_LEFT1);
        left2 = read_sensor(SENSOR_LEFT2);
        delay(5);
    }
    delay(10);
}
void H_L_H_H_wo (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 왼쪽으로 회전
    Car_Spin_Left(car, 50);
    delay(5);
}
void H_H_H_L_wo (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 오른쪽으로 회전
    while (right1 != HIGH || right2 != LOW) {
        Car_Spin_Right(car, 50);
        right1 = read_sensor(SENSOR_RIGHT1);
        right2 = read_sensor(SENSOR_RIGHT2);
        delay(5);
    }
    delay(10);
}
void H_H_L_H_wo (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 오른쪽으로 회전
    Car_Spin_Right(car, 5);
    delay(10);
}
void H_L_L_L_wo (int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 직진 후 오른쪽으로 회전하여 라인 찾기
    Car_Run(car, 10, 50);
    delay(500); // 1000에서 500으로 줄임
    while (left2 != LOW) {
        Car_Spin_Right(car, 50);
        left2 = read_sensor(SENSOR_LEFT2);
        delay(5);
    }
    delay(10);
}
void L_L_L_H_wo(int left1, int left2, int right1, int right2, YB_Pcb_Car* car) {
    // 직진 후 왼쪽으로 회전하여 라인 찾기
    Car_Run(car, 50, 50);
    delay(500); // 1000에서 500으로 줄임
    while (right2 != LOW) {
        Car_Spin_Left(car, 50);
        right2 = read_sensor(SENSOR_RIGHT2);
        delay(5);
    }
    delay(10);
}
int temp_buffer[1] = {0};
void line_following(YB_Pcb_Car* car) {
    
    int left1, left2, right1, right2;

    while (1) {
        left1 = read_sensor(SENSOR_LEFT1);
        left2 = read_sensor(SENSOR_LEFT2);
        right1 = read_sensor(SENSOR_RIGHT1);
        right2 = read_sensor(SENSOR_RIGHT2);

        if (left1 == LOW && left2 == HIGH && right2 == HIGH && right1 == HIGH) {
            L_H_H_H(left1, left2, right1, right2, car);
            temp_buffer[0] = 1;
        } else if (left1 == HIGH && left2 == LOW && right2 == HIGH && right1 == HIGH) {
            H_L_H_H(left1, left2, right1, right2, car);
            temp_buffer[0] = 2;
        } else if (left1 == HIGH && left2 == HIGH && right2 == HIGH && right1 == LOW) {
            H_H_H_L(left1, left2, right1, right2, car);
            temp_buffer[0] = 3;
        } else if (left1 == HIGH && left2 == HIGH && right2 == LOW && right1 == HIGH) {
            H_H_L_H(left1, left2, right1, right2, car);
            temp_buffer[0] = 4;
        } else if (left1 == HIGH && left2 == LOW && right2 == LOW && right1 == LOW) {
            H_L_L_L(left1, left2, right1, right2, car);
            temp_buffer[0] = 5;
        } else if (left1 == LOW && left2 == LOW && right2 == LOW && right1 == HIGH) {
            L_L_L_H(left1,left2, right1, right2, car);
            temp_buffer[0] = 6;
        } else if (left1 == HIGH && left2 == HIGH && right2 == HIGH && right1 == HIGH) {
            ++wait_before_stop;
            if (temp_buffer[0] == 1) {L_H_H_H_wo(left1, left2, right1, right2, car);}
            if (temp_buffer[0] == 2) {H_L_H_H_wo(left1, left2, right1, right2, car); }
            if (temp_buffer[0] == 3) {H_H_H_L_wo(left1, left2, right1, right2, car); }
            if (temp_buffer[0] == 4) {H_H_L_H_wo(left1, left2, right1, right2, car); }
            if (temp_buffer[0] == 5) {H_L_L_L_wo(left1, left2, right1, right2, car); }
            if (temp_buffer[0] == 6) {L_L_L_H_wo(left1, left2, right1, right2, car); }
            if (temp_buffer[0] == 8) {Car_Back(car, 25, 25); }
            // 모든 센서가 HIGH인 경우, 라인을 벗어났을 가능성이 높음. 로봇 정지
            if (wait_before_stop == 15) {
                Car_Stop(car);
                delay(400);   
            }
        } else {
            // 기본 전진
            Car_Run(car, 50, 50);
            delay(100);
            temp_buffer[0] = 8;
        }

    }
}


int main() {
    if (wiringPiSetup() == -1) {
        printf("wiringPi setup failed\n");
        return 1;
    }

    pinMode(SENSOR_LEFT1, INPUT);
    pinMode(SENSOR_LEFT2, INPUT);
    pinMode(SENSOR_RIGHT1, INPUT);
    pinMode(SENSOR_RIGHT2, INPUT);

    YB_Pcb_Car car;
    get_i2c_device(&car, I2C_ADDR);

    printf("Starting line following...\n");
    line_following(&car);

    return 0;
}