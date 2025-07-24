#include <string.h>
#include <stdio.h>
#include "board.h"
#include "gpio.h"
#include "delay.h"
#include "timer.h"
#include "radio.h"

#include "pico/stdlib.h"
// #include "tusb.h"
#include "sx126x.h"
#include "sx126x-board.h"



// Inicio de configurações de compilação para uso do LoRA

#if defined( REGION_AS923 )

#define RF_FREQUENCY                                923000000 // Hz

#elif defined( REGION_AU915 )

#define RF_FREQUENCY                                915000000 // Hz

#elif defined( REGION_CN470 )

#define RF_FREQUENCY                                470000000 // Hz

#elif defined( REGION_CN779 )

#define RF_FREQUENCY                                779000000 // Hz

#elif defined( REGION_EU433 )

#define RF_FREQUENCY                                433000000 // Hz

#elif defined( REGION_EU868 )

#define RF_FREQUENCY                                868000000 // Hz

#elif defined( REGION_KR920 )

#define RF_FREQUENCY                                920000000 // Hz

#elif defined( REGION_IN865 )

#define RF_FREQUENCY                                865000000 // Hz

#elif defined( REGION_US915 )

#define RF_FREQUENCY                                915000000 // Hz

#elif defined( REGION_RU864 )

#define RF_FREQUENCY                                864000000 // Hz

#else
#error "Please define a frequency band in the compiler options."
#endif

#define TX_OUTPUT_POWER                             22        // dBm

#if defined( USE_MODEM_LORA )

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
//  1: 250 kHz,
//  2: 500 kHz,
//  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
//  2: 4/6,
//  3: 4/7,
//  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         5         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false

#elif defined( USE_MODEM_FSK )

#define FSK_FDEV                                    25000     // Hz
#define FSK_DATARATE                                50000     // bps
#define FSK_BANDWIDTH                               50000     // Hz
#define FSK_AFC_BANDWIDTH                           83333     // Hz
#define FSK_PREAMBLE_LENGTH                         5         // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON                   false

#else
#error "Please define a modem in the compiler options."
#endif

// Fim das configurações de compilação

const uint LED_PIN = 25; //default led pin

// estados do receptor
typedef enum{
    RX,
    TX
} TrasmitterStates;



#define RX_TIMEOUT_VALUE 2000
#define BUFFER_SIZE 4

//mensagem de ACK

const char ackMessage[] = "ACK";

//buffer de recebimento
char Buffer[BUFFER_SIZE];
//estado atual
TrasmitterStates state = RX;
static RadioEvents_t RadioEvents;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

int main(void){

    // incialização dos ios
    stdio_init_all();

    // Target board initialization
    BoardInitMcu( );
    BoardInitPeriph( );

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);


    // Radio initialization
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    Radio.Init( &RadioEvents );

    Radio.SetChannel( 865000000 );

    #if defined( USE_MODEM_LORA )

        Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                    true, 0, 0, LORA_IQ_INVERSION_ON, 3000 );

        Radio.SetRxConfig( MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true );

        Radio.SetMaxPayloadLength( MODEM_LORA, BUFFER_SIZE );
    #endif

    printf("comecando\n");
    while(1){
        
        switch (state)
        {
    
        case RX:
        //recebendo
        printf("Estou no RX\n");
            SX126xAntSwOn();

            Radio.Rx( RX_TIMEOUT_VALUE );
            sleep_ms(10);
            break;
        case TX:
        // transmitindo
        printf("Estou no TX\n");
            SX126xAntSwOff();

            memcpy(Buffer, ackMessage, BUFFER_SIZE);

            Radio.Send(Buffer, BUFFER_SIZE);
            sleep_ms(10);
            break;
        default:
            break;
        }
        if( Radio.IrqProcess != NULL )
       {
           Radio.IrqProcess( );
       }
    }
}


void OnTxDone( void ){
    //blinks to indicate success
    for(int i=0; i<2; i++){
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }
    state = RX;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){
    memcpy(Buffer, payload, BUFFER_SIZE);
    for(int i=0; i<6; i++){
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
    state = TX;
}

void OnTxTimeout(void){
    printf("TxTimeout");
}

void OnRxTimeout(void){
    printf("RxTimeout");
    gpio_put(LED_PIN, 1);
    sleep_ms(2000);
    gpio_put(LED_PIN, 0);
    sleep_ms(2000);
}

void OnRxError(void){
    printf("RxError");
}