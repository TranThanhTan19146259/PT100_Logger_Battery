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

let prev_dataJson = null;
function isDuplicated(oldObj, newObj){
    const oldArr = oldObj?.time_his;
    const newArr = newObj?.time_his;

    if (oldArr.length !== newArr.length){
        return true;
    }
    return newArr.some((val, i) => val !== oldArr[i]);
}

function update_new_data_to_database(devNumber, dataJson)
{
    let data = {
        temp: dataJson.temp_his,
        time: dataJson.time_his
    };
    data = JSON.stringify(data);
    // console.log(`recv from ${devNumber}: ${data}`);
    let change = false;
    // if(prev_dataJson){
    //     change = isDuplicated(prev_dataJson, dataJson);
    // }
    
    // if (change || !prev_dataJson){
    // }
    putRequestHttp(db_url_base, db_enpoint, String(devNumber), data, (response) => {
        // console.log(response);
    });
    console.log(change);
    console.log(dataJson);
    prev_dataJson = structuredClone(dataJson);
    
}

