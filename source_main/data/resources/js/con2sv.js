let baseUrl = `http://${window.location.hostname}/`;

let CITUrl = "https://mozanio.citgroup.vn"

let getRequestHttp = function(url, endPoint, dataUrl, callBack) {
    var xhttp = new XMLHttpRequest();
    // console.log("GET"); //debug
    let request_status;
    xhttp.onreadystatechange = async function() {
        if (this.readyState == 4 && this.status == 200) {
            response = this.responseText;
            // console.log(response); //debug
            callBack(this.responseText, this.status);
            // callBack(this.status);
            // request_status = this.status;
            // console.log("OK"); //debug
        }
        else 
        {
            callBack(this.responseText, this.status);
            // callBack(this.responseText);
            // callBack(this.status);
            // request_status = this.status;
        }
    };
    xhttp.open("GET", url + endPoint + dataUrl, true);
    xhttp.send();
}


let postRequestHttp = function(url, endPoint, dataUrl, dataJson, callBack) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = async function() {
        if (this.readyState == 4 && this.status == 200) {
            response = this.responseText;
            // console.log(response); //debug
            callBack(this.responseText);
            // return response;
            // return this.responseText;
        }
        // else 
        // checkTimeoutServer(5000,this.status);
    };
    xhttp.open("POST", url + endPoint + dataUrl, true);
    xhttp.setRequestHeader("Content-type", "application/json");
    xhttp.send(dataJson);
}