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
            location: `Building ${String.fromCharCode(65 + Math.floor((i-1) / 10))}, Floor ${((i-1) % 5) + 1}`,
            lastUpdated: 'Never',
            temperature: '--',
            time: '--'
        };
    }
}

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
            
            // Find device number from ID
            const deviceNumber = parseInt(deviceId.split('-')[1]);
            try {
                // Parse the JSON message
                const data = JSON.parse(message.toString());
                console.log(`Received data from ${deviceId}:`, data);
                
                // Update device data
                if (deviceData[deviceNumber]) {
                    deviceData[deviceNumber].temperature = data.temp || '--';
                    deviceData[deviceNumber].time = data.time || '--';
                    deviceData[deviceNumber].status = 'online';
                    deviceData[deviceNumber].lastUpdated = new Date().toLocaleTimeString();
                    
                    // If we're viewing this device, update the display
                    if (currentDeviceView === deviceNumber) {
                        mqtt_data_input = message.toString();
                        update_history_table(mqtt_data_input);
                        updateDeviceDisplay(deviceNumber);
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
    // document.getElementById('deviceLocation').textContent = device.location;
    document.getElementById('lastUpdated').textContent = device.lastUpdated;
    
    // Update status badge
    const statusBadge = document.getElementById('deviceStatus');
    statusBadge.textContent = device.status.charAt(0).toUpperCase() + device.status.slice(1);
    statusBadge.className = `status-badge status-${device.status}`;
    
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
    // lastest_data_updated = device.temperature

    // console.log(lastest_data_updated);

    if(!isNaN(parseInt(device.temperature))) 
    {
        // remove_chart();
        lastest_data_updated = (device.temperature);   
    }
    else lastest_data_updated = 0;
    // try
    // {

    // }
    // catch
    // {
    //     lastest_data_updated = 0;
    // }
}

// Initialize MQTT connection when script loads
connectMQTT();