let control_handle_btn = document.getElementsByClassName("control-table-btn");

function SR_button_handle(){
    console.log("btn control clicked");
    let devId = `DEV-${String(currentDeviceView).padStart(3, '0')}`;
    let topicMqtt = `Indr_PT100/${devId}/control`
    let msg = {
        cmd: "sampleRate",
        value: parseInt(document.getElementsByClassName("control-table-lb")[0].value)
    }
    msg = JSON.stringify(msg);
    console.log(`topic: ${topicMqtt} msg: ${msg}`);
    publishMQTT(topicMqtt, msg);
}

function handle_data_response_from_devices(devId, response)
{
    response = response.toString();
    response = JSON.parse(response);
    if(response.cmd == "sampleRate" && response.status == "ok")
    {
        let devNumber = parseInt(devId.split('-')[1]);
        console.log(`devNum: ${devNumber}`);
        if(currentDeviceView === devNumber)
        {
            alert(`Sample rate ${devId} set to ${response.value}`);
        }
    }
}
