#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "BlackBox.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "timers.h"
#include "extint.h"
#include "HW_access.h"

// -----------------------------------------------------------------------------
// DEFINICIJE
// -----------------------------------------------------------------------------
#define COM_CH_0  0
#define COM_CH_1  1
#define COM_CH_2  2

#define TASK_SERIAL_REC_PRI   (tskIDLE_PRIORITY + 3)
#define TASK_LED_BAR_PRI      (tskIDLE_PRIORITY + 1)
#define TASK_DISPLAY_PRI      (tskIDLE_PRIORITY + 1)

// -----------------------------------------------------------------------------
// SEMAFORI
// -----------------------------------------------------------------------------
static SemaphoreHandle_t RXC_BinarySemaphore_0 = NULL;
static SemaphoreHandle_t RXC_BinarySemaphore_1 = NULL;
static SemaphoreHandle_t RXC_BinarySemaphore_2 = NULL;
static SemaphoreHandle_t TBE_BinarySemaphore;
static SemaphoreHandle_t LED_INT_BinarySemaphore;
static SemaphoreHandle_t display_mutex;

// -----------------------------------------------------------------------------
// PROMENLJIVE
// -----------------------------------------------------------------------------
static uint16_t min_distanca = 999;
static uint16_t max_distanca = 0;
static uint16_t min_max_flag = 0;
static uint8_t tempomat_flag = 0;    // 0 - iskljucen, 1 - ukljucen
static uint8_t display_digits[10] = { 0 };
static const uint8_t hexnum[] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F };

int prosek = 0;
int zadati_prag = 150;
int brzina = 50;
int zelejna_brzina = 0;  // FIX #3: inicijalizovana na 0
int zadata_brzina = 0;

// -----------------------------------------------------------------------------
// FORWARD DEKLARACIJA
// -----------------------------------------------------------------------------
static void update_display(void);

// -----------------------------------------------------------------------------
// INTERRUPT HANDLER
// -----------------------------------------------------------------------------

static uint32_t OnLED_ChangeInterrupt(void)
{
    BaseType_t xHigherPTW = pdFALSE;
    uint8_t d0, d2;

    get_LED_BAR(0, &d0);
    get_LED_BAR(2, &d2);

    static uint8_t prev_d0 = 0;
    static uint8_t prev_d2 = 0;

    if (d0 != prev_d0)
    {
        prev_d0 = d0;
        xSemaphoreGiveFromISR(LED_INT_BinarySemaphore, &xHigherPTW);
    }

    portYIELD_FROM_ISR(xHigherPTW);
    return 0;
}

static uint32_t prvProcessTBEInterrupt(void)
{
    BaseType_t xHigherPTW = pdFALSE;
    xSemaphoreGiveFromISR(TBE_BinarySemaphore, &xHigherPTW);
    portYIELD_FROM_ISR(xHigherPTW);
    return 0;
}

static uint32_t prvProcessRXCInterrupt(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (get_RXC_status(COM_CH_0))
        xSemaphoreGiveFromISR(RXC_BinarySemaphore_0, &xHigherPriorityTaskWoken);

    if (get_RXC_status(COM_CH_1))
        xSemaphoreGiveFromISR(RXC_BinarySemaphore_1, &xHigherPriorityTaskWoken);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    return 0;
}

// -----------------------------------------------------------------------------
// FUNKCIJE
// -----------------------------------------------------------------------------

void send_message_COM1(const char* msg)
{
    for (size_t i = 0; i < strlen(msg); i++) {
        send_serial_character(COM_CH_1, msg[i]);
        xSemaphoreTake(TBE_BinarySemaphore, portMAX_DELAY);
    }
}

void Obrada_vrednosti_senzora(int broj)
{
    static int buffer[10] = { 0 };
    static int idx = 0;
    static int count = 0;
    int suma = 0;

    buffer[idx] = broj;
    idx = (idx + 1) % 10;
    if (count < 10) count++;

    for (int i = 0; i < count; i++)
        suma += buffer[i];

    prosek = suma / count;

    // FIX #5: azuriranje min i max distance
    if (broj < min_distanca) min_distanca = (uint16_t)broj;
    if (broj > max_distanca) max_distanca = (uint16_t)broj;

    printf("Primljen podatak: %d cm | Prosek poslednjih %d vrednosti: %d cm\n", broj, count, prosek);
    printf("Min: %d cm | Max: %d cm\n", min_distanca, max_distanca);
}

void Obrada_Kanal1(uint8_t cc)
{
    static char buffer[64];
    static int idx = 0;

    if (cc == '\n')
    {
        buffer[idx] = '\0';

        // ---------------- TEMPOMAT_OFF ----------------
        if (strcmp(buffer, "TEMPOMAT_OFF") == 0)
        {
            tempomat_flag = 0;
            update_display();
            printf("Tempomat OFF\n");
        }

        // ---------------- TEMPOMAT_xxx ----------------
        else if (strncmp(buffer, "TEMPOMAT_", 9) == 0)
        {
            int speed = 0;

            for (int i = 9; buffer[i] != '\0'; i++)
            {
                if (buffer[i] >= '0' && buffer[i] <= '9')
                {
                    speed = speed * 10 + (buffer[i] - '0');
                }
                else
                {
                    speed = -1;
                    break;
                }
            }

            if (speed >= 1 && speed <= 350)
            {
                tempomat_flag = 1;
                zelejna_brzina = speed;  // FIX #3: postavljamo zelenjenu brzinu
                zadata_brzina = speed;
                update_display();
                printf("Tempomat ON | Brzina: %d\n", speed);
            }
            else
            {
                printf("GRESKA: neispravna brzina\n");
            }
        }

        // ---------------- PRAG_xxx ----------------
        else if (strncmp(buffer, "PRAG_", 5) == 0)
        {
            zadati_prag = 0;

            for (int i = 5; buffer[i] != '\0'; i++)
            {
                if (buffer[i] >= '0' && buffer[i] <= '9')
                {
                    zadati_prag = zadati_prag * 10 + (buffer[i] - '0');
                }
                else
                {
                    zadati_prag = -1;
                    break;
                }
            }

            if (zadati_prag >= 1 && zadati_prag <= 999)
            {
                printf("Novi prag: %d\n", zadati_prag);
            }
            else
            {
                printf("GRESKA: neispravan prag\n");
            }
        }

        // ---------------- NEPOZNATA KOMANDA ----------------
        else
        {
            printf("Nepoznata komanda: %s\n", buffer);
        }

        idx = 0;
        memset(buffer, 0, sizeof(buffer));
    }
    else
    {
        if (idx < (int)(sizeof(buffer) - 1))
        {
            buffer[idx++] = (char)cc;
        }
        else
        {
            printf("GRESKA: overflow bafera\n");
            idx = 0;
            memset(buffer, 0, sizeof(buffer));
        }
    }
}

void srednja_udaljenost_stanje_tempomata(void)
{
    if (tempomat_flag == 1) {
        send_message_COM1("TEMPOMAT UKLJUCEN\n");
    }
    else {
        send_message_COM1("TEMPOMAT ISKLJUCEN\n");
    }

    if (prosek > zadati_prag) {
        send_message_COM1("Distanca veca\n");
    }

    update_display();
}

// -----------------------------------------------------------------------------
// TASKOVI
// -----------------------------------------------------------------------------

void Prijem_podataka_sa_senzora(void* pvParameters)
{
    uint8_t cc;
    int broj = 0;
    int idx = 0;

    while (1)
    {
        xSemaphoreTake(RXC_BinarySemaphore_0, pdMS_TO_TICKS(200));
        get_serial_character(COM_CH_0, &cc);

        if (cc == '\n')
        {
            if (idx > 0)
            {
                if (broj >= 1 && broj <= 999)
                {
                    Obrada_vrednosti_senzora(broj);
                    update_display();  // osvezi displej nakon novog ocitavanja
                }
                else
                {
                    printf("GRESKA: broj mora biti 1-999\n");
                }
            }
            broj = 0;
            idx = 0;
        }
        else
        {
            if (cc >= '0' && cc <= '9')
            {
                broj = broj * 10 + (cc - '0');
                idx++;
            }
            else
            {
                printf("GRESKA: neispravan unos\n");
                broj = 0;
                idx = 0;
            }
        }
    }
}

void Slanje_podataka_kanal1(void* pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    while (1) {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(2000));
        srednja_udaljenost_stanje_tempomata();
    }
}

void Prijem_podataka_kanal1(void* pvParameters)
{
    uint8_t cc;
    while (1) {
        xSemaphoreTake(RXC_BinarySemaphore_1, portMAX_DELAY);
        get_serial_character(COM_CH_1, &cc);
        Obrada_Kanal1(cc);
    }
}

void LED_Bar_Read1(void* pvParameters)
{
    uint8_t d1;

    while (1) {
        xSemaphoreTake(LED_INT_BinarySemaphore, portMAX_DELAY);
        get_LED_BAR(0, &d1);
        // FIX: tempomat na osnovu LED bara
        if (d1 >= 1 && d1 <= 7 || d1 >= 64 && d1 <= 127 || d1 >= 192 && d1 <= 255) {
            tempomat_flag = 0;
        }
        else {
            tempomat_flag = 1;
        }
        // FIX #6: osvezi displej nakon promene tempomat_flag
        // FIX: min/max prikaz na osnovu bita 7 (d1 >= 128)
        if (d1 >= 128) {
            min_max_flag = 1;  // prikazi max_distancu
        }
        else {
            min_max_flag = 0;  // prikazi min_distancu
        }

        update_display();
    }
}

void LED_Bar_Write(void* pvParameters)
{
    uint8_t pom = 0;

    while (1) {
        if (prosek < zadati_prag) {
            if (pom == 0) {
                set_LED_BAR(1, 0xFF);
                pom = 1;
            }
            else {
                set_LED_BAR(1, 0x00);
                pom = 0;
            }
        }
        else {
            set_LED_BAR(1, 0x00);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// FIX #2: desired promenjen u uint16_t (bio uint8_t, max 255, overflow na 999)
static void update_display(void)
{
    uint16_t desired = tempomat_flag ? (uint16_t)zelejna_brzina : 999;  // FIX #2
    uint16_t current = (uint16_t)brzina;
    uint16_t min_distance = min_distanca;
    uint16_t max_distance = max_distanca;

    if (desired > 999) desired = 999;
    if (current > 999) current = 999;

    if (xSemaphoreTake(display_mutex, pdMS_TO_TICKS(50)) == pdTRUE)
    {
        // Zeljena brzina na prva 3 mesta (0,1,2)
        display_digits[0] = (uint8_t)(desired / 100);
        display_digits[1] = (uint8_t)((desired / 10) % 10);
        display_digits[2] = (uint8_t)(desired % 10);

        // Cetvrto mesto prazno
        display_digits[3] = 255;

        // Trenutna brzina na mestima 4,5,6
        display_digits[4] = (uint8_t)(current / 100);
        display_digits[5] = (uint8_t)((current / 10) % 10);
        display_digits[6] = (uint8_t)(current % 10);

        // Min ili max distanca na mestima 7,8,9
        if (min_max_flag == 1) {
            display_digits[7] = (uint8_t)(max_distance / 100);
            display_digits[8] = (uint8_t)((max_distance / 10) % 10);
            display_digits[9] = (uint8_t)(max_distance % 10);
        }
        else {
            display_digits[7] = (uint8_t)(min_distance / 100);
            display_digits[8] = (uint8_t)((min_distance / 10) % 10);
            display_digits[9] = (uint8_t)(min_distance % 10);
        }

        xSemaphoreGive(display_mutex);
    }
}

void Display_Task(void* pvParameters)
{
    while (1) {
        for (int digit = 0; digit < 10; digit++) {
            select_7seg_digit(digit);
            if (xSemaphoreTake(display_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                if (display_digits[digit] == 255) {
                    set_7seg_digit(0x00);
                }
                else {
                    set_7seg_digit(hexnum[display_digits[digit]]);
                }
                xSemaphoreGive(display_mutex);
            }
            else {
                set_7seg_digit(0x00);
            }
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

void Automatski_Rezim(void* pvParameters)
{

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(3000));  // svake 3s

        if (tempomat_flag == 0)
        {
            brzina--;
            // Automatski rezim - regulacija prema pragu
            if (prosek < zadati_prag)
            {
                // Srednja udaljenost manja od praga - smanjuj brzinu
                if (brzina > 1)
                {
                    
                    printf("AUTO: Smanjujem brzinu -> %d km/h (prosek %d cm < prag %d cm)\n",
                        brzina, prosek, zadati_prag);
                }
            }
            else
            {
                brzina++;
                // Srednja udaljenost veca od praga - vracaj brzinu na 50
                if (brzina < 50)
                {
                    printf("AUTO: Povecavam brzinu -> %d km/h (prosek %d cm >= prag %d cm)\n",
                        brzina, prosek, zadati_prag);
                }
            }
        }
        else  // tempomat_flag == 1
        {
            // Tempomat ukljucen - priblizavaj trenutnu brzinu zelenjenoj
            if (brzina < zelejna_brzina)
            {
                brzina++;
                printf("TEMPOMAT: Povecavam brzinu -> %d km/h (zeljena: %d km/h)\n",
                    brzina, zelejna_brzina);
            }
            else if (brzina > zelejna_brzina)
            {
                brzina--;
                printf("TEMPOMAT: Smanjujem brzinu -> %d km/h (zeljena: %d km/h)\n",
                    brzina, zelejna_brzina);
            }
            else {
                printf("TEMPOMAT: Brzine su jednake -> %d km/h (zeljena: %d km/h)\n",
                    brzina, zelejna_brzina);
            }
            // ako su jednake - ne radi nista
        }

        update_display();
    }
}

// -----------------------------------------------------------------------------
// MAIN DEMO
// -----------------------------------------------------------------------------
void main_demo(void)
{
    // ---------------- INIT HARDWARE ----------------
    init_serial_uplink(COM_CH_0);
    init_serial_downlink(COM_CH_0);
    init_serial_uplink(COM_CH_1);
    init_serial_downlink(COM_CH_1);
    init_7seg_comm();
    init_LED_comm();

    // ---------------- ISR HOOK ----------------
    vPortSetInterruptHandler(portINTERRUPT_SRL_RXC, prvProcessRXCInterrupt);
    vPortSetInterruptHandler(portINTERRUPT_SRL_TBE, prvProcessTBEInterrupt);
    vPortSetInterruptHandler(portINTERRUPT_SRL_OIC, OnLED_ChangeInterrupt);

    // ---------------- SEMAPHORES ----------------
    RXC_BinarySemaphore_0 = xSemaphoreCreateBinary();
    RXC_BinarySemaphore_1 = xSemaphoreCreateBinary();
    RXC_BinarySemaphore_2 = xSemaphoreCreateBinary();
    TBE_BinarySemaphore = xSemaphoreCreateBinary();
    LED_INT_BinarySemaphore = xSemaphoreCreateBinary();
    display_mutex = xSemaphoreCreateMutex();  // FIX #1: inicijalizacija mutexa

    if (RXC_BinarySemaphore_0 == NULL ||
        RXC_BinarySemaphore_1 == NULL ||
        RXC_BinarySemaphore_2 == NULL ||
        TBE_BinarySemaphore == NULL ||
        LED_INT_BinarySemaphore == NULL ||
        display_mutex == NULL)
    {
        while (1);  // FIX: provera svih semafora ukljucujuci i mutex
    }

    // ---------------- TASKS ----------------
    xTaskCreate(Prijem_podataka_sa_senzora, "Senzor_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_SERIAL_REC_PRI, NULL);
    xTaskCreate(Slanje_podataka_kanal1, "TX1_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_SERIAL_REC_PRI, NULL);
    xTaskCreate(Prijem_podataka_kanal1, "RX1_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_SERIAL_REC_PRI, NULL);
    xTaskCreate(Display_Task, "DISP_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_DISPLAY_PRI, NULL);
    xTaskCreate(LED_Bar_Read1, "LED_R1_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_LED_BAR_PRI, NULL);
    xTaskCreate(LED_Bar_Write, "LED_W_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_LED_BAR_PRI, NULL);
    xTaskCreate(Automatski_Rezim, "AUTO_TASK", configMINIMAL_STACK_SIZE + 200, NULL, TASK_SERIAL_REC_PRI, NULL);

    // ---------------- START SCHEDULER ----------------
    vTaskStartScheduler();
    while (1);
}