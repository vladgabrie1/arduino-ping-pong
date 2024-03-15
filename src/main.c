#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define __DELAY_BACKWARD_COMPATIBLE__  
#include <util/delay.h>
#include <usart.h>
#include <display.h>
#include <potentiometer.h>
#include <avr/io.h>
#include <button.h>
#include <avr/interrupt.h>
#include <buzzer.h>
#include <led_utils.h>



#define G5 783.990
#define A5 880.00
#define B5 987.770
#define C6 1046.500
#define BOARD_SIZE 40


uint16_t speed = 0;
bool playing = false;
int ballPosition = 25;
int direction = 1;
int* score;
char* board;




// This function initializes the timer 
void initTimer0()
{
    // We choose FAST PWM; TCNT0 counts to 255
    TCCR0A |= _BV(WGM00) | _BV(WGM01); // WGM00 = 1 and WGM01 = 1 --> Fast PWM Mode

    // The counting speed is determined by the CPU clock (16 Mhz) divided by this factor
    TCCR0B |= _BV(CS01) | _BV(CS00); // CS00 = 1 --> prescaler, factor is now 8

    // Enable interrupts for overflow: TCNT0 == TOP
    TIMSK0 |= _BV(TOIE0); // enable overflow interrupt
}

// This function enables the buttons for the players and interupt
void initButtons()
{
    enableButton(0);
    enableButton(1);

    PCICR |= _BV(PCIE1);
    PCMSK1 |= _BV(PCIE1);

    PCICR |= _BV(PCIE2);
    PCMSK1 |= _BV(PCIE2);

}


// This function allocates memory for the array that contains the score
void initScore(){

    score = calloc(2, sizeof(int));

    if (score == NULL) {
        printf("Memory allocation failed!");
        exit(1); 
    }

    for(int i=0; i < 2; i++) {
        score[i] = 0;
    }
}

// This function initializes the array that represents the board
void initBoard()
{
    board = malloc((BOARD_SIZE + 3) * sizeof(char));

    if (board == NULL)
    {
        printf("Memory allocation failed!");
        exit(1);
    }

    memset(board, '-', BOARD_SIZE + 1);
    board[0] = '[';
    board[BOARD_SIZE + 1] = ']';
    board[BOARD_SIZE + 2] = '\0';
}


// This function plays the song =s for the beginning of the game, it also makes the led countdown
void playStartSong(){
    float freq[] = {G5, A5, B5};
    int durations[] = {300, 300, 700};
    for(int i = 0; i < 3; i++){
        lightDownLed(i);
        enableBuzzer();
        enableAllLeds();
        playTone(freq[i], durations[i]);
        _delay_ms(600);
    }
    lightDownLed(3);
}


// This function makes the buzzer sing a song when the game is finished
void playEndSong(){
    float notes[] = { G5, A5, B5, C6};
    int durations[] = {300, 300, 300,500};

    for(int i=0; i<sizeof(notes)/sizeof(notes[0]); i++){
        enableBuzzer();
        playTone(notes[i], durations[i]);
    }
}


// This function is called when the games has ended 
void showWinner()
{
    //The end song is played
    playEndSong();
    
    //The winner is determined based on the direction of the ball
    int player = (direction == 1) ? 1 : 2;
    int playerScore = (direction == 1) ? *score : *(score + 1);
    
    printf("\n \n Player %d won with %d points", player, playerScore);
    
    //On the 7 segment display, the winner is shown along with the score ex: P105 ( player 1 with 5 points)
    blankSegment(3);
    for (int i = 0; i < 1000; i++)
    {
        writeCharToSegment(0, 'P');
        _delay_ms(5);
        writeNumberToSegment(1, player);
        _delay_ms(5);
        writeNumberToSegment(2, playerScore / 10);
        _delay_ms(5);
        writeNumberToSegment(3, playerScore % 10);
        _delay_ms(5);
    }
}

//This functions prints the board, and update the place of the ball on the board 
void printBoard()
{
    board[ballPosition] = 'O';
    printf("\r %s", board);
    // If the direction is 1 the ball goes from left to right
    board[ballPosition] = '-';
    ballPosition += direction;
}

// This ISR runs every time TCNT0 equals the TOP value (255) 
ISR(TIMER0_OVF_vect)
{
    //The speed of the ball is shown during the whole game, and can be changed based on the value of the potentiometer
    speed = ((getADC() % 10) * 100)/2;
    writeNumber(speed);
    blankSegment(3);
    
}

// This ISR runs every time a button is pressed
ISR(PCINT1_vect)
{
    //If the games has started, it starts the game and roll the ball to the opponent of the player that started the game
    if (!playing)
    {
        playStartSong();
        printf("\n\n\n");
        if (buttonPushed(0)){
            direction = 1;
        }
        while (buttonPushed(0)){}
        if (buttonPushed(1)){
            direction = -1;
        }
        while (buttonPushed(0)){}
        playing = true;
    }
    //If the game has started, it checks what button has been pressed and if the ball is near the edge
    // If so, the ball starts rolling in the opposite direction ( towards the oponent )
    else if ( buttonPushed(0) && ballPosition < 5 )
    {
        direction = 1;
        score[0] += 1;
    }
    else if ( buttonPushed(1) && ballPosition > BOARD_SIZE - 4 )
    {
        direction = -1;
        *(score + 1) += 1;
    }
}



int main()
{
    initADC();
    initUSART();
    initDisplay();
    blankSegment(3);
    initTimer0();
    initButtons();
    initScore();
    initBoard();
    sei();


    printf("\nChoose the speed of the ball and press 1st or 2nd button");


    bool end = false;
    while (1)
    {

        // If the game is being played this loop is activated
        while (playing)
        {    
            // It delays the printing ( the speed of the ball ) based on the value gotten from the potentiometer   
            _delay_ms(speed);

            //The board is printed ( the position of the ball is updated inside this function)
            printBoard();

            //The position of the ball is checked, if it touches the edges, the game ends
            if(ballPosition == BOARD_SIZE+2 || ballPosition == -1){
                cli();
                //The winner is shown
                showWinner();
                playing = false;
                end = true;
            }
        }


        if(end){
            //The memory is freed
            free(board);
            free(score);
            break;  
        }
        
    }

    return 0;
}
