#include "sync_data_to_sv.h"

void save_offline_data()
{
    static uint32_t t_bk_data_offline;
    if (millis() - t_bk_data_offline > BACKUP_TIME_WHEN_DEVICE_OFFLINE)
    {
        if (myRam.data_sync.temp_ptr >= BUFFER_TEMP_DATA_BACKUP_SIZE)
        {
            myRam.data_sync.temp_ptr = 0;
            myRam.data_sync.time_ptr = 0;
            // state_generate_buf_temp_time_bk = 0;
            delete[] myRam.data_sync.buf_temp; // Deallocate the entire array.
            myRam.data_sync.buf_temp = nullptr;

            delete[] myRam.data_sync.buf_time; // Deallocate the entire array.
            myRam.data_sync.buf_time = nullptr;
            ESP_LOGD(TAG,"FULLLLLLLL");
            myRam.data_sync.buf_temp = new float[BUFFER_TEMP_DATA_BACKUP_SIZE];
            myRam.data_sync.buf_time = new String[BUFFER_TIME_DATA_BACKUP_SIZE];
            myRam.data_sync.is_remaining_data = 0;
        }
        else
        {
            myRam.data_sync.buf_temp[myRam.data_sync.temp_ptr] = myRam.pt100_data.temp;
            myRam.data_sync.buf_time[myRam.data_sync.time_ptr] = myRam.ntp_time.ntpTimeString;
            myRam.data_sync.temp_ptr++;
            myRam.data_sync.time_ptr++;
            myRam.data_sync.is_remaining_data = 1;
        }
        ESP_LOGD(TAG,"ESP RAM MEMORY: %d totalTempPtr: %d totalTimePtr: %d", ESP.getFreeHeap(), myRam.data_sync.temp_ptr, myRam.data_sync.time_ptr);
        t_bk_data_offline = millis();
    }
    
      
}

void init_sync_data()
{
    // allocate size for buf temp and time
    myRam.data_sync.buf_temp = new float[BUFFER_TEMP_DATA_BACKUP_SIZE];
    myRam.data_sync.buf_time = new String[BUFFER_TIME_DATA_BACKUP_SIZE];
    myRam.data_sync.sync_state = FIRST_STARTUP_MEMORY;
}

void handle_sync_data()
{
    static uint32_t t_send_data_to_sv;
    switch (myRam.data_sync.sync_state)
    {
    case START_ASSIGN_DATA_TO_RAM:
        {
            save_offline_data();
            if (myRam.mqtt_config_data.isConnectToBroker == 1 && myRam.data_sync.is_remaining_data == 1)
            {
                myRam.data_sync.sync_state = START_SYNC_RAM_DATA_TO_SV; 
            }
            break;
        }
    case START_SYNC_RAM_DATA_TO_SV:
        {
            t_send_data_to_sv = millis();
            ESP_LOGD(TAG, "Start sending data to server");
            myRam.data_sync.sync_state = WAIT_TO_SEND_RAM_DATA_TO_SV;
            break;
        }
    case WAIT_TO_SEND_RAM_DATA_TO_SV:
        {
            if (millis() - t_send_data_to_sv > 1000)
            {
            //   state_sync_data_sv = 2;
                myRam.data_sync.sync_state = START_SEND_RAM_DATA_TO_SV;
                t_send_data_to_sv = millis();
            }
            break;
        }
    case START_SEND_RAM_DATA_TO_SV:
        {
            // myRam.data_sync.sync_state = CLEAR_RAM_DATA;
            break;
        }
    case CLEAR_RAM_DATA:
        {
            ESP_LOGD(TAG, "CLEAR ALL SYNC DATA");
            myRam.data_sync.temp_ptr = 0;
            myRam.data_sync.time_ptr = 0;
            // state_generate_buf_temp_time_bk = 0;
            delete[] myRam.data_sync.buf_temp; // Deallocate the entire array.
            myRam.data_sync.buf_temp = nullptr;

            delete[] myRam.data_sync.buf_time; // Deallocate the entire array.
            myRam.data_sync.buf_time = nullptr;
            myRam.data_sync.buf_temp = new float[BUFFER_TEMP_DATA_BACKUP_SIZE];
            myRam.data_sync.buf_time = new String[BUFFER_TIME_DATA_BACKUP_SIZE];
            delay(2000);
            myRam.data_sync.sync_state = DONE_SYNC_RAM_DATA_TO_SV;
            // myRam.data_sync.start_sync_data_to_sv = 0;
            // state_sync_data_sv = 0;
            break;
        }
    default:
        break;
    }
}
