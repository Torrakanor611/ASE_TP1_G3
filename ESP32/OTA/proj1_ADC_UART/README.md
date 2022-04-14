## Status: TESTED SUCCESSFULLY

## Description:
São configuradas a uart1 e a uart2.
É configurado a adc1 channel 6 (pino 34). Alterou-se o valor de atenuação da adc para a macro ADC_ATTEN_DB_11 para alcançar uma maior intervalo de voltagem

Com o FreeRTOS são criadas as tarefas uart_task1 e uart_task2.

Na 1a tarefa é lido da adc o valor da voltagem com multisampling(x64) e é enviado para a uart2 pela uart1.

Na 2a tarefa é lido do buffer de receção da uart2 o valor da voltagem enviado pela 1a tarefa e comparado ao último valor recebido, imprimindo no terminal se a voltagem é maior ou menor que o valor anterior. 

### recursos
código de colegas das semanas anteriores.

exemplo de single read adc [https://github.com/espressif/esp-idf/tree/c18a72134d649aa1985b9e43f8e98198c4eaebf8/examples/peripherals/adc/single_read/single_read](https://github.com/espressif/esp-idf/tree/c18a72134d649aa1985b9e43f8e98198c4eaebf8/examples/peripherals/adc/single_read/single_read)
