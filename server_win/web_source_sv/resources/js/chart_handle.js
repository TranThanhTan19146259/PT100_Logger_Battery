let has_data = 0;
let lastest_data_updated = 0
let myChart;
let mqtt_data_input;

// Function to load initial data - accessible from other JS files
function loadInitialData(inputData) {
    // Update specific data points based on time
    if (inputData && inputData.time_his && inputData.temp_his) {
        for (let i = 0; i < inputData.time_his.length; i++) {
            const time = inputData.time_his[i];
            const temp = inputData.temp_his[i];

            // Find if this time already exists in the chart
            const existingIndex = myChart.data.labels.indexOf(time);

            if (existingIndex !== -1) {
                // Update existing data point
                myChart.data.datasets[0].data[existingIndex] = temp;
            } else {
                // Add new data point (for initial load)
                myChart.data.labels.push(time);
                myChart.data.datasets[0].data.push(temp);
                lastest_data_updated = temp;
            }
        }
    }

    myChart.update();
}

function update_chart()
{
    loadInitialData(mqtt_data_input)
}

document.addEventListener('DOMContentLoaded', function () {
    const ctx = document.getElementById('myChart').getContext('2d');
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
        const now = new Date();
        const timeLabel = now.getHours() + ':' +
            String(now.getMinutes()).padStart(2, '0') + ':' +
            String(now.getSeconds()).padStart(2, '0');

        myChart.data.labels.push(timeLabel);
        // if(has_data == 0)
        // {
        // }
        myChart.data.datasets[0].data.push(lastest_data_updated);

        // Remove old data if we have too many points
        if (myChart.data.labels.length > maxDataPoints) {
            myChart.data.labels.shift();
            myChart.data.datasets[0].data.shift();
        }

        // Update chart
        myChart.update();
    }

    // Button click handler
    document.getElementById('loadBtn').addEventListener('click', function () {
        const input = document.getElementById('dataInput').value;
        const errorMsg = document.getElementById('errorMsg');

        try {
            // Parse JSON input
            const inputData = JSON.parse(input);

            // Validate data structure
            if (!inputData.temp || !inputData.time) {
                throw new Error('Data must contain "temp" and "time" arrays');
            }

            if (!Array.isArray(inputData.temp) || !Array.isArray(inputData.time)) {
                throw new Error('"temp" and "time" must be arrays');
            }

            if (inputData.temp.length !== inputData.time.length) {
                throw new Error('"temp" and "time" arrays must have the same length');
            }

            // Load the data
            loadInitialData(mqtt_data_input);
            errorMsg.textContent = '✓ Data loaded successfully!';
            errorMsg.style.color = '#27ae60';

            // Clear success message after 3 seconds
            setTimeout(() => {
                errorMsg.textContent = '';
            }, 3000);

        } catch (e) {
            errorMsg.textContent = '✗ Error: ' + e.message;
            errorMsg.style.color = '#e74c3c';
        }
    });

    // Update chart every 1 second (1000ms)
    setInterval(addData, 1000);
    setInterval(update_chart, 1000);

    // Add first data point immediately
    addData();
});