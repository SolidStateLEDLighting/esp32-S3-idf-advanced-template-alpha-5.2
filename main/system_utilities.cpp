#include "system_.hpp"

#include "esp_efuse.h"
#include "esp_efuse_table.h"

SemaphoreHandle_t semSysBoolLock = NULL;
SemaphoreHandle_t semSysUint8Lock = NULL;

const char *System::convertWifiStateToChars(uint8_t state)
{
    const char *rc = nullptr;

    if (state == 0)
        return "NONE";
    else if (state == 1)
        rc = "WIFI_READY_TO_CONNECT";
    else if (state == 2)
        rc = "WIFI_CONNECTING_STA";
    else if (state == 3)
        rc = "WIFI_CONNECTED_STA";
    else if (state == 4)
        rc = "WIFI_DISCONNECTING_STA";
    else if (state == 5)
        rc = "WIFI_DISCONNECTED";
    else
        rc = "Unknown";

    return rc;
}

std::string System::getDeviceID()
{
    uint32_t out_hmac_key[4];
    size_t hmac_key_size = 16;
    esp_efuse_read_field_blob(ESP_EFUSE_OPTIONAL_UNIQUE_ID, out_hmac_key, hmac_key_size * 8);

    std::ostringstream id;
    id << std::hex << __builtin_bswap32(out_hmac_key[0]); // We are swapping big-endian/little-endian so that our results match what
    id << std::hex << __builtin_bswap32(out_hmac_key[1]); // The espefuse tool will print out with the summary command
    id << std::hex << __builtin_bswap32(out_hmac_key[2]);
    id << std::hex << __builtin_bswap32(out_hmac_key[3]);
    return id.str();
}

bool System::lockGetBool(bool *variable)
{
    bool value = false;
    if (xSemaphoreTake(semSysBoolLock, portMAX_DELAY))
    {
        value = *variable; // Dereference and return the value
        xSemaphoreGive(semSysBoolLock);
    }
    return value;
}

void System::lockSetBool(bool *variable, bool value)
{
    if (xSemaphoreTake(semSysBoolLock, portMAX_DELAY))
    {
        *variable = value; // Dereference and set the value
        xSemaphoreGive(semSysBoolLock);
    }
}

uint8_t System::lockGetUint8(uint8_t *variable)
{
    uint8_t value = 0;
    if (xSemaphoreTake(semSysUint8Lock, portMAX_DELAY))
    {
        value = *variable; // Dereference and return the value
        xSemaphoreGive(semSysUint8Lock);
    }
    return value;
}

void System::lockOrUint8(uint8_t *variable, uint8_t value)
{
    if (xSemaphoreTake(semSysUint8Lock, portMAX_DELAY))
    {
        *variable |= value; // Dereference and set the value
        xSemaphoreGive(semSysUint8Lock);
    }
}

void System::lockAndUint8(uint8_t *variable, uint8_t value)
{
    if (xSemaphoreTake(semSysUint8Lock, portMAX_DELAY))
    {
        *variable &= value; // Dereference and set the value
        xSemaphoreGive(semSysUint8Lock);
    }
}

void System::lockSetUint8(uint8_t *variable, uint8_t value)
{
    if (xSemaphoreTake(semSysUint8Lock, portMAX_DELAY))
    {
        *variable = value; // Dereference and set the value
        xSemaphoreGive(semSysUint8Lock);
    }
}
