function generate_device_fields()
{
    getRequestHttp(db_url_base, db_enpoint, "", (response, status) =>{
        if(response !== "")
        {
            let _devices_fields = JSON.parse(response);
            total_devices = _devices_fields.length;
            console.log(`total devices: ${_devices_fields.length}`);
            generateDeviceList();
            generate_device_data();
        }

    });
}
document.addEventListener("DOMContentLoaded", function() {
    generate_device_fields();
});


function update_new_data_to_database()
{
    const tbody = document.getElementById('historyTableBody');
    const rows = tbody.querySelectorAll('tr');

    const temp = [];
    const time = [];

    // Loop through each row
    rows.forEach(row => {
    const cells = row.querySelectorAll('td');
    
    // cells[0] = No, cells[1] = Time, cells[2] = Temperature
    if (cells.length >= 3) {
        time.push(cells[1].textContent.trim());
        temp.push(parseInt(cells[2].textContent.trim()));
    }
    });

    // Create the object
    const data = {
    temp: temp,
    time: time
    };

    // Convert to JSON string
    const jsonString = JSON.stringify(data);

    console.log(`data ready update to db: ${jsonString}`);
    putRequestHttp(db_url_base, db_enpoint, String(currentDeviceView), jsonString, (response) => {
            console.log(response);
    });
}