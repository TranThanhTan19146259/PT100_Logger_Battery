function generate_json_mpu_1()
{
    let allDataSettings = document.getElementsByClassName("MPU-tab-input");
    let i = 0;
    let myObj;

    myObj = {
        gyroScale: parseInt(allDataSettings[0].value),
        accelScale: parseInt(allDataSettings[1].value),
        sample: parseInt(allDataSettings[2].value),
        // timeSend: parseInt(allDataSettings[6].value)
    }

    console.log(JSON.stringify(myObj));// DEBUG
    return JSON.stringify(myObj);
}

// function generate_json_mpu_2()
// {
//     let allDataSettings = document.getElementsByClassName("MPU-tab-input");
//     let i = 0;
//     let myObj;

//     myObj = {
//         gyroScale: parseInt(allDataSettings[1].value),
//         accelScale: parseInt(allDataSettings[3].value),
//         sample: parseInt(allDataSettings[5].value),
//         timeSend: parseInt(allDataSettings[7].value)
//     }

//     console.log(JSON.stringify(myObj));// DEBUG
//     return JSON.stringify(myObj);
// }


function send_data_mpu_config_1()
{
    let jsonData = generate_json_mpu_1();

    postRequestHttp(baseUrl,"config_mpu1","",jsonData, (dataResponse) =>{
        console.log(dataResponse);
    });
    // console.log(jsonData_1);
    // var xhttp = new XMLHttpRequest();
    // xhttp.onreadystatechange = function () {
    // if (xhttp.readyState == 4 && xhttp.status == 200)
    //     callback(xhttp.responseText);
    // };
    // xhttp.open("POST", "/mpu_tab?msg=" + jsonData, true);
    // // xhttp.open("POST", "/mpu_tab", true);
    // // xhttp.setRequestHeader("Content-type", "application/json");
    // xhttp.send();

}


// function send_data_mpu_config_2()
// {
//     let jsonData = generate_json_mpu_2();
//     postRequestHttp(baseUrl,"config_mpu2","",jsonData,(dataResponse) =>{
//         console.log(dataResponse);
//     });

// }


function generate_mpu_content(jsonData)
{
    let allDataSettings = document.getElementsByClassName("MPU-tab-input");
    const obj = JSON.parse(jsonData);
    allDataSettings[0].value = obj.gyroScale[0];
    allDataSettings[1].value = obj.accelScale[0];
    allDataSettings[2].value = obj.sample[0];
    // allDataSettings[3].value = obj.accelScale[1];
    // allDataSettings[4].value = obj.sample[0];
    // allDataSettings[5].value = obj.sample[1];
    // allDataSettings[6].value = obj.timeSend[0];
    // allDataSettings[7].value = obj.timeSend[1];
    // console.log(obj.accelScale[1]);
}

function get_data_mpu()
{   
    // var xhttp = new XMLHttpRequest();
    getRequestHttp(baseUrl,"get_mpu_data","",(dataResponse) =>{
        console.log(dataResponse);
        generate_mpu_content(dataResponse);
    });
}