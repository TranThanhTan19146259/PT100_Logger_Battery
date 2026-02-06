// mqtt-handler.js
// MQTT Configuration
const MQTT_BROKER = `ws://${mqtt_url}:8083/mqtt`;
const MQTT_USERNAME = mqtt_usr;
const MQTT_PASSWORD = mqtt_pass;
let total_devices = 0;

let mqttClient = null;
let currentDeviceView = null;

// Device data storage - shared with device-list.js
const deviceData = {};

// Initialize device data
function generate_device_data()
{
    for (let i = 1; i <= total_devices; i++) {
        deviceData[i] = {
            name: `Device ${i}`,
            id: `DEV-${String(i).padStart(3, '0')}`,
            status: 'offline',
            mac: 'unknown',
            ip: 'unknown',
            sr: 'unknown',
            rt: 'unknown',
            lastUpdated: 'Never',
            temperature: '--',
            time: '--'
        };
    }
}

let canRespond = true;
let canRespondHisData = true;
let lat_flag = false;
// Connect to MQTT Broker
function connectMQTT() {
    generate_mqtt_client_id();
    try {
            mqttClient = mqtt.connect(MQTT_BROKER, {
            clientId: mqtt_client_id,
            username: MQTT_USERNAME,
            password: MQTT_PASSWORD,
            clean: true,
            connectTimeout: 4000,
            reconnectPeriod: 1000,
        });

        mqttClient.on('connect', function () {
            console.log('Connected to MQTT broker');
            
            // Subscribe to all device topics
            const topic = `Indr_PT100/#`;
            mqttClient.subscribe(topic, function (err) {
                if (!err) {
                    console.log(`Subscribed to ${topic}`);
                } else {
                    console.error(`Failed to subscribe to ${topic}:`, err);
                }
            });
            // for (let i = 1; i <= total_devices; i++) {
            //     const deviceId = `DEV-${String(i).padStart(3, '0')}`;
            // }
        });

        mqttClient.on('message', function (topic, message) {
            // Parse the topic to get device ID
            const topicParts = topic.split('/');
            const deviceId = topicParts[1]; // DEV-001, DEV-002, etc.
            let lastestDataFromDevices = topicParts[2]; // this data come from devices, in order to ignore data sent from server to devices
            // Find device number from ID
            const deviceNumber = parseInt(deviceId.split('-')[1]);
            try {
                if(lastestDataFromDevices == "status")
                {
                    let devId = `DEV-${String(deviceNumber).padStart(3, '0')}`;
                    let resStatus = `Indr_PT100/${devId}/responseStatus` 
                    // if (message.toString() !== "ping") return;

                    if (!canRespond) {
                        return;
                    }
                
                    publishMQTT(resStatus, "pong");
                    lat_flag = true;
                
                    canRespond = false;
                    // unlock after 5 seconds
                    setTimeout(() => {
                        canRespond = true;
                        console.log("Ready for next ping");
                    }, 5000);
                }
                if(lastestDataFromDevices == "lastestData")
                {
                    // Parse the JSON message
                    const data = JSON.parse(message.toString());
                    console.log(`Received data from ${deviceId}:`, data);
                    update_new_data_to_database(deviceNumber, data);
                    devices_status_handle(deviceNumber);
                    let devId = `DEV-${String(deviceNumber).padStart(3, '0')}`;
                    let topicResLastestData = `Indr_PT100/${devId}/responseLastestData` 
                    response_lastest_Data = {
                                                time: data.time,
                                                status: "ok"
                                            };
                    response_lastest_Data_json = JSON.stringify(response_lastest_Data);
                    publishMQTT(topicResLastestData,response_lastest_Data_json);
                    // Update device data
                    if (deviceData[deviceNumber]) {
                        deviceData[deviceNumber].temperature = data.temp || '--';
                        deviceData[deviceNumber].time = data.time || '--';
                        deviceData[deviceNumber].status = 'online';
                        deviceData[deviceNumber].ip = data.ip;
                        deviceData[deviceNumber].mac = data.mac;
                        deviceData[deviceNumber].sr = data.SR;
                        deviceData[deviceNumber].rt = data.RT;
                        deviceData[deviceNumber].lastUpdated = new Date().toLocaleTimeString();
                        
                        // If we're viewing this device, update the display
                        if (currentDeviceView === deviceNumber) {
                            mqtt_data_input = message.toString();
                            update_history_table(mqtt_data_input);
                            updateDeviceDisplay(deviceNumber);
                        }
                    }
                }
                if(lastestDataFromDevices == "control")
                {
                    handle_data_response_from_devices(deviceId,message);
                }
                if(lastestDataFromDevices == "hisData")
                {
                    const data = JSON.parse(message.toString());
                    console.log(`recv his data from DEV ${deviceNumber}`);
                    let devId = `DEV-${String(deviceNumber).padStart(3, '0')}`;
                    let topicResHIsData = `Indr_PT100/${devId}/responseHisData` 
                    response_his_Data = {status: "ok"};
                    response_his_Data_json = JSON.stringify(response_his_Data);
                    update_new_data_to_database(deviceNumber, data);
                    if (!canRespondHisData) {
                        return;
                    }
                    
                    publishMQTT(topicResHIsData,response_his_Data_json);
                    // publishMQTT(resStatus, "pong");
                    lat_flag = true;
                
                    canRespondHisData = false;
                    // unlock after 5 seconds
                    setTimeout(() => {
                        canRespondHisData = true;
                        console.log("Ready for next resposne HisData");
                    }, 10);
                    if (currentDeviceView === deviceNumber) {
                            mqtt_data_input = message.toString();
                            update_history_table(mqtt_data_input);
                            // updateDeviceDisplay(deviceNumber);
                    }
                }

            } catch (e) {
                console.error('Error parsing MQTT message:', e);
            }
        });

        mqttClient.on('error', function (error) {
            console.error('MQTT Connection Error:', error);
        });

        mqttClient.on('offline', function () {
            console.log('MQTT client offline');
        });

        mqttClient.on('reconnect', function () {
            console.log('MQTT client reconnecting...');
        });

    } catch (error) {
        console.error('Failed to connect to MQTT broker:', error);
    }
}

// Update device display when viewing details
function updateDeviceDisplay(deviceNumber) {
  
    const device = deviceData[deviceNumber];
    
    // Update detail view with device information
    document.getElementById('deviceName').textContent = device.name;
    document.getElementById('deviceId').textContent = device.id;
    document.getElementById('lastUpdated').textContent = device.lastUpdated;
    document.getElementById('macAddress').textContent = device.mac;
    document.getElementById('deviceIp').textContent = device.ip;
    document.getElementById('SR').textContent = device.sr;
    document.getElementById('RT').textContent = device.rt;
    // Update status badge
    // const statusBadge = document.getElementById('deviceStatus');
    // statusBadge.textContent = device.status.charAt(0).toUpperCase() + device.status.slice(1);
    // statusBadge.className = `status-badge status-${device.status}`;
    
    // Update device data list with real-time data
    const dataList = document.getElementById('deviceData');
    dataList.innerHTML = '';
    
    const dataItems = [
        `Temperature: ${device.temperature}${typeof device.temperature === 'number' ? '°C' : ''}`,
        `Time: ${device.time}`
    ];
    
    dataItems.forEach(dataItem => {
        const li = document.createElement('li');
        li.textContent = dataItem;
        dataList.appendChild(li);
    });
    // data for drawing chart
    // if(!isNaN(parseFloat(device.temperature)))
    // {
    //     // Define your time window (e.g., only accept data from last 10 minutes)
    //     const now = new Date();
    //     const currentTimestamp = now.getTime();
    //     const timeWindowStart = currentTimestamp - (10 * 60 * 1000); // 10 minutes ago
    //     const timeWindowEnd = currentTimestamp + (1 * 60 * 1000); // 1 minute in future (buffer)
        
    //     // Assuming device has a timestamp field, check if it's within range
    //     // If device doesn't have timestamp, use current time
    //     const deviceTimestamp = device.time ? new Date(device.time).getTime() : currentTimestamp;
        
    //     if (deviceTimestamp >= timeWindowStart && deviceTimestamp <= timeWindowEnd) {
    //         lastest_data_updated = (device.temperature);
    //         console.log("✓ Temperature accepted:", device.temperature);
    //     } else {
    //         lastest_data_updated = 0;
    //         console.log("✗ Temperature rejected (old data)");
    //     }
    // }
    // else lastest_data_updated = 0;
    if(!isNaN(parseInt(device.temperature))) 
    {
        // remove_chart();
        lastest_data_updated = (device.temperature);   
    }
    else lastest_data_updated = 0;
}


function publishMQTT(topic, message)
{
    mqttClient.publish(topic, message,{ retain: false }, (err) => {
        if (err) {
            console.error('Failed to publish message:', err);
        } else {
            console.log(`Message published to topic "${topic}": "${message}"`);
        }
        // Optional: end the connection after publishing in a simple script
        // client.end(); 
    });
}

// Initialize MQTT connection when script loads
connectMQTT();