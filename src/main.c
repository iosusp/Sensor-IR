#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <pwm_z42.h>

// Mapeamento da Porta E (onde estão os sensores)
#define PORTA_E_NODE DT_NODELABEL(gpioe)
#define PERSISTENCIA_CURVA 2

#define TPM_MODULE 1000 
uint16_t v_cruzeiro_esq = 1000; // velocidade roda dir
uint16_t v_cruzeiro_dir = 960; // Velocidade roda esq
uint16_t v_curva    = 1000; // Velocidade para corrigir a rota

uint16_t memoria_esq = 0;
uint16_t memoria_dir = 0;

int main(void) // Correção para compilar sem erros no Zephyr
{
    // Obtém o dispositivo da Porta E
    const struct device *gpio_dev = DEVICE_DT_GET(PORTA_E_NODE);

    // --- INICIALIZAÇÃO DOS MOTORES (PWM) ---
    pwm_tpm_Init(TPM2, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Init(TPM0, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);
    pwm_tpm_Init(TPM1, TPM_PLLFLL, TPM_MODULE, TPM_CLK, PS_128, EDGE_PWM);

    // Configuração dos pinos das rodas
    pwm_tpm_Ch_Init(TPM0, 1, TPM_PWM_H, GPIOA, 4);  // Esq Trás
    pwm_tpm_Ch_Init(TPM0, 2, TPM_PWM_H, GPIOA, 5);  // Esq Frente
    pwm_tpm_Ch_Init(TPM1, 0, TPM_PWM_H, GPIOA, 12); // Dir Trás
    pwm_tpm_Ch_Init(TPM0, 4, TPM_PWM_H, GPIOD, 4);  // Dir Frente

    // --- CONFIGURAÇÃO DOS LEDS (Laranja Fixo) ---
    pwm_tpm_Ch_Init(TPM2, 0, TPM_PWM_H, GPIOB, 18); 
    pwm_tpm_Ch_Init(TPM2, 1, TPM_PWM_H, GPIOB, 19); 
    pwm_tpm_CnV(TPM2, 0, 500); 
    pwm_tpm_CnV(TPM2, 1, 830); 

    // --- CONFIGURAÇÃO DOS SENSORES (ENTRADA) ---
    gpio_pin_configure(gpio_dev, 20, GPIO_INPUT); // Sensor Esq 
    gpio_pin_configure(gpio_dev, 21, GPIO_INPUT); // Sensor Dir 

    while (1) {
        int s_esq = gpio_pin_get(gpio_dev, 20);
        int s_dir = gpio_pin_get(gpio_dev, 21);

        // --- REGRA 1: ATUALIZAÇÃO DAS MEMÓRIAS ---
        if (s_esq == 1 && s_dir == 1) {
            // Cruzamento: Cancela tudo para ir reto
            memoria_esq = 0;
            memoria_dir = 0;
        } 
        else if (s_esq == 1) {
            // Viu na esquerda: Renova curva esquerda e zera a direita
            memoria_esq = PERSISTENCIA_CURVA;
            memoria_dir = 0; 
        } 
        else if (s_dir == 1) {
            // Viu na direita: Renova curva direita e zera a esquerda
            memoria_dir = PERSISTENCIA_CURVA;
            memoria_esq = 0;
        }

        // --- REGRA 2: MOVIMENTAÇÃO ---
        if (memoria_esq > 0) {
            // CURVA PARA ESQUERDA
            pwm_tpm_CnV(TPM0, 2, 0);       // Esq Frente DESLIGADO
            pwm_tpm_CnV(TPM0, 1, 680);     // Esq Trás LIGADO (Pivô)
            pwm_tpm_CnV(TPM0, 4, v_curva); // Dir Frente LIGADO MAX
            pwm_tpm_CnV(TPM1, 0, 0);       // Dir Trás DESLIGADO 
            memoria_esq--; 
            memoria_dir = 0;
            
        }
        else if (memoria_dir > 0) {
            // CURVA PARA DIREITA
            pwm_tpm_CnV(TPM0, 2, v_curva); // Esq Frente LIGADO MAX
            pwm_tpm_CnV(TPM0, 1, 0);       // Esq Trás DESLIGADO 
            pwm_tpm_CnV(TPM0, 4, 0);       // Dir Frente DESLIGADO
            pwm_tpm_CnV(TPM1, 0, 670);     // Dir Trás LIGADO (Pivô)
            memoria_dir--;
            memoria_esq = 0;
            
        }
        else {
            // SEGUE RETO 
            pwm_tpm_CnV(TPM0, 2, v_cruzeiro_esq); // Esq Frente LIGADO
            pwm_tpm_CnV(TPM0, 4, v_cruzeiro_dir); // Dir Frente LIGADO
            pwm_tpm_CnV(TPM0, 1, 0);          // Esq Trás DESLIGADO
            pwm_tpm_CnV(TPM1, 0, 0);          // Dir Trás DESLIGADO
        }
 
    }
    
    return 0; // Fechamento obrigatório da função int main
}