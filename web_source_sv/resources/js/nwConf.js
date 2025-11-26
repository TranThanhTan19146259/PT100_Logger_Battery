let radio_select_protocol = 0;

function generate_json_network(pair_device_mode)
{
    let protocol_input = document.getElementsByClassName("pairkey-input");
    let myObj = {
        pair_key: protocol_input[pair_device_mode].value,
    };
    console.log(JSON.stringify(myObj));// DEBUG
    return JSON.stringify(myObj);
}

function send_data_network_config()
{
    if(radio_select_protocol == 0)
    {
        let jsonData = generate_json_network(radio_select_protocol);
        postRequestHttp(baseUrl,"add_new_device","",jsonData, (dataResponse) =>{
            console.log(dataResponse);
            alert(dataResponse);
        });
    }
    else
    {
        let jsonData = generate_json_network(radio_select_protocol);
        postRequestHttp(baseUrl,"change_pair_key","",jsonData, (dataResponse) =>{
            alert(dataResponse);
            console.log(dataResponse);
        });
    }
}


function change_protocol_mode()
{
    let select_protocol = document.getElementsByClassName("radio-select-protocol");
    let main_content = document.getElementsByClassName("protocol-select");
    let protocol_input = document.getElementsByClassName("pairkey-input");
    for(let i =0;i<select_protocol.length;i++)
    {
        select_protocol[i].addEventListener('change', function() {
            for (let i =0;i<select_protocol.length;i++)
            {
                protocol_input[i].value = "";
                main_content[i].classList.remove("active-content");
            }
            if (this.checked) {
                radio_select_protocol = i;
                // console.log(radio_select_protocol);
                main_content[i].classList.add("active-content");
            } 
          });
    }

}

