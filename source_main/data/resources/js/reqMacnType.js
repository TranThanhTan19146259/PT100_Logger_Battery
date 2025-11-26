let json_raw_server_infor;
let machine_type = 1;
let machine_type_query_status = 0;
let mqtt_sv_query_status = 0;
let check_sv_status; 
function handle_query_machine_types()
{
    let pair_key_element = document.getElementsByClassName("pairkey-input")[0];
    
    pair_key_element.addEventListener("input", (e) =>{
        // console.log(e);
        check_valid_pairkey(pair_key_element);
    });
}

function handle_choose_machine_types()
{
    let machine_type_description = document.getElementsByClassName("machine-type-description");
    for(let i=0;i<machine_type_description.length;i++)
    {
        machine_type_description[i].addEventListener("click", function(){
            machine_type = i + 1;
            console.log(machine_type);
            for (let i = 0; i < machine_type_description.length; i++) {
                machine_type_description[i].classList.remove("active-machine-type");
            }
            if (!machine_type_description[i].classList.contains("active-machine-type"))
            {
                machine_type_description[i].classList.add("active-machine-type");
            }
            // generate_json_machine_sv_infor();
        })
    }
}

function generate_json_machine_sv_infor()
{
    // console.log(json_raw_server_infor);
    // console.log(machine_type);
    let json_sv_parse_obj = JSON.parse(json_raw_server_infor);
    let jsonObj ={
        name: json_sv_parse_obj.data[0].name,
        host: json_sv_parse_obj.data[0].host,
        port: json_sv_parse_obj.data[0].port,
        username: json_sv_parse_obj.data[0].username,
        password: json_sv_parse_obj.data[0].password,
        machine_type: machine_type,
        pair_key: String(document.getElementsByClassName("pairkey-input")[0].value)
    }
    let json_str = JSON.stringify(jsonObj);
    console.log(json_str);
    return json_str;
} 

function generate_machine_type_images(json_obj)
{
    // console.log(json_obj.data.length);
    let table_display = document.getElementsByClassName("machine-types-image-table");
    // let image_display = document.getElementsByClassName("machine-types-image");
    let machine_type_img = document.getElementsByClassName("machine-type-image");
    let machine_type_description = document.getElementsByClassName("machine-type-description");
    let total_machine_types = json_obj.data.length;
    for(let i = 0;  i< total_machine_types; i ++)
    {
        machine_type_img[i].src = json_obj.data[i].image;
        machine_type_description[i].innerHTML = json_obj.data[i].name;
        machine_type_img[i].classList.remove("machine-type-image-none");
        machine_type_img[i].classList.add("machine-type-image-display");
        machine_type_description[i].classList.remove("machine-type-description-none");
        machine_type_description[i].classList.add("machine-type-description-display");
        

        
    }

    table_display[0].classList.remove("images-table-none");
    table_display[0].classList.add("images-table-display");




}
let first_reach_to_6_chars;
let query_status;
function check_valid_pairkey(e)
{
    // alert(test_data);
    // if(e.value.lenght == 6)
    // {
    //     console.log("start querying");
    // }
    let counter_check_state = 0;
    let btn_submit_send_pair_data = document.getElementById("btn_send_pair_data");
    let query_status_label = document.getElementById("query-status");
    let query_state = 0;
    if(e.value.length < 6)
    {
        // generate_machine_type_images("");
        let table_display = document.getElementsByClassName("machine-types-image-table");
        table_display[0].classList.add("images-table-none");
        table_display[0].classList.remove("images-table-display");
        first_reach_to_6_chars = 0;
        machine_type_query_status = 0;
        mqtt_sv_query_status = 0;
        clearInterval(check_sv_status);
        btn_submit_send_pair_data.disabled = true;
        query_status_label.innerHTML = "";
    } 
    if(e.value.length == 6)
    {
        if (first_reach_to_6_chars == 0)
        {
            check_sv_status = setInterval(function() {
                if(machine_type_query_status == 1 && mqtt_sv_query_status == 1)
                {
                    btn_submit_send_pair_data.disabled = false;
                    query_state = true;
    
                }
                else
                {
                    btn_submit_send_pair_data.disabled = true;
                    query_state = false;
                }
                console.log(counter_check_state);
                counter_check_state++;
                if(counter_check_state > 25)
                {
                    if(query_state == false)
                    {
                        // alert("Please re-type new pair key"); 

                    } 
                    query_status_label.innerHTML = "";
                    clearInterval(check_sv_status);
                } 
                // if(check_sv_status > 50) clearInterval(check_sv_status);
            }, 100);
            let machine_type_endpoint = "machine-types";
            let mqtt_sever_endpoint = "mqtt-servers";
            
            getRequestHttp(CITUrl,`/api/v1/${e.value}/${machine_type_endpoint}`,"",(response, status) => {
            // console.log(response);
            let json_response = JSON.parse(response);
            query_status_label.style.color = "red";
            query_status_label.innerHTML = json_response.message;
            if(status == 200)
            {
                generate_machine_type_images(json_response);
                machine_type_query_status = 1;
                console.log(json_response);
                query_status_label.style.color = "green";
                query_status_label.innerHTML = "success";
                // console.log(json_response.data.length);
                // console.log(json_response.id);
            }
            if(status != 200)
            {
                // alert(json_response.message);
            }
            });
            getRequestHttp(CITUrl,`/api/v1/${e.value}/${mqtt_sever_endpoint}`,"",(response, status) => {
            // console.log(response);
            if(status == 200)
            {
                let json_response = JSON.parse(response);
                json_raw_server_infor = response;
                mqtt_sv_query_status = 1;
                console.log(json_response);
                // generate_machine_type_images(json_response);
                // console.log(json_response);
                // console.log(json_response.data.length);
                // console.log(json_response.id);
            }
            });
            // first_reach_to_6_chars++;
        }
        first_reach_to_6_chars++;
            // console.log(mqtt_sv_query_status);
        // console.log(machine_type_query_status);
    }
        console.log(first_reach_to_6_chars);
    // if(first_reach_to_6_chars > 0) first_reach_to_6_chars = 1;
    
    // if (first_reach_to_6_chars > 0)
    // {
    //     generate_machine_type_images("");
    // }
    // console.log(first_reach_to_6_chars);
    // else
    // {
    //     first_reach_to_6_length = 0;
    // }
    // if(first_reach_to_6_length == 1)
    // {
    // }
    // console.log(e.value.length);
}

function send_pair_device_data(){
    // btn_submit_send_pair_data.disabled = true;
    if(machine_type_query_status == 1 && mqtt_sv_query_status == 1)
    {
        // btn_submit_send_pair_data.disabled = false;
        let jsonData = generate_json_machine_sv_infor();
        postRequestHttp(baseUrl, "add_new_device","",jsonData,(dataResponse) => {
            // console.log(dataResponse); //debug load data from server
            alert(dataResponse);
            // generate_content_netWorkConfig_tab(dataResponse);
        });
    }
    else
    {
        // btn_submit_send_pair_data.disabled = true;
    }
}