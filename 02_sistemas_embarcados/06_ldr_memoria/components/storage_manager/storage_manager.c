#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "esp_littlefs.h"

static const char *TAG = "LITTLEFS";

void init_littlefs(void)
{
    ESP_LOGI(TAG, "Initializing LittleFS");

    // Configuração para montar a partição
    esp_vfs_littlefs_conf_t conf = {
        .base_path = "/storage",        // Caminho onde o sistema de arquivos ficará acessível
        .partition_label = "storage",   // Rótulo da partição (DEVE ser o mesmo do partitions.csv)
        .format_if_mount_failed = true, // Formata se a montagem falhar (ex: na 1ª vez)
        .dont_mount = false,            // Queremos montar agora
    };

    // Registra (monta) o sistema de arquivos
    esp_err_t ret = esp_vfs_littlefs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find LittleFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize LittleFS (%s)", esp_err_to_name(ret));
        }
        return;
    }

    ESP_LOGI(TAG, "LittleFS mounted");
}

void app_main(void)
{
    init_littlefs();

    // ====== Escrita de arquivo ======
    ESP_LOGI(TAG, "Writing to file...");

    // 1. Abre o arquivo em modo de escrita ("w")
    FILE *f = fopen("/storage/hello.txt", "w");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for writing");
        return;
    }

    // 2. Escreve no arquivo
    fprintf(f, "Olá, LittleFS!");

    // 3. Fecha o arquivo
    fclose(f);
    ESP_LOGI(TAG, "File written");

    ESP_LOGI(TAG, "Reading from file...");

    // ====== Leitura de arquivo ======
    // 1. Abre o arquivo em modo de leitura ("r")
    f = fopen("/storage/hello.txt", "r");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    // 2. Pula para o final do arquivo para descobrir o tamanho
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    // Volta para o início do arquivo
    fseek(f, 0, SEEK_SET); 

    // 3. Aloca memória para guardar o conteúdo (+1 para o caractere nulo)
    char *content = malloc(fsize + 1);

    // 4. Lê o arquivo para o buffer
    fread(content, 1, fsize, f);

    // 5. Fecha o arquivo
    fclose(f);

    // Adiciona o terminador de string
    content[fsize] = 0;

    // Imprime o conteúdo
    ESP_LOGI(TAG, "File content: '%s'", content);

    // 6. Libera a memória! Essencial para evitar memory leaks.
    free(content);
}