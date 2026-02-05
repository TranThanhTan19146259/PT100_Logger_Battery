let count_req_times = 0;

function parseDate(str) {
    if (!str || typeof str !== "string") return null;

    const parts = str.trim().split(" ");
    if (parts.length !== 2) return null;

    const [datePart, timePart] = parts;

    const dmy = datePart.split("/").map(Number);
    const hms = timePart.split(":").map(Number);

    if (dmy.length !== 3 || hms.length !== 3) return null;

    const [d, m, y] = dmy;
    const [hh, mm, ss] = hms;

    if ([d, m, y, hh, mm, ss].some(n => !Number.isFinite(n))) {
        return null;
    }

    return new Date(y, m - 1, d, hh, mm, ss);
}

const tbody = document.getElementById("historyTableBody");

// Track existing timestamps → prevents duplicates
const timeIndex = new Set();

// throttle sorting
let sortScheduled = false;

function sort_table_safe() {
    const tbody = document.getElementById("historyTableBody");
    if (!tbody) return;

    const rows = Array.from(tbody.rows);
    if (rows.length <= 1) return;

    const seen = new Set();
    const uniqueRows = [];

    for (const row of rows) {
        // Skip placeholder or broken rows
        if (row.cells.length < 3) continue;

        const timeText = row.cells[1].textContent.trim();
        const tempText = row.cells[2].textContent.trim();

        // 🔑 Dedup key (time + temp)
        const key = `${timeText}|${tempText}`;

        if (seen.has(key)) {
            // duplicate → remove from DOM
            row.remove();
            continue;
        }

        seen.add(key);

        // Ensure timestamp exists
        if (!row.dataset.ts) {
            const d = parseDate(timeText);
            row.dataset.ts = d ? d.getTime() : 0;
        }

        uniqueRows.push(row);
    }

    // 🔥 Sort ASC by timestamp
    uniqueRows.sort((a, b) => Number(a.dataset.ts) - Number(b.dataset.ts));

    // 🔁 Re-append + re-number
    const frag = document.createDocumentFragment();
    uniqueRows.forEach((row, i) => {
        row.cells[0].textContent = i + 1;
        frag.appendChild(row);
    });

    tbody.appendChild(frag);
}

document.addEventListener("DOMContentLoaded", () => {
    const historyScroll = document.getElementById("historyScroll");
    const tbody = document.getElementById("historyTableBody");

    if (!historyScroll || !tbody) {
        console.error("History elements not found");
        return;
    }

    let scrollTimer = null;

    historyScroll.addEventListener("scroll", () => {
        console.log("HISTORY TABLE SCROLLED");

        // clear previous timer
        clearTimeout(scrollTimer);

        // run sort ONLY after scrolling stops
        scrollTimer = setTimeout(() => {
            console.log("Sorting after scroll stop");
            sort_table_safe();
        }, 300);
    });
});

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
}



let prevData = null;

function isChanged(oldObj, newObj)
{
    const oldArr = oldObj?.time_his;
    const newArr = newObj?.time_his;

    if (oldArr.length !== newArr.length){
        return true;
    }
    return newArr.some((val, i) => val !== oldArr[i]);
}



function update_history_table(inputData)
{
    let his_table = document.getElementById("historyTableBody");
    // update table rows after adding new data points;
    let lastest_table_rows = his_table.rows.length;
    
    /*
    
    */

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
            // let change = false;
            
            // if (prevData){
            //     change = isChanged(prevData, inputData);
            //         // JSON.stringify(inputData.time_his) != JSON.stringify(prevData.time_his)
            // }
            // console.log(inputData);
            // console.log(prevData);

            inputData.time_his.forEach((time, i) =>{
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

            })
            
            // if (change || !prevData)
            // {
            //     // console.log("Data changed");
            // }
            // prevData = structuredClone(inputData);


            
        }
    }
    // re-arrange history table after getting data from device
    // sort_table();
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