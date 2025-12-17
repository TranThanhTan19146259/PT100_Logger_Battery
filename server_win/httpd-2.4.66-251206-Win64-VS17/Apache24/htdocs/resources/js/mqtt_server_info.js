let mqtt_url = "192.168.1.104"
let mqtt_client_id = ""
let mqtt_usr = "Indr_Pt100"
let mqtt_pass = "123456789"

function generate_mqtt_client_id()
{
    var length = 12; // the text numbers 
    var result           = '';
    var characters       = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';
    var charactersLength = characters.length;
    for ( var i = 0; i < length; i++ ) {
        result += characters.charAt(Math.floor(Math.random() * charactersLength));
    }
    mqtt_client_id = "Indr_Pt100_client_" + result 
    // console.log(mqtt_client_id);
}
