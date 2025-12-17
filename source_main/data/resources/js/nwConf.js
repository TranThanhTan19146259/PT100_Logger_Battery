function send_data_mqtt()
{
    let mqtt_infor = document.getElementsByClassName("mqtt-tab-input");
    let jsonObj =
    {
        host:       mqtt_infor[0].value,
        port:       mqtt_infor[1].value,
        username:   mqtt_infor[2].value,
        password:   mqtt_infor[3].value
    }
    console.log(JSON.stringify(jsonObj));
    postRequestHttp(baseUrl,"config_mqtt","",JSON.stringify(jsonObj), (dataResponse) =>{
        // console.log(dataResponse);
        alert(dataResponse);
    });
}