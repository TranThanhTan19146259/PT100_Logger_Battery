let count_req_times = 0;

function generate_history_table()
{
    // get a reference to his_table
    let his_table = document.getElementById("historyTableBody");
    getRequestHttp(db_url_base, db_enpoint, String(currentDeviceView), (response) =>{
        if(response !== "")
        {
            if(count_req_times == 0)
            {
                try
                {
                    response = JSON.parse(response);
                    for(let i = 0; i < response.time.length; i++)
                    {
                        let newRow = his_table.insertRow(-1);
                        // Insert a cell in the row at index 0
                        let noCell = newRow.insertCell(0);
                        let timeCell = newRow.insertCell(1);
                        let tempCell = newRow.insertCell(2);
                        let noText = document.createTextNode(String(i));
                        let timeText = document.createTextNode(String(response.time[i]));
                        let tempText = document.createTextNode(String(response.temp[i]));
                        noCell.appendChild(noText);
                        timeCell.appendChild(timeText);
                        tempCell.appendChild(tempText);
                        count_req_times ++;
                    }
                    his_table.deleteRow(0); // remove "No data row"
                }
                catch
                {
                    
                }
                // console.log(`total table rows: ${his_table.rows.length}`)
            }
        }
    });
    // insert row to his_table
    // let newRow = his_table.insertRow(-1);

    // // Insert a cell in the row at index 0
    // let timeCell = newRow.insertCell(0);
    // let tempCell = newRow.insertCell(1);

    // Append a text node to the cell

    // let tempText = document.createTextNode("30");
    // tempCell.appendChild(tempText);
    // let timeText = document.createTextNode("10:30");
    // timeCell.appendChild(timeText);
    // let dev_json_obj = {
    //     devId: "DEV-001",
    //     temp: [30,40,50,60,100,50,80],
    //     time: ["10:30:50","10:30:51","10:30  :52","10:30:53","10:30:54","10:30:55","10:30:56"]
    // }
    // dev_json_obj = JSON.stringify(dev_json_obj);
    // console.log(dev_json_obj);
    // putRequestHttp(db_url_base, db_enpoint, "/1", dev_json_obj, (response) => {
    //     console.log(response);
    // });
    // delRequestHttp(db_url_base, db_enpoint, "/30", (response) => {
    //     console.log(response);
    // });
    
    // console.log("table history");
}

function scroll_history_table_handle()
{
    
}



function update_history_table(inputData)
{
    let his_table = document.getElementById("historyTableBody");
    // update table rows after adding new data points;
    let lastest_table_rows = his_table.rows.length;
    try{
        inputData = JSON.parse(inputData)
        // console.log(`Received: `, inputData.time_his);
    }
    catch {

    }
    // Update specific data points based on time
    if (inputData && inputData.time_his && inputData.temp_his) {
        for (let i = 0; i < inputData.time_his.length; i++) {
            const time = inputData.time_his[i];
            const temp = inputData.temp_his[i];
            let newRow = his_table.insertRow(-1);
            // Insert a cell in the row at index 0
            let noCell = newRow.insertCell(0);
            let timeCell = newRow.insertCell(1);
            let tempCell = newRow.insertCell(2);
            let noText = document.createTextNode(String(i + lastest_table_rows));
            let timeText = document.createTextNode(String(time));
            let tempText = document.createTextNode(String(temp));
            noCell.appendChild(noText);
            timeCell.appendChild(timeText);
            tempCell.appendChild(tempText);
        }
    }
    
    // update_new_data_to_database();
}

function remove_history_table()
{
    let his_table = document.getElementById("historyTableBody");
    let total_table_rows = his_table.rows.length;
    for(let i = 0; i < total_table_rows; i++)
    {
        his_table.deleteRow(0); // remove all table rows
    }
    // create a first row after deleting it when website get data from DB
    // Get the specific table
    const tbody = document.getElementById('historyTableBody');

    // Create the row
    const row = document.createElement('tr');
    const cell = document.createElement('td');

    cell.colSpan = 3;
    cell.style.textAlign = 'center';
    cell.style.color = '#999';
    cell.textContent = 'No data available';

    row.appendChild(cell);

    // Add the row to the tbody
    tbody.appendChild(row);
}