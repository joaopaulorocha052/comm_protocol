#include <string.h>
#include <stdio.h>
#include "board.h"
#include "gpio.h"
#include "delay.h"
#include "timer.h"
#include "radio.h"
#include "hardware/adc.h"
#include "hardware/gpio.h"

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
const uint ADC_PIN = 26; // pin para leitura do ADC

// estados que serão utilizados pelo protocolo

typedef enum{
    SLEEP,
    TX,
    RX,
    WAITING_FOR_TRANSMISSION
} TrasmitterStates;

#define RX_TIMEOUT_VALUE 2000
#define BUFFER_SIZE 4

//Mensagem de ACK para verificação

const unsigned char ackMessage[] = "ACK";

// Buffer de Recebimento
unsigned char Buffer[BUFFER_SIZE];


// Estado atual
TrasmitterStates state = SLEEP;

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

float average_voltage_sampling( void );

int main(void){

    // configurações de ios
    stdio_init_all();
    adc_init();

    // Target board initialization
    BoardInitMcu( );
    BoardInitPeriph( );

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //inicialização do pino ADC
    adc_gpio_init(ADC_PIN);

    adc_select_input(0);

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

    // Sequencia de indicação de inicialização
    for(int i=0; i<4; i++){
        gpio_put(LED_PIN, 1);
        sleep_ms(500);
        gpio_put(LED_PIN, 0);
        sleep_ms(500);
    }
    printf("Comecando!!\n");

    
    while(1){
        switch (state)
        {
        case WAITING_FOR_TRANSMISSION:
            break;

        case SLEEP:
            // Espera interrupção (por enquanto, le a tensão)

            printf("\nDormindo!\n");


            // realiza a leitura da tensão (média)
            float reading = average_voltage_sampling();
            printf("Tensão: %f V", reading);

            if(reading < 0.5f){
                printf("\nmenor que 0.5\n");
                gpio_put(25, 1);
                state = TX;
            }else{
                printf("\nmaior que 0.5\n");
                gpio_put(25, 0);
                state = SLEEP;
            }
            sleep_ms(100);
            // state = TX;
            
            break;
        case TX:
        //envio
            Buffer[0] = 5;
            SX126xAntSwOff();

            Radio.Send(Buffer, BUFFER_SIZE);
            state = WAITING_FOR_TRANSMISSION;
            break;
            
        case RX:
            //Pisca para indicar sucesso de trasnmissão

            for(int i=0; i<3; i++){
                gpio_put(LED_PIN, 1);
                sleep_ms(250);
                gpio_put(LED_PIN, 0);
                sleep_ms(250);
            }
            //wait to receive ack

            SX126xAntSwOn();
            Radio.Rx(RX_TIMEOUT_VALUE);

            if(strcmp((char*) Buffer, ackMessage) == 0){
                state = SLEEP;
            }
            else{
                state = SLEEP;
            }
            state = WAITING_FOR_TRANSMISSION;
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
    
    state = RX;
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr ){
    printf("RxDone\n");

    memcpy(Buffer, payload, BUFFER_SIZE);

    state = SLEEP;
}

void OnTxTimeout(void){
    printf("TxTimeout\n");
}

void OnRxTimeout(void){
    printf("RxTimeout\n");
    state = RX;
}

void OnRxError(void){
    printf("RxError\n");
}

float average_voltage_sampling( void ){
    // calcula a média de 10 amostras de tensão
    float average = .0f;
    for(int i=0; i<10; i++){
        average += adc_read() * (3.3f / (1 << 12));
        sleep_ms(100);
    }

    return average / 10;
}