function generatePage()
{
    // console.log(baseUrl)
    // alert('test!!!');
    // change_protocol_mode();
    generate_mqtt_client_id();
    change_wifi_mode();
    generate_content();
    get_fw_fs_version();
    ping_device();
    // handle_query_machine_types();
    // handle_choose_machine_types();
    // send_pair_device_data();
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


let intervalId = null;
let currentInterval = 0; // store current interval time


function getReqPt100Data()
{
    let now = new Date();
    console.log(now);
    getRequestHttp(baseUrl,"pt100_data","",(dataResponse) =>{
        // console.log("dataResponse");
        // console.log(dataResponse);
        let obj = JSON.parse(dataResponse);
        obj.temp = Math.round(obj.temp * 100) / 100;

        
        document.getElementById("temp_data").innerHTML = obj.temp;
        // document.getElementById("temp_data").innerHTML = temp_data;
        // document.getElementsByClassName("fw_fs_version")[0].innerHTML = "FW:" + " " + obj.fw_ver;
        // document.getElementsByClassName("fw_fs_version")[1].innerHTML = document.getElementsByClassName("fw_fs_version")[1].innerHTML + " " + obj.fs;
        // if(dataResponse == "OK") alert("Device reseted!!!");
    });
}

function get_pt100_data()
{
    let newInterval = parseInt(document.getElementById("t_get_data").value);
    if(newInterval < 0 || newInterval > 30)
    {
        alert("Out of range !!!");
    }
    else
    {
        if(Number.isNaN(newInterval))
        {
            alert("Invalid value !!!");
        }
        else
        {
            // only restart if value actually changes
                newInterval *= 1000;
                if (newInterval !== currentInterval) {
                currentInterval = newInterval;
                if (intervalId) {
                    clearInterval(intervalId);
                    intervalId = null;
                }
                // start fresh
                intervalId = setInterval(getReqPt100Data, newInterval);
                console.log("Interval updated to:", newInterval, "ms");
            }
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





