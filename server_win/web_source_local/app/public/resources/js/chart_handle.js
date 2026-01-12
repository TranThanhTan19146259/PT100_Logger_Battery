let has_data = 0;
let lastest_data_updated = 0
let myChart;
let mqtt_data_input;
let chart_interval_addData;
let chart_interval_loadInitialData;


// Function to load initial data - accessible from other JS files
function loadInitialData(inputData) {
    try{
        inputData = JSON.parse(inputData)
        // console.log(`Received: `, inputData.time_his);
    }
    catch {

    }
    // Update specific data points based on time
    if (inputData && inputData.time_his && inputData.temp_his) {
        for (let i = 0; i < inputData.time_his.length; i++) {
            
            
            const time = inputData.time_his[i].split(' ')[1];
            const temp = inputData.temp_his[i];

            // Find if this time already exists in the chart
            const existingIndex = myChart.data.labels.indexOf(time);

            if (existingIndex !== -1) {
                // Update existing data point
                myChart.data.datasets[0].data[existingIndex] = temp;
                // clear existing data
                if(existingIndex == 0)
                {
                    mqtt_data_input = ""
                }
            } else {
                // Add new data point (for initial load)
                myChart.data.labels.push(time);
                myChart.data.datasets[0].data.push(lastest_data_updated);
            }
        }
        // console.log(`update_chart: ${lastest_data_updated}`)
    }

    myChart.update();
}

function update_chart()
{
    loadInitialData(mqtt_data_input)
    // try
    // {
    // }
    // catch
    // {

    // }
}


function load_chart()
{
    let chartId = `temperatureChart DEV - ${String(currentDeviceView)}`;
    const ctx = document.getElementById(chartId).getContext('2d');
    const maxDataPoints = 50;

    // Initialize chart data
    const data = {
        labels: [],
        datasets: [{
            label: 'Live Data',
            data: [],
            borderColor: 'rgb(75, 192, 192)',
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            tension: 0.4,
            fill: true
        }]
    };
    // Create chart
    myChart = new Chart(ctx, {
        type: 'line',
        data: data,
        options: {
            responsive: true,
            maintainAspectRatio: false,
            animation: {
                duration: 750
            },
            scales: {
                y: {
                    beginAtZero: true,
                    max: 5000
                },
                x: {
                    display: true
                }
            },
            plugins: {
                legend: {
                    display: true
                }
            }
        }
    });
    // Function to add new data
    function addData() {
    try
    {
        const now = new Date();
        const timeLabel = now.getHours() + ':' +
            String(now.getMinutes()).padStart(2, '0') + ':' +
            String(now.getSeconds()).padStart(2, '0');
        lastest_data_updated = parseInt(lastest_data_updated);
        myChart.data.labels.push(timeLabel);
        myChart.data.datasets[0].data.push(lastest_data_updated);
        // Remove old data if we have too many points
        if (myChart.data.labels.length > maxDataPoints) {
            myChart.data.labels.shift();
            myChart.data.datasets[0].data.shift();
        }

        // Update chart
        myChart.update();
    }
    catch
    {

    }
}

    // Button click handler
    // Update chart every 1 second (1000ms)
    // myChart.destroy();
    chart_interval_addData = setInterval(addData, 1000);
    chart_interval_loadInitialData = setInterval(update_chart, 1000);
    // Add first data point immediately
    addData();
}

function remove_chart()
{
    let newId = 'temperatureChart';
    let oldId = `temperatureChart DEV - ${String(currentDeviceView)}`;
    document.getElementById(oldId).id = newId;
    // console.log("remove all data");
    // if (myChart) {
    //     myChart.destroy();
    // }
    // myChart.data.datasets[0].data[-1] = 0;
    mqtt_data_input = ""
    lastest_data_updated = 0;
    myChart.destroy();
    clearInterval(chart_interval_addData);
    clearInterval(chart_interval_loadInitialData);
}