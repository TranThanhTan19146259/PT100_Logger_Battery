const url = 'ws://192.168.11.101:8083/mqtt'
// Create an MQTT client instance
const options = {
  // Clean session
  clean: true,
  connectTimeout: 4000,
  // Authentication
  clientId: 'emqx_test',
  username: 'Indr_Pt100',
  password: '123456789',
}
const client  = mqtt.connect(url, options)
client.on('connect', function () {
  console.log('Connected')
  client.subscribe('PT100_logger/#', function (err) {
  })
})

// Receive messages
client.on('message', function (topic, message) {
  if(topic == "PT100_logger/data")
  {
    let obj = JSON.parse(message.toString());
    obj.temp = Math.round(obj.temp * 100) / 100;
    document.getElementById("temp_data").innerHTML = obj.temp;
    const now = new Date();
    console.log(now);
    console.log(obj);
  }
})

function send_time_get_pt100_data()
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
            const myObject = {
                timeSend: newInterval,
            };
            // console.log(myObject);
            client.publish('PT100_logger/config/timeSend', JSON.stringify(myObject));
        }
    }
}

