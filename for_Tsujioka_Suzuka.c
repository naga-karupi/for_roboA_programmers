/* USER CODE BEGIN Includes */
#include<stdint.h>
#include<stdbool.h>
#include<stdlib.h>
/* USER CODE END Includes */

//省略

/* USER CODE BEGIN PV */
typedef struct Axis{
	uint8_t x,
			y,
			omega;
} axis;

typedef struct gpio_setter
{
    GPIO_TypeDef* GPIO_PortA;
    uint16_t PinA;
    GPIO_TypeDef* GPIO_PortB;
    uint16_t PinB;
    uint32_t pwm_channel;
}GPIO_setter;

GPIO_setter motors[4];

/* USER CODE END PV */

//省略

/* USER CODE BEGIN PFP */
    void motor_stop(GPIO_setter*);
    void motor_forward(int, GPIO_setter*);
    void motor_back(int, GPIO_setter*);

    void motor(axis*);
/* USER CODE END PFP */

//省略

/* USER CODE BEGIN 0 */
bool uart6_receive_flag = false;

void (*motor_function[3])(int, GPIO_setter*) = {motor_forward, motor_back};

void setup();

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart6){
	uart6_receive_flag = 1;
}
/* USER CODE END 0 */

//省略

int main(){
//省略

//Ln137
    setup();

    uint8_t*rx_data;

    while(1){
        HAL_UART_Receive_IT(&huart6, (uint8_t *)&rx_data, 0xf);

	    while(!uart6_receive_flag);

        axis*axis_rx = (axis*)rx_data;
        motor(axis_rx);
        uart6_receive_flag = false;
    }
}

void setup(){
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    motors[0].GPIO_PortA  = IN_A1_GPIO_Port;
    motors[0].PinA        = IN_A1_Pin;
    motors[0].GPIO_PortB  = IN_B1_GPIO_Port;
    motors[0].PinB        = IN_B1_Pin;
    motors[0].pwm_channel = TIM_CHANNEL_4;
    motors[1].GPIO_PortA  = IN_A2_GPIO_Port;
    motors[1].PinA        = IN_A2_Pin;
    motors[1].GPIO_PortB  = IN_B2_GPIO_Port;
    motors[1].PinB        = IN_B2_Pin;
    motors[1].pwm_channel = TIM_CHANNEL_2;
    motors[2].GPIO_PortA  = IN_A3_GPIO_Port;
    motors[2].PinA        = IN_A3_Pin;
    motors[2].GPIO_PortB  = IN_B3_GPIO_Port;
    motors[2].PinB        = IN_B3_Pin;
    motors[2].pwm_channel = TIM_CHANNEL_1;
    motors[3].GPIO_PortA  = IN_A4_GPIO_Port;
    motors[3].PinA        = IN_A4_Pin;
    motors[3].GPIO_PortB  = IN_B4_GPIO_Port;
    motors[3].PinB        = IN_B4_Pin;
    motors[3].pwm_channel = TIM_CHANNEL_3;
}

void motor_stop(GPIO_setter* setter){
    HAL_GPIO_WritePin(setter->GPIO_PortA, setter->PinA, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(setter->GPIO_PortB, setter->PinB, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim1, setter->pwm_channel, 0);
}

void motor_forward(int i, GPIO_setter*setter){
    HAL_GPIO_WritePin(setter->GPIO_PortA, setter->PinA, GPIO_PIN_SET);
    HAL_GPIO_WritePin(setter->GPIO_PortB, setter->PinB, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim1, setter->pwm_channel, i*350);
}

void motor_back(int i, GPIO_setter*setter){
    HAL_GPIO_WritePin(setter->GPIO_PortA, setter->PinA, GPIO_PIN_SET);
    HAL_GPIO_WritePin(setter->GPIO_PortB, setter->PinB, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(&htim1, setter->pwm_channel, i*350);
}

void motor(axis* axis_motor){
    int motor_counters[4] = {0};
    motor_counters[0] =  axis_motor->x - axis_motor->y - axis_motor->omega;
    motor_counters[1] = -axis_motor->x - axis_motor->y - axis_motor->omega;
    motor_counters[2] = -axis_motor->x + axis_motor->y - axis_motor->omega;
    motor_counters[3] =  axis_motor->x + axis_motor->y - axis_motor->omega;

    for(int i = 0; i < 4; i++){
        int motor_counters_abs = abs(motor_counters[i]);

        if(motor_counters[i] == 0) motor_stop(&(motors[i]));
        else motor_counters[i]>0?motor_forward(motor_counters_abs, &(motors[i])):motor_back(motor_counters_abs, &(motors[i]));
    }
}
