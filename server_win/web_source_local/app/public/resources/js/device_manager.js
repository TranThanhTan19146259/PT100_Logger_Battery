// device-list.js
// Device list generation and UI management
// let currentDeviceView = null;
// // Switch between tabs
function switchTab(tabName) {
  if (tabName === 'chart') {
      document.getElementById('chartTab').classList.add('active');
      document.getElementById('historyTab').classList.remove('active');
      document.getElementById('controlTab').classList.remove('active');
  } else if (tabName === 'history') {
      document.getElementById('historyTab').classList.add('active');
      document.getElementById('chartTab').classList.remove('active');
      document.getElementById('controlTab').classList.remove('active');
      console.log("tab table")
  } else if (tabName === 'control') {
      document.getElementById('controlTab').classList.add('active');
      document.getElementById('historyTab').classList.remove('active');
      document.getElementById('chartTab').classList.remove('active');
      // if (currentDeviceView) {
      //     updateHistoryTable(currentDeviceView);
      // }
  }
}





const buttonsPerRow = 3;

// Generate device buttons
function generateDeviceList() {
    const table = document.querySelector('.devices-container');
    
    // Clear existing content
    table.innerHTML = '';
    
    for (let i = 0; i < total_devices; i++) {
        // Create a new row every 3 buttons
        if (i % buttonsPerRow === 0) {
            var row = table.insertRow();
        }
        
        // Create cell and button
        const cell = row.insertCell();
        const button = document.createElement('button');
        button.textContent = `Device ${i + 1}`;
        button.className = 'device-btn';
        button.setAttribute('data-device-id', i + 1);
        // Add click event to show device details
        button.addEventListener('click', function() {
            showDeviceDetail(i + 1);
            load_chart();
        });
        
        cell.appendChild(button);
    }
    
    console.log(`Generated ${total_devices} device buttons`);
}

// Show device detail view
function showDeviceDetail(deviceNumber) {
    const device = deviceData[deviceNumber];
    currentDeviceView = deviceNumber;
    
    updateDeviceDisplay(deviceNumber);
    generate_history_table();
    // Hide device list and show detail view
    document.getElementById('deviceListView').style.display = 'none';
    document.getElementById('deviceDetailView').classList.add('active');
    // let class_list_chart = "DEV - " + String(deviceNumber);
    let oldId = 'temperatureChart';
    let newId = `temperatureChart DEV - ${String(deviceNumber)}`;
    document.getElementById(oldId).id = newId;
    // console.log(currentDeviceView);
    // let temp_chart = document.createElement("canvas");
    // temp_chart.setAttribute('id', 'temperatureChart');
}

// Show device list view
function showDeviceList() {
    count_req_times = 0;
    remove_chart();
    remove_history_table();
    currentDeviceView = null;
    // console.log(currentDeviceView);
    
    // Hide detail view and show device list
    document.getElementById('deviceDetailView').classList.remove('active');
    document.getElementById('deviceListView').style.display = 'block';
}

// Initialize device list when page loads
// window.addEventListener('DOMContentLoaded', function() {
    // generateDeviceList();
    // console.log('Device list initialized');
// });