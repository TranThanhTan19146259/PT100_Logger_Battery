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


function update_new_data_to_database(devNumber, dataJson)
{
    let data = {
        temp: dataJson.temp_his,
        time: dataJson.time_his
    };
    data = JSON.stringify(data);
    console.log(`recv from ${devNumber}: ${data}`);

    putRequestHttp(db_url_base, db_enpoint, String(devNumber), data, (response) => {
            // console.log(response);
    });
}