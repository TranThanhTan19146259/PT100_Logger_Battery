function generatePage()
{
    // console.log(baseUrl)
    // alert('test!!!');
    // change_protocol_mode();
    change_wifi_mode();
    generate_content();
    get_fw_fs_version();
    handle_query_machine_types();
    handle_choose_machine_types();
    send_pair_device_data();
    // document.getElementsByClassName("fw_fs_version")[0].innerHTML = " 0.0.1";
    // get_data_mpu();
    // get_data_alarm_config();
    // update_fw();
}

function reset_device()
{
    getRequestHttp(baseUrl,"rs_device","",(dataResponse) =>{
        console.log(dataResponse);
        if(dataResponse == "OK") alert("Device reseted!!!");
    });
}

function get_fw_fs_version()
{
    getRequestHttp(baseUrl,"get_fw_fs_version","",(dataResponse) =>{
        console.log(dataResponse);
        const obj = JSON.parse(dataResponse);
        document.getElementsByClassName("fw_fs_version")[0].innerHTML = "FW:" + " " + obj.fw_ver;
        // document.getElementsByClassName("fw_fs_version")[1].innerHTML = document.getElementsByClassName("fw_fs_version")[1].innerHTML + " " + obj.fs;
        // if(dataResponse == "OK") alert("Device reseted!!!");
    });
}

function send_time_send()
{
    let timeSend = parseInt(document.getElementById("t_send_timeSend").value);
    if(timeSend < 30 || timeSend > 3 * 60)
    {
        alert("Out of range !!!");
    }
    else
    {
        if(Number.isNaN(timeSend))
        {
            alert("Invalid value !!!");
        }
        else
        {
            let myObj = {
                timeSend: parseInt(timeSend)
            };
            console.log(JSON.stringify(myObj));// DEBUG
            postRequestHttp(baseUrl,"pt100_data_timeSend","",JSON.stringify(myObj), (dataResponse) =>{
                // console.log(dataResponse);
                alert(dataResponse);
            });
        }
    }
}


function generate_content()
{
    const select_tab = document.getElementsByClassName("select-tab");
    const main_content = document.getElementsByClassName("main-content");
    // for (let i = 0; i < select_tab.length; i++) {
    //     // select_tab[i].classList.remove("active");
    //     main_content[i].style.display = "none";
    // }
    for(let i=0;i<select_tab.length;i++)
    {
        select_tab[i].addEventListener("click", function(){
            for (let i = 0; i < select_tab.length; i++) {
                select_tab[i].classList.remove("active");
                main_content[i].classList.remove("active-content");
            }
            if (!select_tab[i].classList.contains("active"))
            {
                select_tab[i].classList.add("active");
                main_content[i].classList.add("active-content");
            }
        })
    }
}



// function getReqHttp(url, endpoint,)





