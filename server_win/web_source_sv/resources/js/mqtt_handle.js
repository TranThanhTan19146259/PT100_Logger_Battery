// const url = 'ws://192.168.11.101:8083/mqtt'
const url = `ws://${mqtt_url}:8083/mqtt`
// Create an MQTT client instance
const options = {
  // Clean session
  clean: true,
  connectTimeout: 4000,
  // Authentication
  clientId: mqtt_client_id,
  username: mqtt_usr,
  password: mqtt_pass,
}
const client  = mqtt.connect(url, options)
client.on('connect', function () {
  console.log(mqtt_client_id);
  console.log('Connected')
  client.subscribe('PT100_logger/#', function (err) {
  })
})
let dev_status = 0;
let count_dev_timeout = 0;
// Receive messages
client.on('message', function (topic, message) {
  if(topic == "PT100_logger/data")
  {
    let obj = JSON.parse(message.toString());
    //put try catch here to catch response string from device
    try{
      // console.log(obj.response);
      obj.temp = Math.round(obj.temp * 100) / 100;
      if(Number.isNaN(obj.temp))
      {
          // alert("Invalid value !!!");
      }
      else
      {
        document.getElementById("temp_data").innerHTML = obj.temp;
      }
      if(obj.response === undefined)
      {
      }
      else
      {
        alert(obj.response);

      }
      const now = new Date();
      dev_status = 1;
      let str_datetime = now.toString().slice(0, 24);
      document.getElementById("last_data").innerHTML = String(str_datetime);
      // console.log(obj)
    }
    catch
    {

    }
  }
  if(topic == "PT100_logger/status")
  {
    const now = new Date();
    // let str_datetime = String(now.getDate()) + " " + String(now.getTime())
    console.log(now);
    // console.log(obj);
    // dev_status = 1;
    count_dev_timeout = 0;
    dev_status = 1;
    // console.log("online");
    console.log(message.toString());
  }
})

function ping_device()
{
  setInterval(function () {
    client.publish('PT100_logger/info/status', "ping")
    count_dev_timeout++;
    if (dev_status == 0) document.getElementById("status_data").innerHTML = "OFFLINE";
    else document.getElementById("status_data").innerHTML = "ONLINE";
    if(count_dev_timeout > 5)
    {
      dev_status = 0;
      count_dev_timeout = 0;
      // console.log("offline");
    }
}, 1000);
}

function send_time_get_pt100_data()
{
    let newInterval = parseInt(document.getElementById("t_get_data").value);
    if(newInterval < 30 || newInterval > 3 * 60)
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
            const myObject = {
                timeSend: newInterval,
            };
            // console.log(myObject);
            client.publish('PT100_logger/config/timeSend', JSON.stringify(myObject));
        }
    }
}

